#include <iostream>
#include <variant>
#include <iomanip>
#include <regex>

#include "utils.h"
#include "lisp_tree.h"
#include "token.h"

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



#if 0
  namespace syntax_analyzer_n {

    using namespace aml::utils_n;
    using namespace syntax_lisp_analyzer_n;

    // GRAMMAR
    // program: func+   expr
    // expr:    ARG     <digit>
    // expr:    BLOCK   expr+
    // expr:    CALL    expr    expr*
    // expr:    DEFVAR  <name>  expr
    // expr:    IF      expr    expr expr
    // expr:    INT     <digit>
    // expr:    SYSCALL expr+
    // expr:    VAR     <name>
    // func:    DEFN    expr    expr





















  ////////////////////////////////////////////////////////////////////////////////




  std::shared_ptr<stmt_t> process(const syntax_lisp_tree_t& tree) {
    return std::make_shared<stmt_program_t>(tree);
  }
}



  namespace semantic_analyzer_n {

    using namespace aml::utils_n;
    using namespace syntax_analyzer_n;
  }



  namespace intermediate_code_generator_n {

    using namespace aml::utils_n;
    using namespace syntax_analyzer_n;

    code_ctx_t process(std::shared_ptr<stmt_t> stmt) {
      code_ctx_t code_ctx;
      stmt->intermediate_code(code_ctx);
      return code_ctx;
    }
  }



  namespace code_optimizer_n {
  }



#if 0
  namespace code_generator_n {
    using namespace intermediate_code_generator_n;

    void process(data_t& text, const instructions_t& instructions) {
      text.assign(sizeof(instruction_t) * instructions.size(), 0);
      for (size_t i = 0; i < instructions.size(); ++i) {
        memcpy(text.data() + i * sizeof(instruction_t), &instructions[i].value, sizeof(instruction_t));
      }

      for (size_t i = 0; i < text.size(); i += sizeof(instruction_t)) {
        DEBUG_LOGGER_CG("text: '%02hhx%02hhx'", text.at(i), text.at(i + 1));
      }
    }
  }
