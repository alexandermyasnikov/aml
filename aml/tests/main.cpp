#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"
#include "aml.h"



#define AML_TEST(name, output, input) \
TEST_CASE(name) {   \
  std::string code;   \
  {   \
    std::stringstream log;   \
    bool success = aml::aml_n::compile(input, code, "", log);   \
    REQUIRE(success);   \
  }   \
  {   \
    std::stringstream log;   \
    bool success = aml::aml_n::execute(code, log);   \
    REQUIRE(success);   \
    REQUIRE(output == log.str());   \
  }   \
}



AML_TEST("int 0", "0",
    "(defn test (int 0)) (call (func test))")

AML_TEST("int 1", "1",
    "(defn test (int 1)) (call (func test))")

AML_TEST("int -1", "-1",
    "(defn test (int -1)) (call (func test))")



AML_TEST("if then", "10",
    "(defn test (if (int 1) (int 10) (int -10))) (call (func test))")

AML_TEST("if else", "-10",
    "(defn test (if (int 0) (int 10) (int -10))) (call (func test))")

