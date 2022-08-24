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

#define _(...) __VA_ARGS__

#define S_CHECK_EQ(type1, type2)                                               \
    static_assert(std::is_same<type1, type2>::value, #type1 " == " #type2)

std::string TEST_NAME = "iface_test1";

int main( int argc, char **argv ){
  std::cout << "TEST = " << TEST_NAME << std::endl;

  SSTDebug Dbg;

  std::string FOO = TEST_NAME;
  unsigned BAR    = 42;
  uint64_t CYCLE  = 100;

  using T0 = type_list<>;
  using T0_1 = T0::append<type_list<std::string>>;
  using T0_2 = T0_1::append<type_list<unsigned>>;

  S_CHECK_EQ(T0::prepend<bool>, _(type_list<bool>));

  T0 t0;
  T0_2 t1;

  Dbg.dump(CYCLE, t0, FOO, BAR);    // this compiles

  // This does not compile
  // How do we make the TypeList variadic to record the type info?
  //
  // Issue: Variadic templates are handled recursively.  We can derive
  //        the type info from the variadic template arguments _OR_
  //        do something with the data members, but not both.
  //
  // Goal:
  //       I want to do the following in the `dump` method:
  //       1) Record the type info and data sizes from each of the data arguments
  //       2) Write a header out to a binary file describing the data format
  //       3) Write all the data to a binary file in the prescribed format
  //Dbg.dump(CYCLE, t1, FOO, BAR);

  std::cout << "PASS" << std::endl;

  return 0;
}

// EOF