#endif



  namespace executor_n {

    using namespace intermediate_code_generator_n;

    struct machine_t {
      std::deque<int64_t> stack;
      int64_t rbp = {};
      size_t rip = {};

      std::string show() {
        std::string str;
        str += "size: " + std::to_string(stack.size()) + "\t";
        str += "rbp: " + std::to_string(rbp) + "\t";
        str += "rip: " + std::to_string(rip) + "\t";
        str += "stack: ";
        for (const auto& v : stack) {
          str += std::to_string(v) + " ";
        }
        return str;
      }
    };

    bool step(machine_t& machine, const code_t& code) {
      DEBUG_LOGGER_EXEC("\t%s", machine.show().c_str());
      auto cmd = static_cast<instruction_rpn_t>(code.read_u8(machine.rip));
      DEBUG_LOGGER_EXEC("\tcmd: %s", code_t::show_instruction(cmd).c_str());

      if (machine.stack.size() > 1000)
        throw fatal_error_t("too big stack");

      switch (cmd) {
        case instruction_rpn_t::exit: {
          return false;

        } case instruction_rpn_t::arg: {
          int64_t value = code.read_i64(machine.rip);
          DEBUG_LOGGER_EXEC("value: %d", value);
          machine.stack.push_back(machine.stack.at(machine.stack.size() - value));
          DEBUG_LOGGER_EXEC("arg: %d", machine.stack.back());
          break;

        } case instruction_rpn_t::call: {
          if (machine.stack.empty())
            throw fatal_error_t("invalid stack");

          int64_t arg_count = machine.stack.back();
          DEBUG_LOGGER_EXEC("arg_count: %d", arg_count);
          if (arg_count < 1 || arg_count > machine.stack.size())
            throw fatal_error_t("invalid stack");

          for (size_t i{}; i < arg_count - 1; ++i) {
            int64_t arg = machine.stack.at(machine.stack.size() - 1 - 1/*arg_count*/ - i);
            DEBUG_LOGGER_EXEC("arg: %d", arg);
          }

          int64_t rip = machine.stack.at(machine.stack.size() - 1 - arg_count);
          DEBUG_LOGGER_EXEC("rip: %d", rip);

          machine.stack.push_back(machine.rbp);
          machine.stack.push_back(machine.rip);
          machine.rbp = machine.stack.size() - 1/*rbp*/ - 1/*rip*/;
          machine.rip = rip;
          break;

         } case instruction_rpn_t::jmp: {
          int64_t rip = code.read_i64(machine.rip);
          DEBUG_LOGGER_EXEC("rip: %d", rip);
          machine.rip = rip;
          break;

        } case instruction_rpn_t::pop_jif: {
          if (machine.stack.empty())
            throw fatal_error_t("invalid stack");

          int64_t ret = machine.stack.back();
          machine.stack.pop_back();
          DEBUG_LOGGER_EXEC("ret: %d", ret);

          int64_t rip = code.read_i64(machine.rip);
          DEBUG_LOGGER_EXEC("rip: %d", rip);

          if (!ret) {
            machine.rip = rip;
            DEBUG_LOGGER_EXEC("rip: %d", rip);
          }

          break;

        } case instruction_rpn_t::push8: {
          int64_t value = code.read_i64(machine.rip);
          machine.stack.push_back(value);
          DEBUG_LOGGER_EXEC("value: %d", value);
          break;

        } case instruction_rpn_t::ret: {
          if (machine.stack.size() < 4)
            throw fatal_error_t("invalid stack");

          int64_t ret = machine.stack.back();
          machine.stack.pop_back();

          machine.rip = machine.stack.back();
          machine.stack.pop_back();

          machine.rbp = machine.stack.back();
          machine.stack.pop_back();

          int64_t arg_count = machine.stack.back();
          machine.stack.pop_back();

          if (machine.stack.size() < arg_count)
            throw fatal_error_t("invalid stack");

          for (size_t i{}; i < arg_count; ++i) {
            machine.stack.pop_back();
          }

          machine.stack.push_back(ret);
          break;

        } case instruction_rpn_t::syscall: {
          if (machine.stack.empty())
            throw fatal_error_t("invalid stack");

          int64_t arg_count = machine.stack.back();
          machine.stack.pop_back();
          DEBUG_LOGGER_EXEC("arg_count: %d", arg_count);
          if (arg_count > machine.stack.size())
            throw fatal_error_t("invalid stack");




          int64_t ret = -1;
          int64_t op  = -1;
          if (arg_count > 0) {
            op = machine.stack.back();
            machine.stack.pop_back();
            arg_count--;

            static std::vector<std::function<int64_t(int64_t, int64_t)>> ops2 = {
              /*200*/ [](int64_t a, int64_t b) { return a + b; },
              /*201*/ [](int64_t a, int64_t b) { return a - b; },
              /*202*/ [](int64_t a, int64_t b) { return a * b; },
              /*203*/ [](int64_t a, int64_t b) { return !b ? 0 : a / b; },
              /*204*/ [](int64_t a, int64_t b) { return a == b; },
              /*205*/ [](int64_t a, int64_t b) { return a < b; },
              /*206*/ [](int64_t a, int64_t b) { return a && b; },
              /*207*/ [](int64_t a, int64_t b) { return a || b; },
            };

            if (arg_count == 1 && op == 100) {
              int64_t opnd1 = machine.stack.back();
              machine.stack.pop_back();
              arg_count--;
              ret = !opnd1;
              DEBUG_LOGGER_EXEC("%d: %d => %d", op, opnd1, ret);

            } else if (arg_count == 2 && op >= 200 && op < 200 + ops2.size()) {
              int64_t opnd1 = machine.stack.back();
              machine.stack.pop_back();
              arg_count--;
              int64_t opnd2 = machine.stack.back();
              machine.stack.pop_back();
              arg_count--;

              ret = ops2[op - 200](opnd1, opnd2);
              DEBUG_LOGGER_EXEC("%d: %d %d => %d", op, opnd1, opnd2, ret);
            }
          }

          if (arg_count) {
            DEBUG_LOGGER_EXEC("invalid syscall: %d : %d", op, arg_count);
            for (size_t i{}; i < arg_count; ++i) {
              int64_t arg = machine.stack.back();
              machine.stack.pop_back();
              DEBUG_LOGGER_EXEC("arg: %d", arg);
            }
          }

          machine.stack.push_back(ret);
          break;

        } default: throw fatal_error_t("unknown cmd");
      }
      return true;
    }

    void process(const code_ctx_t& code_ctx) {
      DEBUG_LOGGER_TRACE_EXEC;

      machine_t machine;
      machine.rip = code_ctx.rip;
      machine.rbp = {};

      while (step(machine, code_ctx.code)) {
      }

      if (machine.stack.size() == 1) {
        std::cout << machine.stack.front() << std::endl;
      }
    }
  }
#endif
}

struct interpreter_t {

  void exec(const std::string code) {
    using namespace aml_n;

    auto tokens = lexical_analyzer_n::process(code);
    DEBUG_LOGGER_LA("tokens: \n%s", aml::token_n::show_tokens(tokens).c_str());

    // auto syntax_lisp_tree = syntax_lisp_analyzer_n::process(tokens);
    // DEBUG_LOGGER_SA("syntax_lisp_tree: \n%s", syntax_lisp_tree.show({}).c_str());

    // auto stmt = syntax_analyzer_n::process(syntax_lisp_tree);
    // DEBUG_LOGGER_SA("syntax_tree: \n%s", stmt->show({}).c_str());

    // auto code_ctx = intermediate_code_generator_n::process(stmt);
    // DEBUG_LOGGER_ICG("intermediate_code: \n%s", code_ctx.code.show().c_str());

    // aml::utils_n::data_t text;
    // code_generator_n::process(text, instructions);

    // executor_n::process(code_ctx);
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

