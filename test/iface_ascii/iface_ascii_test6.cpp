//
// iface_ascii_test6.cpp
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

std::string TEST_NAME = "iface_ascii_test6";

int main( int argc, char **argv ){
  std::cout << "TEST = " << __FILE__ << std::endl;

  SSTDebug Dbg(TEST_NAME,"./");

  std::string FOO = TEST_NAME;
  unsigned BAR    = 42;
  uint64_t CYCLE  = 100;

  Dbg.dump(CYCLE, "FOO", FOO);
  Dbg.dump(CYCLE+1, "FOO", FOO, "BAR", BAR);
  Dbg.dump(CYCLE+2, DARG(FOO));
  Dbg.dump(CYCLE+3, DARG(FOO), DARG(BAR));

  std::vector<std::string> Comps = Dbg.GetComponents();

  for( auto i : Comps ){
    std::cout << i << std::endl;
    std::vector<SSTCYCLE> Clocks = Dbg.GetClockValsByComponent(i);
    for( auto j : Clocks){
      std::cout << "\tClock = " << j << std::endl;
      SSTVALUE Vals = Dbg.GetDebugValues(i,j);
      for( auto k : Vals ){
        std::cout << "\t\tItem: " << k.first << " = " << k.second << std::endl;
      }
    }
  }

  std::cout << "PASS" << std::endl;

  return 0;
}

// EOF
