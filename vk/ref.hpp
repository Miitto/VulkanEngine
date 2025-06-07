#pragma once

#include <memory>

template <class T> class Reference {
protected:
  std::shared_ptr<T *> m_ref;

public:
  Reference() = delete;
  explicit Reference(T *ref) : m_ref(std::make_shared<T *>(ref)) {}
  static auto create(T &ref) -> Reference { return Reference(&ref); }
  static auto create(T *ref) -> Reference { return Reference(ref); }

  auto is(const T *const ref) const -> bool { return *m_ref == ref; }
  void set(T *ref) { *this->m_ref = ref; }
  void reset() { *this->m_ref = nullptr; }

  [[nodiscard]] auto has_value() const -> bool {
    bool sharedValid = *m_ref;
    bool rawValid = **m_ref;
    return sharedValid && rawValid;
  }

  auto rawPtr() const -> T * { return *m_ref; }

  auto operator*() const -> T & { return **m_ref; }
  auto operator->() const -> T * { return *m_ref; }
  auto value() const -> T & { return **m_ref; }
  operator T &() const { return **m_ref; }
};

template <class T, typename Raw> class RawRef : public Reference<T> {
public:
  explicit RawRef(T *ref) : Reference<T>(ref) {}
  auto raw() const -> Raw { return ***this->m_ref; }
  operator Raw() const { return ***this->m_ref; }
};

template <class T> class Refable {
protected:
  Reference<T> m_ref = Reference<T>(nullptr);

public:
  Refable() : m_ref(static_cast<T *>(this)) {}

  auto ref() -> Reference<T> & { return m_ref; }

  Refable(const Refable &) = delete;
  auto operator=(const Refable &) -> Refable & = delete;

  Refable(Refable &&o) noexcept : m_ref(o.m_ref) {
    m_ref.set(static_cast<T *>(this));
  }

  auto operator=(Refable &&o) noexcept -> Refable & {
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

  auto ref() const -> RawRef<T, Raw> { return m_ref; }

  RawRefable(const RawRefable &) = delete;
  auto operator=(const RawRefable &) -> RawRefable & = delete;

  RawRefable(RawRefable &&o) noexcept : m_ref(o.m_ref) {
    m_ref.set(static_cast<T *>(this));
  }

  operator bool() const { return m_ref.is(static_cast<const T *>(this)); }

  auto operator=(RawRefable &&o) noexcept -> RawRefable & {
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
