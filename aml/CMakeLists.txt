FILE(GLOB SRCFILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

find_package(Boost COMPONENTS program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

add_executable(aml "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp" ${SRCFILES})

set_property(TARGET aml PROPERTY CXX_STANDARD 20)
target_compile_options(aml PRIVATE
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
  -Wshadow=compatible-local
  #-Wsign-conversion
  -Wsign-promo
  -Wzero-as-null-pointer-constant

  -Wcast-align
  -Wcast-qual
  -Woverloaded-virtual
  -Wredundant-decls
  )

target_include_directories(aml PUBLIC
  "${PROJECT_SOURCE_DIR}/3rdparty"
  "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries(aml PUBLIC
  ${Boost_LIBRARIES})

# enable_testing()
# add_test(aml_tests aml_tests)
