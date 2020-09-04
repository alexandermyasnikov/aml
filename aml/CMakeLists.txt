# add_library(aml
#   "src/math_base.cpp"
#   "src/math_vector.cpp"
#   )

# target_include_directories(aml PUBLIC
#   "${CMAKE_CURRENT_SOURCE_DIR}/include"
#   )



add_executable(aml "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")

# target_include_directories(math_tests PUBLIC
#   "${CMAKE_CURRENT_SOURCE_DIR}/include"
#   "${PROJECT_SOURCE_DIR}/3rdparty"
#   )

# target_link_libraries(aml_tests PUBLIC aml)

# enable_testing()
# add_test(aml_tests aml_tests)
