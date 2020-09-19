#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"
#include "aml.h"



#define AML_TEST(name, expected, code) \
TEST_CASE(name) {   \
  using namespace aml::aml_n;   \
  options_t options = {   \
    .input = code,   \
  };   \
  {   \
    options.cmd = "compile";   \
    bool success_compile = run(options);   \
    INFO("errors: " << options.errors);   \
    REQUIRE(success_compile);   \
    REQUIRE(options.errors.empty());   \
  }   \
  options.input  = std::move(options.output);   \
  options.output = {};   \
  {   \
    options.cmd = "execute";   \
    bool success_execute = run(options);   \
    INFO("errors: " << options.errors);   \
    REQUIRE(success_execute);   \
    REQUIRE(options.errors.empty());   \
    REQUIRE(expected == options.output);   \
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



AML_TEST("arg1", "100",
    "(defn test (arg 1)) (call (func test) (int 100) (int 200) (int 300))")

AML_TEST("arg2", "200",
    "(defn test (arg 2)) (call (func test) (int 100) (int 200) (int 300))")

AML_TEST("arg3", "300",
    "(defn test (arg 3)) (call (func test) (int 100) (int 200) (int 300))")



AML_TEST("block1", "1",
    "(defn test (block (int 1))) (call (func test) (int 100) (int 200) (int 300))")

AML_TEST("block2", "2",
    "(defn test (block (int 1) (int 2))) (call (func test) (int 100) (int 200) (int 300))")



AML_TEST("var1", "12",
    "(defn test (block (defvar a (int 12)))) (call (func test))")

AML_TEST("var2", "12",
    "(defn test (block (defvar a (int 12)) (var a))) (call (func test))")

AML_TEST("var3", "10",
    "(defn test (block (defvar a (int 10)) (defvar b (int 11)) (defvar c (int 12)) (var a))) (call (func test))")

AML_TEST("var4", "11",
    "(defn test (block (defvar a (int 10)) (defvar b (int 11)) (defvar c (int 12)) (var b))) (call (func test))")

AML_TEST("var5", "12",
    "(defn test (block (defvar a (int 10)) (defvar b (int 11)) (defvar c (int 12)) (var c))) (call (func test))")



TEST_CASE("zigzag") {
  for (int64_t i = -1000; i < 1000; ++i) {
    int64_t a = aml::code_n::zigzag_encode(i);
    int64_t b = aml::code_n::zigzag_decode(a);
    REQUIRE(i == b);
  }
}

TEST_CASE("zigzag size") {
  REQUIRE(1 == aml::code_n::zigzag_size(0));
  REQUIRE(1 == aml::code_n::zigzag_size(1));
  REQUIRE(1 == aml::code_n::zigzag_size(0xFF));
  REQUIRE(2 == aml::code_n::zigzag_size(0x0100));
  REQUIRE(2 == aml::code_n::zigzag_size(0xFFFF));
}

