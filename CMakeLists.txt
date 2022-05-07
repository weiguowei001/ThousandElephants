cmake_minimum_required(VERSION 3.10.0)

project(thousand-elephants)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# thousand-elephants sources
# include(cmake/thousand-elephants-search.cmake)

set(thousand-elephants_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/main.cpp
    # ${CMAKE_CURRENT_LIST_DIR}/first/s1.cpp
    ${CMAKE_CURRENT_LIST_DIR}/first/s1.hpp
    )

add_executable(thousand-elephants ${thousand-elephants_SOURCES})
# target_precompile_headers(thousand-elephants PRIVATE ${CMAKE_CURRENT_LIST_DIR}/precompiledheader.h)