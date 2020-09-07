FILE(GLOB SRCFILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

set(Boost_USE_STATIC_LIBS ON)

SET(FLAGS
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



add_library(aml_lib SHARED ${SRCFILES})
target_compile_options(aml_lib PUBLIC ${FLAGS})
target_include_directories(aml_lib PUBLIC include)
set_target_properties(aml_lib PROPERTIES OUTPUT_NAME aml CXX_STANDARD 20)



find_package(Boost COMPONENTS program_options REQUIRED)
add_executable(aml_bin main.cpp)
target_include_directories(aml_bin PUBLIC include ${Boost_INCLUDE_DIR})
target_link_libraries(aml_bin PUBLIC ${Boost_LIBRARIES} aml_lib)
set_target_properties(aml_bin PROPERTIES OUTPUT_NAME aml CXX_STANDARD 20)



add_executable(aml_tests tests/main.cpp)
set_property(TARGET aml_tests PROPERTY CXX_STANDARD 20)
target_include_directories(aml_tests PUBLIC include ${CMAKE_SOURCE_DIR}/3rdparty)
target_link_libraries(aml_tests aml_lib)

enable_testing()
