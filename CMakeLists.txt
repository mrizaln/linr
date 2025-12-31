cmake_minimum_required(VERSION 3.12)

project(
  linr
  VERSION 0.1.0
  LANGUAGES CXX
  DESCRIPTION "Simple console input library"
)

string(
  COMPARE EQUAL
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_SOURCE_DIR}"
  LINR_STANDALONE
)

option(LINR_BUILD_EXAMPLES "Build examples" ${LINR_STANDALONE})
option(LINR_BUILD_TESTS "Build tests" ${LINR_STANDALONE})

add_library(linr INTERFACE)
target_include_directories(linr INTERFACE include)
target_compile_features(linr INTERFACE cxx_std_20)
set_target_properties(linr PROPERTIES CXX_EXTENSIONS OFF)

if(LINUX)
  target_compile_definitions(linr INTERFACE LINR_ENABLE_GETLINE)
endif()

if(LINR_BUILD_TESTS)
  add_subdirectory(test)
endif()

if(LINR_BUILD_EXAMPLES)
  add_subdirectory(example)
endif()
