#pragma once

#include <string>
#include <deque>
#include <variant>
#include <regex>

#include "utils.h"

namespace aml::token_n {
  namespace utils_n = aml::utils_n;



  struct pos_t {
    size_t line   = utils_n::line_start;
    size_t column = utils_n::column_start;
    size_t length = 0;

    std::string show() const;
  };



  using value_t = std::variant<
    int64_t,
    std::string>;



  enum class type_t {
    unknown,
    new_line,
    whitespace,
    lp,
    rp,
    key_arg,
    key_block,
    key_call,
    key_defn,
    key_defvar,
    key_func,
    key_if,
    key_int,
    key_syscall,
    key_var,
    integer,
    ident,
    eof,
  };



  struct token_t {
    type_t      type   = type_t::unknown;
    pos_t       pos    = {};
    std::string lexeme = {};
    value_t     value  = {};

    bool is_primary() const;
    std::string show() const;
    bool next(std::string::const_iterator& it, std::string::const_iterator ite);
  };

  using tokens_t = std::deque<token_t>;



  struct rule_t {
    using value_cb_t = std::function<value_t(const std::string&)>;

    type_t     type;
    std::regex regex;
    value_cb_t get_value;
  };



  static inline std::vector<rule_t> rules = {
    {
      type_t::new_line,
      std::regex(R"(\n)"),
      [](const std::string&) { return value_t{}; },
    }, {
      type_t::whitespace,
      std::regex(R"([ \t]+)"),
      [](const std::string&) { return value_t{}; },
    }, {
      type_t::whitespace,
      std::regex(R"(;[^\n]*)"),
      [](const std::string&) { return value_t{}; },
    }, {
      type_t::lp,
      std::regex(R"(\()"),
      [](const std::string&) { return value_t{}; },
    }, {
      type_t::rp,
      std::regex(R"(\))"),
      [](const std::string&) { return value_t{}; },
    }, {
      type_t::key_arg,
        std::regex(R"(arg)"),
        [](const std::string&) { return value_t{}; },
    }, {
      type_t::key_block,
        std::regex(R"(block)"),
        [](const std::string&) { return value_t{}; },
    }, {
      type_t::key_call,
        std::regex(R"(call)"),
        [](const std::string&) { return value_t{}; },
    }, {
      type_t::key_defn,
        std::regex(R"(defn)"),
        [](const std::string&) { return value_t{}; },
    }, {
      type_t::key_defvar,
        std::regex(R"(defvar)"),
        [](const std::string&) { return value_t{}; },
    }, {
      type_t::key_func,
        std::regex(R"(func)"),
        [](const std::string&) { return value_t{}; },
    }, {
      type_t::key_if,
        std::regex(R"(if)"),
        [](const std::string&) { return value_t{}; },
    }, {
      type_t::key_int,
        std::regex(R"(int)"),
        [](const std::string&) { return value_t{}; },
    }, {
      type_t::key_syscall,
        std::regex(R"(syscall)"),
        [](const std::string&) { return value_t{}; },
    }, {
      type_t::key_var,
        std::regex(R"(var)"),
        [](const std::string&) { return value_t{}; },
    }, {
      type_t::integer,
        std::regex(R"([-+]?\d+)"),
        [](const std::string& lexeme) { return stol(lexeme); },
    }, {
      type_t::ident,
        std::regex(R"([\w<=>&|+\-\!:*^\/]+)"),
        [](const std::string& lexeme) { return lexeme; },
    },
  };



  std::string show_tokens(const tokens_t& tokens);
  tokens_t process(const std::string& code);
}
