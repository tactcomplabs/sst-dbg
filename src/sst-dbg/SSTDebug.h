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

// -- SST Headers
#include <sst/core/sst_config.h>
#include <sst/core/component.h>

class SSTDebug {
private:

  // Private variables
  std::string Name;       ///< Name of the component
  std::string Path;       ///< Output path

  /// SSTDebug: SST internal type derivation
  template <typename T>
  std::type_info getType(T t){
    return typeid(t).name();
  }

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
  template<SST::Cycle_t, typename T, typename... Args>
  bool dump(SST::Cycle_t currentCycle, T t, Args... args){
    getType(args...);
    return true;
  }

};

// EOF
