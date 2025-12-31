function(create_exe name)
  add_executable(${name} source/${name}.cpp)
  target_include_directories(${name} PRIVATE source)
  target_link_libraries(${name} PRIVATE linr)

  if(NOT MSVC)
    target_compile_options(${name} PRIVATE -Wall -Wextra -Wconversion)

    # # sanitizer
    # target_compile_options(${name} PRIVATE -fsanitize=address,leak,undefined)
    # target_link_options(${name} PRIVATE -fsanitize=address,leak,undefined)
  else()
    target_compile_options(${name} PRIVATE /W4)
  endif()
endfunction()

create_exe(example)
create_exe(bench)
create_exe(bufread)
create_exe(custom_type)
