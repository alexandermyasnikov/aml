cmake_minimum_required(VERSION 3.10)

project(aml_project VERSION 0.5)

add_subdirectory(aml)

# add_executable(aml "${PROJECT_SOURCE_DIR}/src/main.cpp")

set_property(TARGET aml PROPERTY CXX_STANDARD 20)

target_include_directories(aml PUBLIC
  "${PROJECT_SOURCE_DIR}/3rdparty"
  )

# target_link_libraries(aml PUBLIC
#   math
#   flow
#   )

# target_include_directories(aml PUBLIC
#   "${PROJECT_BINARY_DIR}"
#   "${PROJECT_SOURCE_DIR}/libs"
#   "${PROJECT_SOURCE_DIR}/3rdparty"
#   )



enable_testing()
