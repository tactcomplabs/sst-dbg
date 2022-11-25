//
// iface_ascii_test3.cpp
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

std::string TEST_NAME = "iface_ascii_test3";

int main( int argc, char **argv ){
  std::cout << "TEST = " << __FILE__ << std::endl;

  SSTDebug Dbg(TEST_NAME,"./");

  if( !Dbg.IsJSON() ){
    std::cout << "PASS" << std::endl;
    return 0;
  }else{
    std::cout << "FAIL" << std::endl;
    return -1;
  }
}

// EOF
