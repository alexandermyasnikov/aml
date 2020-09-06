#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "aml.h"



struct test_case_t {
  std::string name   = {};
  std::string input  = {};
  std::string output = {};
  bool check_result  = true;
};



static inline std::vector<test_case_t> test_cases = {
  {
    .name   = "int 0",
    .input  = "(int 0)",
    .output = "0",
  }, {
    .name   = "int 1",
    .input  = "(int 1)",
    .output = "1",
  }, {
    .name   = "if then",
    .input  = "(if (int 1) (int 10) (int -10))",
    .output = "10",
  }, {
    .name   = "if else",
    .input  = "(if (int 0) (int 10) (int -10))",
    .output = "-10",
  },
};



TEST_CASE("core tests") {
  for (const auto& test_case : test_cases) {
    std::string name = "test_case " + test_case.name;
    SECTION(name) {
      std::string code;

      {
        std::stringstream log;
        bool success = aml::aml_n::compile(test_case.input, code, log);
        REQUIRE(success);
      }

      {
        std::stringstream log;
        bool success = aml::aml_n::execute(code, log);
        REQUIRE(success);
        if (test_case.check_result) {
          REQUIRE(test_case.output == log.str());
        }
      }
    }
  }
}

