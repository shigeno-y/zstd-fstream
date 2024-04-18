# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.27)

project(libzstdstream LANGUAGES CXX)

add_library(${PROJECT_NAME})
target_sources(
  ${PROJECT_NAME}
  PUBLIC FILE_SET
         HEADERS
         TYPE
         HEADERS
         BASE_DIRS
         include
         FILES
         include/zstdstream/zstdstream.hpp
  #
)
target_sources(
  ${PROJECT_NAME}
  PRIVATE FILE_SET
          internalHeader
          TYPE
          HEADERS
          BASE_DIRS
          internal_include
          FILES
          internal_include/zstdstream/zstdstream_impl.hpp
  #
)
target_sources(${PROJECT_NAME} PRIVATE src/zstdstream.cpp)

target_include_directories(
  ${PROJECT_NAME}
  PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/internal_include>
          $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
         $<INSTALL_INTERFACE:include>)

target_compile_definitions(${PROJECT_NAME}
                           PRIVATE "$<$<CXX_COMPILER_ID:MSVC>:NOMINMAX>")

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

set_target_properties(${PROJECT_NAME} PROPERTIES CMAKE_CXX_STANDARD_REQUIRED ON
                                                 CMAKE_CXX_EXTENSIONS OFF)

find_package(zstd REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC zstd::libzstd_static)

find_package(Poco REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC Poco::Poco)

if(MSVC)
  get_target_property(_s ${PROJECT_NAME} HEADER_SET_HEADERS)
  if(_s)
    source_group(
      TREE "${PROJECT_SOURCE_DIR}/include"
      PREFIX "\\Header Files\\include"
      FILES ${_s})
  endif()
  get_target_property(_s ${PROJECT_NAME} HEADER_SET_internalHeader)
  if(_s)
    source_group(
      TREE "${PROJECT_SOURCE_DIR}/internal_include"
      PREFIX "\\Header Files\\internal_include"
      FILES ${_s})
  endif()
  get_target_property(_s ${PROJECT_NAME} SOURCES)
  if(_s)
    source_group(
      TREE "${PROJECT_SOURCE_DIR}/src"
      PREFIX "\\Source Files"
      FILES ${_s})
  endif()
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/zstdstream/version.hpp.in
               ${CMAKE_CURRENT_BINARY_DIR}/version.hpp @ONLY)
target_sources(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/version.hpp)

install(TARGETS ${PROJECT_NAME} FILE_SET HEADERS DESTINATION include)
