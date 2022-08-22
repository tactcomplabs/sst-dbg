# src/console CMakeLists.txt driver
#
# Copyright (C) 2017-2022 Tactical Computing Laboratories, LLC
# All Rights Resereved
# contact@tactcomplabs.com
#
# This file is a part of the SST-DBG package.  For license
# information, see the LICENSE file in the top level directory of
# this distribution.
#

set(CONSOLE_SRCS
main.cpp
)

include_directories( . )
include_directories(${SSTDBG_INCLUDE_PATH})
include_directories(${SST_INCLUDE_DIR})
add_executable(sst-dbg ${CONSOLE_SRCS})
target_link_libraries(sst-dbg ncurses SSTDebug)
install(TARGETS sst-dbg DESTINATION bin)

# EOF
