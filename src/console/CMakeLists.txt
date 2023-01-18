# src/console CMakeLists.txt driver
#
# Copyright (C) 2017-2023 Tactical Computing Laboratories, LLC
# All Rights Resereved
# contact@tactcomplabs.com
#
# This file is a part of the SST-DBG package.  For license
# information, see the LICENSE file in the top level directory of
# this distribution.
#

set(CONSOLE_SRCS
sst-console.cpp
)

set(SSTDBG_SRCS
sst-dbg.cpp
)

include_directories( . )
include_directories(${SSTDBG_INCLUDE_PATH})
include_directories(${SST_INCLUDE_DIR})

add_executable(sst-dbg-console ${CONSOLE_SRCS})
target_link_libraries(sst-dbg-console ncurses)
install(TARGETS sst-dbg-console DESTINATION ${SST_BIN_DIR})

add_executable(sst-dbg ${SSTDBG_SRCS})
install(TARGETS sst-dbg DESTINATION ${SST_BIN_DIR})
# EOF
