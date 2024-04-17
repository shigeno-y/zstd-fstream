# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.27)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
set(CONAN_CMAKE_SILENT_OUTPUT TRUE)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project(zstdstream LANGUAGES CXX)

execute_process(
  COMMAND git -C ${CMAKE_SOURCE_DIR} rev-parse HEAD
  OUTPUT_VARIABLE GIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(
  COMMAND git -C ${CMAKE_SOURCE_DIR} rev-parse --abbrev-ref HEAD
  OUTPUT_VARIABLE GIT_BRANCH
  OUTPUT_STRIP_TRAILING_WHITESPACE)

add_compile_options(
  "$<$<C_COMPILER_ID:MSVC>:/MP>"
  "$<$<CXX_COMPILER_ID:MSVC>:/MP>"
  "$<$<C_COMPILER_ID:MSVC>:/utf-8>"
  "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>"
  "$<$<C_COMPILER_ID:MSVC>:/permissive->"
  "$<$<CXX_COMPILER_ID:MSVC>:/permissive->")
add_subdirectory(libzstdstream)

include(cmake/utils.cmake)
get_all_targets(targets ${CMAKE_CURRENT_SOURCE_DIR})
foreach(target ${targets})
  get_target_property(current_name ${target} NAME)
  get_target_property(current ${target} SOURCE_DIR)
  if(current)
    cmake_path(GET current FILENAME current_parent)
    if((current_name STREQUAL current) OR (current_name STREQUAL current_parent
                                          ))
      cmake_path(GET current PARENT_PATH current)
    endif()
    message("${current_name}\t${current}\t${current_parent}")
    file(RELATIVE_PATH current ${CMAKE_SOURCE_DIR} ${current})
    set_target_properties(${target} PROPERTIES FOLDER "${current}")
  endif(current)
endforeach(target ${targets})
