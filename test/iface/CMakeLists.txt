# test/iface CMakeLists.txt driver
#
# Copyright (C) 2017-2022 Tactical Computing Laboratories, LLC
# All Rights Resereved
# contact@tactcomplabs.com
#
# This file is a part of the SST-DBG package.  For license
# information, see the LICENSE file in the top level directory of
# this distribution.
#

include_directories(${SSTDBG_INCLUDE_PATH})
include_directories(${SST_INCLUDE_DIR})

#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${SST_ELEMENT_LDFLAGS}")
#set(CMAKE_POSITION_INDEPENDENT_CODE ON)

file(GLOB TEST_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

if(BUILD_SSTDBG_IFACE_TESTING)
  foreach(testSrc ${TEST_SRCS})
    # Extract the file name
    get_filename_component(testName ${testSrc} NAME_WE)

    # Add compile target
    add_executable(${testName} ${testSrc})

    # Add linker deps
    target_link_libraries(${testName} SSTDebug)

    # Drop the exe's in a separate directory
    set_target_properties(${testName} PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/testBin)

    # Add the tests for execution
    add_test(NAME ${testName}
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/testBin
      COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/testBin/${testName} )
    set_tests_properties( ${testName} PROPERTIES PASS_REGULAR_EXPRESSION "PASS")

  endforeach(testSrc)
endif()

# EOF
