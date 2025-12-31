include(cmake/fetched-libs.cmake)

enable_testing()

add_executable(test source/test.cpp)
target_include_directories(test PRIVATE source)
target_link_libraries(test PRIVATE Boost::ut linr)

if(NOT MSVC)
  # sanitizer
  target_compile_options(test PRIVATE -Wall -Wextra -Wconversion)
  target_compile_options(test PRIVATE -fsanitize=address,leak,undefined)
  target_link_options(test PRIVATE -fsanitize=address,leak,undefined)
else()
  target_link_options(test PRIVATE /W4)
endif()

add_test(NAME test COMMAND $<TARGET_FILE:test>)
add_custom_command(
  TARGET test
  POST_BUILD
  COMMAND
    ${CMAKE_COMMAND} -E cat ${CMAKE_CURRENT_SOURCE_DIR}/test.input |
    $<TARGET_FILE:test>
)
