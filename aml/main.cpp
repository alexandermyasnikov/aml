#include <iostream>
#include <variant>
#include <iomanip>
#include <regex>

#include "utils.h"
#include "lisp_tree.h"
#include "token.h"
#include "stmt.h"
#include "executor.h"

#include "debug_logger.h"

#define DEBUG_LOGGER_TRACE_LA            DEBUG_LOGGER("la   ", logger_indent_aml_t::indent)
#define DEBUG_LOGGER_LA(...)             DEBUG_LOG("la   ", logger_indent_aml_t::indent, __VA_ARGS__)

#define DEBUG_LOGGER_TRACE_SA            // DEBUG_LOGGER("sa   ", logger_indent_aml_t::indent)
#define DEBUG_LOGGER_SA(...)             DEBUG_LOG("sa   ", logger_indent_aml_t::indent, __VA_ARGS__)

#define DEBUG_LOGGER_TRACE_ICG           // DEBUG_LOGGER("icg  ", logger_indent_aml_t::indent)
#define DEBUG_LOGGER_ICG(...)            DEBUG_LOG("icg  ", logger_indent_aml_t::indent, __VA_ARGS__)

#define DEBUG_LOGGER_TRACE_CG            // DEBUG_LOGGER("cg   ", logger_indent_aml_t::indent)
#define DEBUG_LOGGER_CG(...)             // DEBUG_LOG("cg   ", logger_indent_aml_t::indent, __VA_ARGS__)

#define DEBUG_LOGGER_TRACE_EXEC          DEBUG_LOGGER("exec ", logger_indent_aml_t::indent)
#define DEBUG_LOGGER_EXEC(...)           DEBUG_LOG("exec ", logger_indent_aml_t::indent, __VA_ARGS__)

template <typename T>
struct logger_indent_t { static inline int indent = 0; };

struct logger_indent_aml_t : logger_indent_t<logger_indent_aml_t> { };



namespace aml_n {

  namespace lexical_analyzer_n {
    auto process(const std::string& code) {
      return aml::token_n::process(code);
    }
  }

  namespace syntax_lisp_analyzer_n {
    auto process(const aml::token_n::tokens_t& tokens) {
      return aml::lisp_tree_n::process(tokens);
    }
  }

  namespace syntax_analyzer_n {
    auto process(const aml::lisp_tree_n::lisp_tree_t& tree) {
      return std::make_shared<aml::stmt_n::stmt_program_t>(tree);
    }
  }

  namespace semantic_analyzer_n {
  }

  namespace intermediate_code_generator_n {
    auto process(std::shared_ptr<aml::stmt_n::stmt_t> stmt) {
      aml::code_n::code_ctx_t code_ctx;
      stmt->intermediate_code(code_ctx);
      return code_ctx;
    }
  }

  namespace code_optimizer_n {
  }

  namespace code_generator_n {
  }

  namespace executor_n {
    void process(const aml::code_n::code_ctx_t& code_ctx) {
      aml::exec_n::machine_t machine;
      machine.rip = code_ctx.rip;
      machine.rbp = {};

      while (machine.step(code_ctx.code)) {
      }

      if (machine.stack.size() == 1) {
        std::cout << machine.stack.front() << std::endl;
      } else {
        std::cerr << "invalid output" << std::endl;
      }
    }
  }
}

struct interpreter_t {

  void exec(const std::string code) {
    using namespace aml_n;

    auto tokens = lexical_analyzer_n::process(code);
    DEBUG_LOGGER_LA("tokens: \n%s", aml::token_n::show_tokens(tokens).c_str());

    auto syntax_lisp_tree = syntax_lisp_analyzer_n::process(tokens);
    DEBUG_LOGGER_SA("syntax_lisp_tree: \n%s", syntax_lisp_tree.show({}).c_str());

    auto stmt = syntax_analyzer_n::process(syntax_lisp_tree);
    DEBUG_LOGGER_SA("syntax_tree: \n%s", stmt->show({}).c_str());

    auto code_ctx = intermediate_code_generator_n::process(stmt);
    DEBUG_LOGGER_ICG("intermediate_code: \n%s", code_ctx.code.show().c_str());

    executor_n::process(code_ctx);
  }
};



int main(int argc, char* argv[]) {
  std::string code;

  if (argc < 2) {
    std::cerr << "usage: ./<program> <code>" << std::endl;
    return 1;
  }

  if (argc > 1) {
    code = argv[1];
  }

  // std::cout << "code: '\n" << code << "'" << std::endl;

  interpreter_t interpreter;
  interpreter.exec(code);

  return 0;
}

