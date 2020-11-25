cmake_minimum_required(VERSION 3.10)

project(aml_compiler
  VERSION 0.7
  LANGUAGES CXX)



file(GLOB SRCFILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE HDRFILES "${CMAKE_CURRENT_SOURCE_DIR}/include/*")



set(FLAGS
  -Werror
  -Wall
  -Wextra
  -Wpedantic
  -Wconversion
  -Wextra-semi
  -Wfloat-equal
  -Wlogical-op
  -Wnon-virtual-dtor
  -Wold-style-cast
  #-Wshadow=compatible-local
  #-Wsign-conversion
  -Wsign-promo
  -Wzero-as-null-pointer-constant

  -Wcast-align
  -Wcast-qual
  -Woverloaded-virtual
  -Wredundant-decls
  )

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS program_options REQUIRED)
find_package(Catch2 REQUIRED)
find_package(fmt REQUIRED)



add_library(aml SHARED ${SRCFILES})
target_compile_options(aml PUBLIC ${FLAGS})
target_include_directories(aml PUBLIC ${CMAKE_SOURCE_DIR}/include)
set_target_properties(aml PROPERTIES PUBLIC_HEADER "${HDRFILES}")
target_link_libraries(aml fmt::fmt)



add_executable(amlc main.cpp)
target_link_libraries(amlc PUBLIC ${Boost_LIBRARIES} aml)
# target_include_directories(amlc PUBLIC ${CMAKE_SOURCE_DIR}/3rdparty)



include(CTest)
include(Catch)
enable_testing()

add_executable(aml_tests test/main.cpp)
target_link_libraries(aml_tests aml)
catch_discover_tests(aml_tests WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

install(TARGETS aml amlc aml_tests
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION bin ${CMAKE_INSTALL_BINDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/amlc
  )
install(DIRECTORY aml/standard DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/amlc/aml)
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Alexander Myasnikov")
SET(CPACK_GENERATOR "${AML_PACKAGE_TYPE}")
INCLUDE(CPack)

