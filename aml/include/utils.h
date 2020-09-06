#pragma once

#include <stdexcept>
#include <fstream>

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



  static inline std::string str_from_file(const std::string& path) {
    std::ifstream file(path);
    std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return str;
  }

  static inline void str_to_file(const std::string& str, const std::string& path) {
    std::ofstream file(path);
    file << str;
  }



  struct fatal_error_t : std::runtime_error {
    fatal_error_t(const std::string& msg = "unknown error") : std::runtime_error(msg) { }
  };
}
