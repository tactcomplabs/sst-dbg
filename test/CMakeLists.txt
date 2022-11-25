# test/ CMakeLists.txt driver
#
# Copyright (C) 2017-2022 Tactical Computing Laboratories, LLC
# All Rights Resereved
# contact@tactcomplabs.com
#
# This file is a part of the SST-DBG package.  For license
# information, see the LICENSE file in the top level directory of
# this distribution.
#


if(BUILD_ALL_TESTING)
  find_program (SST_EXECUTABLE sst)
  if (SST_EXECUTABLE)
    message(STATUS "FOUND SST AT ${SST_EXECUTABLE}")
  else ()
    message(FATAL_ERROR "COULD NOT FIND SST")
  endif ()

  set(BUILD_SSTDBG_IFACE_JSON_TESTING "ON")
  set(BUILD_SSTDBG_IFACE_ASCII_TESTING "ON")
endif()

add_subdirectory(iface_json)
add_subdirectory(iface_ascii)

# EOF
