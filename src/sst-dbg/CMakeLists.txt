# src/sst-dbg CMakeLists.txt driver
#
# Copyright (C) 2017-2023 Tactical Computing Laboratories, LLC
# All Rights Resereved
# contact@tactcomplabs.com
#
# This file is a part of the SST-DBG package.  For license
# information, see the LICENSE file in the top level directory of
# this distribution.
#

set(SSTDebugSrcs
SSTDebug.h
)

include_directories( . )
include_directories(${SST_INCLUDE_DIR})

install(DIRECTORY ./ DESTINATION ${SST_DBG_HEADER_INSTALL_PATH} FILES_MATCHING PATTERN "*.h")

# EOF
