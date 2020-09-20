cmake_minimum_required(VERSION 3.10)

project(aml_project LANGUAGES CXX)



file(GLOB SRCFILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB HDRFILES "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")



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



set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS program_options REQUIRED)
find_package(Catch2 REQUIRED)
find_package(fmt REQUIRED)



add_library(aml_lib SHARED ${SRCFILES})
target_compile_options(aml_lib PUBLIC ${FLAGS})
target_include_directories(aml_lib PUBLIC include)
set_target_properties(aml_lib PROPERTIES OUTPUT_NAME aml CXX_STANDARD 20 PUBLIC_HEADER "${HDRFILES}")
target_link_libraries(aml_lib fmt::fmt)



add_executable(amlc main.cpp)
target_link_libraries(amlc PUBLIC ${Boost_LIBRARIES} aml_lib)
set_target_properties(amlc PROPERTIES CXX_STANDARD 20)
# target_include_directories(amlc PUBLIC ${CMAKE_SOURCE_DIR}/3rdparty)



include(CTest)
include(Catch)
enable_testing()

add_executable(aml_tests test/main.cpp)
set_property(TARGET aml_tests PROPERTY CXX_STANDARD 20)
target_link_libraries(aml_tests aml_lib)
catch_discover_tests(aml_tests WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})



install(TARGETS aml_lib amlc aml_tests
  PUBLIC_HEADER DESTINATION usr/include/aml/cpp
  )
install(DIRECTORY aml/ DESTINATION usr/include/aml)
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Alexander Mysnikov")
SET(CPACK_GENERATOR "DEB" "RPM" "TBZ2" "7Z")
INCLUDE(CPack)

