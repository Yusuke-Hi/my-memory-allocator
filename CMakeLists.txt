cmake_minimum_required(VERSION 3.22)
project(my-memory-allocator CXX)

add_library(${PROJECT_NAME} 
  src/mymalloc.cpp
  src/myfree.cpp
  src/memory-header.cpp
  src/coalesce.cpp
  src/allocator-internal.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC include)

add_subdirectory(test)
