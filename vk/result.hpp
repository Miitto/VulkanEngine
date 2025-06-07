#pragma once

#include <optional>

namespace vk {
template <typename Ok, class Err> class Result {
  union {
    Ok m_ok;
    Err m_err;
  };

  bool m_isOk;

public:
  Result(Ok value) : m_ok(std::move(value)), m_isOk(true) {}
  Result(Err value) : m_err(std::move(value)), m_isOk(false) {}

  static auto create(Ok value = Ok()) -> Result<Ok, Err>;
  static auto err(Err value) -> Result<Ok, Err>;

  [[nodiscard]] auto isOk() const -> bool { return m_isOk; }
  [[nodiscard]] auto isErr() const -> bool { return !m_isOk; }

  auto unwrap() -> Ok & {
    assert(m_isOk, "Attempted to unwrap an err");
    return m_ok;
  }

  auto unwrapErr() -> Err & {
    assert(!m_isOk, "Attempted to err unwrap an ok value");
    return m_err;
  }

  auto ok() -> std::optional<Ok> {
    if (m_isOk) {
      return m_ok;
    }
    return std::nullopt;
  }
};
} // namespace vk
