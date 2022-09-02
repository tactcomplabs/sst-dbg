# sst-dbg Top-Level CMake Description
#
# Copyright (C) 2017-2022 Tactical Computing Laboratories, LLC
# All Rights Resereved
# contact@tactcomplabs.com
#
# This file is a part of the SST-DBG package.  For license
# information, see the LICENSE file in the top level directory of
# this distribution.
#

cmake_minimum_required (VERSION 2.6)
project(sst-dbg CXX)


#--- SETUP BUILD ENV
#find_program(SST_CONFIG sst-config)
#if(NOT SST_CONFIG)
#  message(FATAL_ERROR "No sst-config found in the current path")
#endif()

#message(STATUS "Using sst-config at ${SST_CONFIG}")

#execute_process (
#  COMMAND bash -c "${SST_CONFIG} --CXXFLAGS"
#  OUTPUT_VARIABLE SST_CXX_FLAGS
#  OUTPUT_STRIP_TRAILING_WHITESPACE
#)

#execute_process (
#  COMMAND bash -c "${SST_CONFIG} --includedir"
#  OUTPUT_VARIABLE SST_INCLUDE_DIR
#  OUTPUT_STRIP_TRAILING_WHITESPACE
#)

#execute_process (
#  COMMAND bash -c "${SST_CONFIG} --ELEMENT_LDFLAGS"
#  OUTPUT_VARIABLE SST_ELEMENT_LDFLAGS
#  OUTPUT_STRIP_TRAILING_WHITESPACE
#)

set(SST_CXX_FLAGS "-std=c++1y -D__STDC_FORMAT_MACROS")
set(SST_INCLUDE_DIR "")

message(STATUS "SST CXX flags set to: ${SST_CXX_FLAGS}")
message(STATUS "SST header path set to: ${SST_INCLUDE_DIR}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SST_CXX_FLAGS}")
set(SSTDBG_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src/sst-dbg")

enable_testing()

add_subdirectory(src)
add_subdirectory(test)

# EOF
