#pragma once

#include <filesystem>
#include "lisp_tree.h"
#include "stmt.h"
#include "token.h"

namespace aml::aml_n {

  namespace lexical_analyzer_n {
    token_n::tokens_t process(const std::string& code);
  }



  namespace syntax_lisp_analyzer_n {
    lisp_tree_n::lisp_tree_t process(const token_n::tokens_t& tokens);
  }



  namespace syntax_analyzer_n {
    std::shared_ptr<stmt_n::stmt_t> process(const lisp_tree_n::lisp_tree_t& tree, const std::string& wd);
  }



  namespace semantic_analyzer_n {
  }



  namespace intermediate_code_generator_n {
    code_n::code_ctx_t process(std::shared_ptr<stmt_n::stmt_t> stmt);
  }



  namespace code_optimizer_n {
  }



  namespace code_generator_n {
  }



  namespace executor_n {
    std::string process(const code_n::code_ctx_t& code_ctx);
  }



  struct options_t {
    std::string file_output = {};
    std::string file_input  = {};
    std::string input       = {};
    std::string output      = {};
    std::string wd          = std::filesystem::current_path();
    std::string file_log    = {};
    std::string level       = {};
    std::string errors      = {};
    std::string cmd         = {};

    std::string show();
    void preprocessing();
    void postprocessing();
  };



  bool compile(options_t& options);
  bool execute(options_t& options);
  bool run(options_t& options);
}
