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

include (FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

set(CONSOLE_SRCS
  ./DebugConsole.h
  ./DebugConsole.cpp
  ./Scroller.h
  ./DebugMsg.h
  ./Main.cpp
)

set(SSTDBG_SRCS
sst-dbg.cpp
)

include_directories( . )
include_directories(${SSTDBG_INCLUDE_PATH})
include_directories(${SST_INCLUDE_DIR})
# include_directories(${CSVPARSER_DIR/include})

add_executable(sst-dbg-console ${CONSOLE_SRCS})
target_link_libraries(sst-dbg-console 
  # PRIVATE ncurses
  # PRIVATE csv
  
  PRIVATE nlohmann_json::nlohmann_json
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component
  )
# target_link_libraries(sst-dbg-console csvparser)
install(TARGETS sst-dbg-console DESTINATION ${SST_BIN_DIR})

add_executable(sst-dbg ${SSTDBG_SRCS})
install(TARGETS sst-dbg DESTINATION ${SST_BIN_DIR})
# EOF
