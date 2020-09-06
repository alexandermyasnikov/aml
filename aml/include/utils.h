#pragma once

#include <stdexcept>

namespace aml::utils_n {
  template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
  template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

  const static inline size_t column_start = 1;
  const static inline size_t line_start = 1;

  const static inline std::string separator_start = std::string(10, '*') + ' ';
  const static inline std::string separator_line  = std::string(80, '*') + '\n';

  static inline std::string indent(size_t count) {
    static size_t tab_size = 2;
    return std::string(count * tab_size, ' ');
  }

  struct fatal_error_t : std::runtime_error {
    fatal_error_t(const std::string& msg = "unknown error") : std::runtime_error(msg) { }
  };
}
