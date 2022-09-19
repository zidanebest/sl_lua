project(sl_lua LANGUAGES CXX)


add_library(sl_lua STATIC 
    " "
)

file(GLOB_RECURSE _sources 
  *.cpp 
  *.h
  *.inl
  *.hpp  
)

target_sources(sl_lua PRIVATE
${_sources}
)

