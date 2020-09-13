#include "aml.h"

#include <iostream>
#include "logger.h"

namespace aml::aml_n {

  token_n::tokens_t lexical_analyzer_n::process(const std::string& code) {
    AML_TRACER;
    auto tokens = token_n::process(code);

    AML_LOGGER(info, "source code:\n{}", code);
    AML_LOGGER(info, "tokens:\n{}", token_n::show_tokens(tokens));
    return tokens;
  }



  lisp_tree_n::lisp_tree_t syntax_lisp_analyzer_n::process(const token_n::tokens_t& tokens) {
    AML_TRACER;
    auto lisp_tree = lisp_tree_n::process(tokens);

    AML_LOGGER(info, "lisp tree:\n{}", lisp_tree.show({}));
    return lisp_tree;
  }



  std::shared_ptr<stmt_n::stmt_t> syntax_analyzer_n::process(const lisp_tree_n::lisp_tree_t& tree, const std::string& wd) {
    AML_TRACER;
    auto options = stmt_n::options_t{.wd = wd};
    auto stmt = stmt_n::stmt_t::parse(tree, nullptr, {stmt_n::type_t::stmt_program}, options);

    AML_LOGGER(info, "stmt:\n{}", stmt->show({}));
    return stmt;
  }



  code_n::code_ctx_t intermediate_code_generator_n::process(std::shared_ptr<stmt_n::stmt_t> stmt) {
    AML_TRACER;
    code_n::code_ctx_t code_ctx;
    stmt->intermediate_code(code_ctx);

    AML_LOGGER(info, "intermediate code:\n{}", code_ctx.code.show());
    return code_ctx;
  }



  std::string executor_n::process(const code_n::code_ctx_t& code_ctx) {
    code_n::stack_t stack;
    stack.rip = code_ctx.rip;
    stack.rbp = {};

    for (size_t i{}; i < 1000000; ++i) {
      if (!stack.step(code_ctx.code)) {
        break;
      }
    }

    return stack.size() == 1
      ? std::to_string(stack.get({}))
      : "invalid output";
  }



  std::string options_t::show() {
    std::stringstream ss;
    ss << "file_input:  " << file_input  << std::endl;
    ss << "file_output: " << file_output << std::endl;
    ss << "input:       " << input       << std::endl;
    ss << "output:      " << output      << std::endl;
    ss << "wd:          " << wd          << std::endl;
    ss << "file_log:    " << file_log    << std::endl;
    ss << "cmd:         " << cmd         << std::endl;
    return ss.str();
  }

  void options_t::preprocessing() {
    if (!file_input.empty())
      input = utils_n::str_from_file(file_input);
  }

  void options_t::postprocessing() {
    if (file_output == "-") {
      std::cout << output;
    } else if (!file_output.empty())
      utils_n::str_to_file(output, file_output);
  }



  bool compile(options_t& options) {
    try {
      options.preprocessing();

      auto code      = options.input;
      auto tokens    = lexical_analyzer_n::process(code);
      auto lisp_tree = syntax_lisp_analyzer_n::process(tokens);
      auto stmt      = syntax_analyzer_n::process(lisp_tree, options.wd);
      auto code_ctx  = intermediate_code_generator_n::process(stmt);
      options.output = code_ctx.save();

      options.postprocessing();
    } catch (const std::exception& ex) {
      options.errors = ex.what();
      return false;
    }

    return true;
  }



  bool execute(options_t& options) {
    AML_TRACER;
    try {
      options.preprocessing();

      code_n::code_ctx_t code_ctx;
      auto code = options.input;
      code_ctx.load(code);
      auto output = executor_n::process(code_ctx);

      options.output = output;
      options.postprocessing();
    } catch (const std::exception& ex) {
      options.errors = ex.what();
      return false;
    }

    return true;
  }



  bool run(options_t& options) {
    logger_n::logger_t::init(options.file_log, options.level);
    AML_TRACER;

    if (options.cmd == "compile") {
      return compile(options);
    } else if (options.cmd == "execute") {
      return execute(options);
    } else {
      options.errors = "unknown cmd";
      return false;
    }
  }
}
