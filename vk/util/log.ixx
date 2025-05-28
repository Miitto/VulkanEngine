module;

#include <format>
#include <iostream>
#include <print>
#include <source_location>

export module util:log;

namespace Color {
enum Code {
  FG_RED = 31,
  FG_GREEN = 32,
  FG_BLUE = 34,
  FG_DEFAULT = 39,
  BG_RED = 41,
  BG_GREEN = 42,
  BG_BLUE = 44,
  BG_DEFAULT = 49
};
class Modifier {

public:
  Code code;

  Modifier(Code pCode) : code(pCode) {}
  friend std::ostream &operator<<(std::ostream &os, const Modifier &mod) {
    return os << "\033[" << mod.code << "m";
  }
};

} // namespace Color

export namespace util {
template <class... Args>
void log(const std::format_string<Args...> str, Args &&...args) {
#ifndef NDEBUG
  std::cout << Color::Modifier(Color::FG_BLUE) << "LOG "
            << std::source_location::current().file_name() << ":"
            << std::source_location::current().line() << " | "
            << Color::Modifier(Color::FG_DEFAULT)
            << std::format(str, std::forward<Args>(args)...) << "\n";
#endif
}

template <class... Args>
void todo(const std::format_string<Args...> str, Args &&...args) {
#ifndef NDEBUG
  std::cout << Color::Modifier(Color::FG_GREEN) << "TODO "
            << std::source_location::current().file_name() << ":"
            << std::source_location::current().line() << " | "
            << Color::Modifier(Color::FG_DEFAULT)
            << std::format(str, std::forward<Args>(args)...) << "\n";
#endif
}

template <class... Args>
void log_err(const std::format_string<Args...> str, Args &&...args) {
  std::cerr << Color::Modifier(Color::FG_RED) << "ERR "
            << std::source_location::current().file_name() << ":"
            << std::source_location::current().line() << " | "
            << Color::Modifier(Color::FG_DEFAULT)
            << std::format(str, std::forward<Args>(args)...) << "\n";
}
} // namespace util
