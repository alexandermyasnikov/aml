
#include "token.h"

namespace aml::token_n {

  std::string pos_t::show() const {
    return std::to_string(line)
      + ":" + std::to_string(column)
      + ":" + std::to_string(length);
  }



  bool token_t::is_primary() const {
    switch (type) {
      case type_t::unknown:
      case type_t::new_line:
      case type_t::whitespace:
      case type_t::eof:
        return false;
      default:
        return true;
    }
  }

  std::string token_t::show() const {
    switch (type) {
      case type_t::new_line:    return "\n";
      case type_t::whitespace:  return " ";
      case type_t::lp:          return "(";
      case type_t::rp:          return ")";
      case type_t::key_arg:     return "arg";
      case type_t::key_block:   return "block";
      case type_t::key_call:    return "call";
      case type_t::key_defn:    return "defn";
      case type_t::key_defvar:  return "defvar";
      case type_t::key_func:    return "func";
      case type_t::key_if:      return "if";
      case type_t::key_int:     return "int";
      case type_t::key_syscall: return "syscall";
      case type_t::key_var:     return "var";
      case type_t::integer:
                                {
                                  const auto* p = std::get_if<int64_t>(&value);
                                  return p ? std::to_string(*p) : "<integer>";
                                }
      case type_t::ident:
                                {
                                  const auto* p = std::get_if<std::string>(&value);
                                  return p ? *p : "<ident>";
                                }
      case type_t::eof:         return "\0";
      default:                  return "(unknown)";
    }
  }

  bool token_t::next(std::string::const_iterator& it, std::string::const_iterator ite) {
    if (it == ite) {
      pos.column += pos.length;
      pos.length  = 0;
      lexeme      = "\0";
      value       = {};
      type        = type_t::eof;
      return false;
    }

    std::smatch m;
    std::regex_constants::match_flag_type flags =
      std::regex_constants::match_continuous | std::regex_constants::match_not_null;

    const rule_t* match_rule = nullptr;
    std::smatch match_best;

    for (const auto& rule : rules) {
      if (!std::regex_search(it, ite, m, rule.regex, flags))
        continue;

      if (m.position())
        throw utils_n::fatal_error_t("token_t: unknown position at " + pos.show());

      if (m.length() > (!match_best.empty() ? match_best.length() : 0)) {
        match_best = m;
        match_rule = &rule;
      }
    }

    if (!match_rule) {
      return false;
    }

    it               += match_best.length();
    pos.column += pos.length;
    pos.length  = match_best.length();
    lexeme      = match_best.str();
    value       = match_rule->get_value(lexeme);
    type        = match_rule->type;

    if (match_rule->type == type_t::new_line) {
      pos.line++;
      pos.length = 0;
      pos.column = utils_n::column_start;
    }

    return true;
  }



  std::string show_tokens(const token_n::tokens_t& tokens) {
    std::string str;
    for (const auto& token : tokens) {
      str += token.show() + " ";
    }
    return str;
  }
}
