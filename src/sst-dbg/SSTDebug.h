//
// _SSTDEBUG_H_
//
// Copyright (C) 2017-2022 Tactical Computing Laboratories, LLC
// All Rights Reserved
// contact@tactcomplabs.com
//
// See LICENSE in the top level directory for licensing details
//

// -- CXX Headers
#include <fstream>
#include <iostream>
#include <typeinfo>
#include <string>
#include <vector>
#include <cstdarg>
#include <typelist.hpp>

// -- Required Macros
#define SSTCYCLE  uint64_t

using namespace tl;

class SSTDebug {
private:

  // Private variables
  std::string Name;       ///< Name of the component
  std::string Path;       ///< Output path

  /// SSTDebug: SST internal type derivation

public:
  /// SSTDebug: SST Debug constructor
  SSTDebug();

  /// SSTDebug: SST Debug overloaded constructor
  SSTDebug(std::string Name);

  /// SSTDebug: SST Debug overloaded constructor
  SSTDebug(std::string Name, std::string Path);

  /// SSTDebug: SST Debug destructor
  ~SSTDebug();

  /// SSTDebug: SST Debug data dump
  void dump(SSTCYCLE cycle, tl::type_list<> list){
  }
};

// EOF
