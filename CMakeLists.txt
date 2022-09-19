cmake_minimum_required(VERSION 3.15)

project(sl_lua_test LANGUAGES CXX)

set(proj_name "sl_lua_test")

set(source_dir ${PROJECT_SOURCE_DIR}/source)


add_subdirectory(${source_dir})

add_executable(${proj_name} sl_lua_test.cpp lua_test.lua)

target_link_libraries(${proj_name} PRIVATE
  sl_lua
)

target_include_directories(${proj_name}  PUBLIC
  ${source_dir}
)
