#include <iostream>
#include <fstream>
#include <boost/program_options.hpp>

#include "utils.h"
#include "lisp_tree.h"
#include "token.h"
#include "stmt.h"
#include "executor.h"



namespace aml_n {

  namespace lexical_analyzer_n {
    auto process(const std::string& code, std::stringstream& log) {
      auto tokens = aml::token_n::process(code);

      log << aml::utils_n::separator_line;
      log << aml::utils_n::separator_start << "source code" << std::endl;
      log << aml::utils_n::separator_line;
      log << code;

      log << aml::utils_n::separator_line;
      log << aml::utils_n::separator_start << "tokens" << std::endl;
      log << aml::utils_n::separator_line;
      log << aml::token_n::show_tokens(tokens) << std::endl;
      return tokens;
    }
  }

  namespace syntax_lisp_analyzer_n {
    auto process(const aml::token_n::tokens_t& tokens, std::stringstream& log) {
      auto lisp_tree = aml::lisp_tree_n::process(tokens);

      log << aml::utils_n::separator_line;
      log << aml::utils_n::separator_start << "lisp_tree" << std::endl;
      log << aml::utils_n::separator_line;
      log << lisp_tree.show({}) << std::endl;
      return lisp_tree;
    }
  }

  namespace syntax_analyzer_n {
    auto process(const aml::lisp_tree_n::lisp_tree_t& tree, std::stringstream& log) {
      auto stmt = std::make_shared<aml::stmt_n::stmt_program_t>(tree);

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
    auto process(std::shared_ptr<aml::stmt_n::stmt_t> stmt, std::stringstream& log) {
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



std::string str_from_file(const std::string& path) {
  std::ifstream file(path);
  std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
  return str;
}

void str_to_file(const std::string& str, const std::string& path) {
  std::ofstream file(path);
  file << str;
}



struct options_t {
  bool        quiet  = false;
  std::string input  = {};
  std::string output = {};
  std::string cmd    = {};
  std::string log    = {};

  std::string show() {
    std::string str;
    std::cout << "input:  " << input << std::endl;
    std::cout << "output: " << output << std::endl;
    std::cout << "cmd:    " << cmd << std::endl;
    std::cout << "log:    " << log << std::endl;
    return str;
  }

  bool parse(int argc, char* argv[]) {
    try {
      using namespace boost::program_options;

      options_description desc{"Options"};
      desc.add_options()
        ("help,h",                              "Help screen")
        ("quiet,q",  value(&quiet),             "Print less information")
        ("input,i",  value(&input)->required(), "Read from file")
        ("output,i", value(&output),            "Write to file")
        ("cmd,c",    value(&cmd)->required(),   "Availible commands \"compile\" and \"execute\"")
        ("log,l",    value(&log),               "Write verbose output to file. Stdout is used if file is '-'")
        ;

      variables_map vm;
      store(parse_command_line(argc, argv, desc), vm);

      if (vm.count("help")) {
        std::cout << desc << std::endl;
        return false;
      }
      notify(vm);

    } catch (const std::exception& ex) {
      std::cerr << ex.what() << std::endl;
      return false;
    }

    return true;
  }
};


int main(int argc, char* argv[]) {

  options_t options = {};
  if (!options.parse(argc, argv)) {
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

    std::stringstream log;

    try {
      using namespace aml_n;
      auto code      = str_from_file(options.input);
      auto tokens    = lexical_analyzer_n::process(code, log);
      auto lisp_tree = syntax_lisp_analyzer_n::process(tokens, log);
      auto stmt      = syntax_analyzer_n::process(lisp_tree, log);
      auto code_ctx  = intermediate_code_generator_n::process(stmt, log);
      str_to_file(code_ctx.save(), options.output);
    } catch (const std::exception& ex) {
      std::cerr << ex.what() << std::endl;
    }

    if (options.log.empty()) {
      ;
    } else if (options.log == "-") {
      std::cout << log.str();
    } else {
      str_to_file(log.str(), options.log);
    }

  } else if (options.cmd == "execute") {
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

