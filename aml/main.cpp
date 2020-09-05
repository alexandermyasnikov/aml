#include <iostream>
#include <fstream>
#include <boost/program_options.hpp>

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



std::string str_from_file(const std::string& path) {
  std::ifstream file(path);
  std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
  return str;
}

void str_to_file(const std::string& str, const std::string& path) {
  std::ofstream file(path);
  file << str;
}


int main(int argc, char* argv[]) {
  struct options_t {
    bool        quiet  = false;
    std::string input  = "";
    std::string output = "";
    std::string cmd    = "";
    std::string log    = "aml." + std::to_string(std::time(nullptr)) + ".log";

    std::string show() {
      std::string str;
      std::cout << "input:  " << input << std::endl;
      std::cout << "output: " << output << std::endl;
      std::cout << "cmd:    " << cmd << std::endl;
      std::cout << "log:    " << log << std::endl;
      return str;
    }
  };

  options_t options = {};

  try {
    using namespace boost::program_options;

    options_description desc{"Options"};
    desc.add_options()
      ("help,h",
        "help screen")
      ("quiet,q",
        value(&options.quiet),
        "Print less information")
      ("input,i",
        value(&options.input)->required(),
        "path of file")
      ("output,i",
        value(&options.output),
        "path of file")
      ("cmd,c",
        value(&options.cmd)->required(),
        "use \"compile\" or \"exec\"")
      ("log,l",
        value(&options.log),
        "path of log")
      ;

    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);

    if (vm.count("help")) {
      std::cout << desc << std::endl;
      return 0;
    }
    notify(vm);

  } catch (const std::exception& ex) {
    std::cerr << ex.what() << std::endl;
    return 1;
  }

  if (!options.quiet) {
    std::cout << options.show() << std::endl;
  }

  if (options.cmd == "compile") {
    if (options.output.empty()) {
      std::cerr << "Option output is not set" << std::endl;
      return 1;
    }

    try {
      using namespace aml_n;
      auto code      = str_from_file(options.input);
      auto tokens    = lexical_analyzer_n::process(code);
      auto lisp_tree = syntax_lisp_analyzer_n::process(tokens);
      auto stmt      = syntax_analyzer_n::process(lisp_tree);
      auto code_ctx  = intermediate_code_generator_n::process(stmt);
      str_to_file(code_ctx.save(), options.output);
    } catch (const std::exception& ex) {
      std::cerr << ex.what() << std::endl;
      return 1;
    }

  } else if (options.cmd == "exec") {
    try {
      aml::code_n::code_ctx_t code_ctx;
      code_ctx.load(str_from_file(options.input));
      aml_n::executor_n::process(code_ctx);
    } catch (const std::exception& ex) {
      std::cerr << ex.what() << std::endl;
      return 1;
    }

  } else {
    std::cerr << "Unknown cmd. Use --help" << std::endl;
  }

  return 0;
}

