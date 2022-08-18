# src/sst-dbg CMakeLists.txt driver
#
# Copyright (C) 2017-2022 Tactical Computing Laboratories, LLC
# All Rights Resereved
# contact@tactcomplabs.com
#
# This file is a part of the SST-DBG package.  For license
# information, see the LICENSE file in the top level directory of
# this distribution.
#

set(SSTDebugSrcs
SSTDebug.h
SSTDebug.cpp
)

include_directories( . )
include_directories(${SST_INCLUDE_DIR})

add_library(SSTDebug STATIC ${SSTDebugSrcs})

install(TARGETS SSTDebug DESTINATION lib)
install(DIRECTORY ./ DESTINATION include FILES_MATCHING PATTERN "*.h")

# EOF
