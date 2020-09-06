#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "aml.h"

auto sum = [](auto a, auto b) { return a + b; };

TEST_CASE("stmt int", "[int]") {

  std::string input = "(int 1)";
  std::string output = "1";
  std::string code = {};

  {
    std::stringstream log;
    bool success = aml::aml_n::compile(input, code, log);
    REQUIRE(success);
  }

  {
    std::stringstream log;
    bool success = aml::aml_n::execute(code, log);
    REQUIRE(success);
    REQUIRE(output == log.str());
  }
}

TEST_CASE("stmt int 2", "[int]") {

  std::string input = "(int -1)";
  std::string output = "-1";
  std::string code = {};

  {
    std::stringstream log;
    bool success = aml::aml_n::compile(input, code, log);
    REQUIRE(success);
  }

  {
    std::stringstream log;
    bool success = aml::aml_n::execute(code, log);
    REQUIRE(success);
    REQUIRE(output == log.str());
  }
}
