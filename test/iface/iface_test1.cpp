//
// iface_test1.cpp
//
// Copyright (C) 2017-2022 Tactical Computing Laboratories, LLC
// All Rights Reserved
// contact@tactcomplabs.com
//
// See LICENSE in the top level directory for licensing details
//

#include <iostream>
#include <string>
#include "SSTDebug.h"

std::string TEST_NAME = "iface_test1";

int main( int argc, char **argv ){
  std::cout << "TEST = " << TEST_NAME << std::endl;

  SSTDebug Dbg(TEST_NAME,"./");

  std::string FOO = TEST_NAME;
  unsigned BAR    = 42;
  uint64_t CYCLE  = 100;

  Dbg.dump(CYCLE, "FOO", FOO);
  Dbg.dump(CYCLE+1, "FOO", FOO, "BAR", BAR);
  Dbg.dump(CYCLE+2, DARG(FOO));
  Dbg.dump(CYCLE+3, DARG(FOO), DARG(BAR));

  std::cout << "PASS" << std::endl;

  return 0;
}

// EOF
