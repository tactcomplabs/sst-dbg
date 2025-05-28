//
// iface_json_test8.cpp
//
// Copyright (C) 2017-2023 Tactical Computing Laboratories, LLC
// All Rights Reserved
// contact@tactcomplabs.com
//
// See LICENSE in the top level directory for licensing details
//

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include "SSTDebug.h"

std::string TEST_NAME = "iface_json_test8";

int main( int argc, char **argv ){
  std::cout << "TEST = " << __FILE__ << std::endl;

  SSTDebug Dbg(TEST_NAME,"./");

  std::string FOO = TEST_NAME;
  unsigned BAR    = 42;
  uint64_t CYCLE  = 100;
  unsigned long long TEST = 42;
  double DPF = 3.14;

  std::vector<unsigned> V1 = {1, 2, 3, 4, 5};
  std::list L1{1, 2, 3};

  Dbg.dump(CYCLE, "FOO", FOO);
  Dbg.dump(CYCLE+1, "FOO", FOO, "BAR", BAR);
  Dbg.dump(CYCLE+2, DARG(FOO));
  Dbg.dump(CYCLE+3, DARG(FOO), DARG(BAR));
  Dbg.dump(CYCLE+4, DARG(FOO), DARG(BAR), DARG(TEST), DARG(DPF));
  //Dbg.dump(CYCLE+5, DARG(V1));
  Dbg.dump(CYCLE+6, DARG(FOO), DARG(V1));
  Dbg.dump(CYCLE+7, DARG(FOO), DARG(L1));
  Dbg.dump(CYCLE+8, DARG(FOO), DARG(V1), DARG(L1));

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
