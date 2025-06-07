#include "result.hpp"

namespace vk {
template <typename Ok, class Err>
auto Result<Ok, Err>::create(Ok value) -> Result {
  Result result(value);
  return result;
}

template <class Err> auto Result<void, Err>::create() -> Result<void, Err> {
  Result<void, Err> result;
  return result;
}

template <typename Ok, class Err>
auto Result<Ok, Err>::err(Err value) -> Result {
  Result result(value);
  return result;
}
} // namespace vk
