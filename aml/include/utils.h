
#pragma once

#include <stdexcept>

namespace aml::utils_n {

  template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
  template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

  static inline size_t column_start = 1;
  static inline size_t line_start = 1;

  static std::string indent(size_t count) {
    static size_t tab_size = 2;
    return std::string(count * tab_size, ' ');
  }

  struct fatal_error_t : std::runtime_error {
    fatal_error_t(const std::string& msg = "unknown error") : std::runtime_error(msg) { }
  };
}
