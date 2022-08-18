//
// _SSTDEBUG_CPP_
//
// Copyright (C) 2017-2022 Tactical Computing Laboratories, LLC
// All Rights Reserved
// contact@tactcomplabs.com
//
// See LICENSE in the top level directory for licensing details
//

#include "SSTDebug.h"

SSTDebug::SSTDebug()
  : Name("."), Path("."){
}

SSTDebug::SSTDebug(std::string Name)
  : Name(Name), Path("./"){
}

SSTDebug::SSTDebug(std::string Name, std::string Path)
  : Name(Name), Path(Path){
}

SSTDebug::~SSTDebug(){
}

// EOF
