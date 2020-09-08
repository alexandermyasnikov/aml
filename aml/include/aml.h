#pragma once

#include "utils.h"
#include "utils.h"
#include "lisp_tree.h"
#include "token.h"
#include "stmt.h"
#include "executor.h"

namespace aml::aml_n {
  namespace utils_n = aml::utils_n;



  namespace lexical_analyzer_n {
    static inline auto process(const std::string& code, std::stringstream& log) {
      auto tokens = token_n::process(code);

      log << utils_n::separator_line;
      log << utils_n::separator_start << "source code" << std::endl;
      log << utils_n::separator_line;
      log << code;

      log << utils_n::separator_line;
      log << utils_n::separator_start << "tokens" << std::endl;
      log << utils_n::separator_line;
      log << token_n::show_tokens(tokens) << std::endl;
      return tokens;
    }
  }



  namespace syntax_lisp_analyzer_n {
    static inline auto process(const aml::token_n::tokens_t& tokens, std::stringstream& log) {
      auto lisp_tree = aml::lisp_tree_n::process(tokens);

      log << aml::utils_n::separator_line;
      log << aml::utils_n::separator_start << "lisp_tree" << std::endl;
      log << aml::utils_n::separator_line;
      log << lisp_tree.show({}) << std::endl;
      return lisp_tree;
    }
  }



  namespace syntax_analyzer_n {
    static inline auto process(const aml::lisp_tree_n::lisp_tree_t& tree, std::stringstream& log) {
      auto stmt = aml::stmt_n::stmt_t::parse(tree, nullptr, {aml::stmt_n::type_t::stmt_program});

      log << aml::utils_n::separator_line;
      log << aml::utils_n::separator_start << "stmt" << std::endl;
      log << aml::utils_n::separator_line;
      log << stmt->show({}) << std::endl;
      return stmt;
    }
  }



  namespace semantic_analyzer_n {
  }



  namespace intermediate_code_generator_n {
    static inline auto process(std::shared_ptr<aml::stmt_n::stmt_t> stmt, std::stringstream& log) {
      aml::code_n::code_ctx_t code_ctx;
      stmt->intermediate_code(code_ctx);

      log << aml::utils_n::separator_line;
      log << aml::utils_n::separator_start << "intermediate code" << std::endl;
      log << aml::utils_n::separator_line;
      log << code_ctx.code.show() << std::endl;
      return code_ctx;
    }
  }



  namespace code_optimizer_n {
  }



  namespace code_generator_n {
  }



  namespace executor_n {
    static inline void process(const aml::code_n::code_ctx_t& code_ctx, std::stringstream& log) {
      aml::exec_n::machine_t machine;
      machine.rip = code_ctx.rip;
      machine.rbp = {};

      while (machine.step(code_ctx.code)) {
      }

      if (machine.stack.size() == 1) {
        log << machine.stack.front();
      } else {
        log << std::endl << "invalid output" << std::endl;
      }
    }
  }



  static inline bool compile(const std::string& input, std::string& output, std::stringstream& log) {
    try {
      auto tokens    = lexical_analyzer_n::process(input, log);
      auto lisp_tree = syntax_lisp_analyzer_n::process(tokens, log);
      auto stmt      = syntax_analyzer_n::process(lisp_tree, log);
      auto code_ctx  = intermediate_code_generator_n::process(stmt, log);
      output         = code_ctx.save();
    } catch (const std::exception& ex) {
      log << std::endl << ex.what() << std::endl;
      return false;
    }

    return true;
  }



  static inline bool execute(const std::string& input, std::stringstream& log) {
    try {
      aml::code_n::code_ctx_t code_ctx;
      code_ctx.load(input);
      aml_n::executor_n::process(code_ctx, log);
    } catch (const std::exception& ex) {
      log << std::endl << ex.what() << std::endl;
      return false;
    }

    return true;
  }
}
