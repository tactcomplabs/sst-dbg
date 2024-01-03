# sst-dbg Top-Level CMake Description
#
# Copyright (C) 2017-2023 Tactical Computing Laboratories, LLC
# All Rights Resereved
# contact@tactcomplabs.com
#
# This file is a part of the SST-DBG package.  For license
# information, see the LICENSE file in the top level directory of
# this distribution.
#

cmake_minimum_required (VERSION 3.14)
project(sst-dbg CXX)

set(CMAKE_CXX_STANDARD 17)

#--- SETUP BUILD ENV
find_program(SST_CONFIG sst-config)
if(NOT SST_CONFIG)
  message(FATAL_ERROR "No sst-config found in the current path")
endif()

#-- SETUP MPI ENV
if(SSTDBG_MPI)
  find_package(MPI)
  if(MPI_FOUND)
    add_definitions(-DSSTDBG_MPI=1)
    message(STATUS "Found MPI CXX at ${MPI_CXX_COMPILER}")
  endif()
endif()

include(FetchContent)

# --- Fetch FTXUI
FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  GIT_TAG v5.0.0
  # GIT_TAG v3.0.0
)

FetchContent_GetProperties(ftxui)
if(NOT ftxui_POPULATED)
  FetchContent_Populate(ftxui)
  add_subdirectory(${ftxui_SOURCE_DIR} ${ftxui_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

# END FTXUI

message(STATUS "Using sst-config at ${SST_CONFIG}")

execute_process (
  COMMAND bash -c "${SST_CONFIG} --includedir"
  OUTPUT_VARIABLE SST_INCLUDE_DIR
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
  COMMAND bash -c "${SST_CONFIG} --bindir"
  OUTPUT_VARIABLE SST_BIN_DIR
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(SSTDBG_ASCII)
  add_definitions(-DSSTDBG_ASCII=1)
endif()

set(SST_DBG_HEADER_INSTALL_PATH "${SST_INCLUDE_DIR}/sst/dbg")

set(SST_CXX_FLAGS "-std=c++17 -D__STDC_FORMAT_MACROS")
set(SST_INCLUDE_DIR "")

message(STATUS "SST CXX flags set to: ${SST_CXX_FLAGS}")
message(STATUS "SST header path set to: ${SST_INCLUDE_DIR}")
message(STATUS "SST bin path set to ${SST_BIN_DIR}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SST_CXX_FLAGS}")
set(SSTDBG_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src/sst-dbg")

enable_testing()

add_subdirectory(src)
add_subdirectory(test)

# EOF
