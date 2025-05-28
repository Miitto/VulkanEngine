module;

#include <memory>

export module vk:ref;

template <class T> class Reference {
protected:
  std::shared_ptr<T *> m_ref;

public:
  Reference() = delete;
  explicit Reference(T *ref) : m_ref(std::make_shared<T *>(ref)) {}
  static Reference create(T &ref) { return Reference(&ref); }
  static Reference create(T *ref) { return Reference(ref); }

  bool is(const T *const ref) const { return *m_ref == ref; }
  void set(T *ref) { *this->m_ref = ref; }
  void reset() { *this->m_ref = nullptr; }

  [[nodiscard]] bool has_value() const { return *m_ref && **m_ref; }

  T *rawPtr() const { return *m_ref; }

  T &operator*() const { return **m_ref; }
  T &value() const { return **m_ref; }
  operator T &() const { return **m_ref; }
};

template <class T, typename Raw> class RawRef : public Reference<T> {
public:
  explicit RawRef(T *ref) : Reference<T>(ref) {}
  Raw raw() const { return ***this->m_ref; }
  operator Raw() const { return ***this->m_ref; }
};

template <class T> class Refable {
protected:
  Reference<T> m_ref = Reference<T>(nullptr);

public:
  Refable() : m_ref(static_cast<T *>(this)) {}

  Reference<T> &ref() { return m_ref; }

  Refable(const Refable &) = delete;
  Refable &operator=(const Refable &) = delete;

  Refable(Refable &&o) noexcept : m_ref(o.m_ref) {
    m_ref.set(static_cast<T *>(this));
  }

  Refable &operator=(Refable &&o) noexcept {
    if (this != &o) {
      m_ref = std::move(o.m_ref);
      o.m_ref.set(this);
    }
    return *this;
  }

  ~Refable() {
    if (m_ref.is(static_cast<T *>(this))) {
      m_ref.reset();
    }
  }
};

template <class T, typename Raw> class RawRefable {
protected:
  RawRef<T, Raw> m_ref = RawRef<T, Raw>(nullptr);

public:
  RawRefable() : m_ref(static_cast<T *>(this)) {}

  RawRef<T, Raw> ref() const { return m_ref; }

  RawRefable(const RawRefable &) = delete;
  RawRefable &operator=(const RawRefable &) = delete;

  RawRefable(RawRefable &&o) noexcept : m_ref(o.m_ref) {
    m_ref.set(static_cast<T *>(this));
  }

  operator bool() const { return m_ref.is(static_cast<const T *>(this)); }

  RawRefable &operator=(RawRefable &&o) noexcept {
    if (this != &o) {
      m_ref = std::move(o.m_ref);
      o.m_ref.set(this);
    }
    return *this;
  }

  ~RawRefable() {
    if (m_ref.is(static_cast<T *>(this))) {
      m_ref.reset();
    }
  }
};
