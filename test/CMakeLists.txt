# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.27)

project(test LANGUAGES CXX)

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE main.cpp)

target_compile_definitions(${PROJECT_NAME}
                           PRIVATE "$<$<CXX_COMPILER_ID:MSVC>:NOMINMAX>")

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

set_target_properties(${PROJECT_NAME} PROPERTIES CMAKE_CXX_STANDARD_REQUIRED ON
                                                 CMAKE_CXX_EXTENSIONS OFF)

target_link_libraries(${PROJECT_NAME} PRIVATE libzstdstream)
