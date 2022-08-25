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
#include <stdarg.h>
#include <typeinfo>

// -- Required Macros
#define SSTCYCLE  uint64_t

#define VARNAME(v) #v
#define DARG(v) VARNAME(v), v

class SSTDebug {
private:

  // Private variables
  std::string Name;       ///< Name of the component
  std::string Path;       ///< Output path

  std::ofstream Bin;      ///< Binary output

  template<typename T>
  void __internal_dump(){
  }

  template<typename T>
  void __internal_dump(T v){
    Bin << v << std::endl;
  }

  template<typename T, typename U, typename... Args>
  bool __internal_dump(T t, U u, Args... args){
    Bin << t << ":" << u << std::endl;
  }

  template<typename... Args>
  void __internal_dump(Args... args){
  }

  template<typename T1, typename T2>
  void __internal_dump(T1 v1, T2 v2){
    Bin << v1 << ":" << v2 << std::endl;
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

  /// SSTDebug: SST Debug data dump function
  template<typename T, typename U, typename... Args>
  bool dump(SSTCYCLE cycle, T t, U u, Args... args){

    std::string BinName = Name + "." + std::to_string(cycle) + ".out";
    Bin.open(BinName.c_str(), std::ios::out);

    Bin << t << ":" << u << std::endl;
    __internal_dump(args...);

    Bin.close();
  }
};

// EOF
