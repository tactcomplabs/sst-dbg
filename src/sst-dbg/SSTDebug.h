//
// _SSTDEBUG_H_
//
// Copyright (C) 2017-2023 Tactical Computing Laboratories, LLC
// All Rights Reserved
// contact@tactcomplabs.com
//
// See LICENSE in the top level directory for licensing details
//

#ifndef _SSTDEBUG_H_
#define _SSTDEBUG_H_

// -- CXX Headers
#include <fstream>
#include <iostream>
#include <typeinfo>
#include <string>
#include <vector>
#include <cstdarg>
#include <stdarg.h>
#include <typeinfo>
#include <dirent.h>
#include <algorithm>

// -- Required Macros
#define SSTCYCLE  uint64_t
#define SSTVALUE  std::vector<std::pair<std::string,std::string>>

#define VARNAME(v) #v
#define DARG(v) VARNAME(v), v

class SSTDebug {
private:

  // Private variables
  std::string Name;       ///< Name of the component
  std::string Path;       ///< Output path
  std::ofstream Bin;      ///< Output stream

  template<typename T, typename U, typename... Args>
  void __internal_dump(T t, U u, Args... args){
    __internal_dump(t,u);
    __internal_dump(args...);
  }

  template<typename... Args>
  void __internal_dump(Args... args){
  }

  template<typename T1, typename T2>
  void __internal_dump(T1 v1, T2 v2){
#ifdef SSTDBG_ASCII
    // use CSV
    Bin << v1 << "," << v2 << std::endl;
#else
    // use JSON
    Bin << "," << std::endl << "\"" << v1 << "\": \""
        << v2 << "\"";
#endif
  }

  void SplitStr(const std::string &s, char delim,
                std::vector<std::string>& v){
    auto i = 0;
    auto pos = s.find(delim);
    if( pos == std::string::npos ){
      v.push_back(s.substr(i,s.length()));
    }
    while( pos != std::string::npos ){
      v.push_back(s.substr(i,pos-i));
      i = ++pos;
      pos = s.find(delim,pos);
      if( pos == std::string::npos ){
        v.push_back(s.substr(i,s.length()));
      }
    }
  }

  /// SSTDebug: SST Debug retrieve the values from the ASCII target component at the target clock cycle
  SSTVALUE GetASCIIDebugValues(std::string Component,
                               SSTCYCLE Cycle){
    SSTVALUE v;
    std::ifstream Input;
    std::string BinName = Component + "." + std::to_string(Cycle) + ".out";
    Input.open(BinName.c_str());
    if( !Input.is_open() )
      return v;

    std::string line;
    std::vector<std::string> tmp;
    while( std::getline(Input,line) ){
      // split the line into two tokens: CSV format
      SplitStr(line,',',tmp);
      if( tmp.size() == 2 ){
        v.push_back(std::pair<std::string,std::string>(tmp[0],tmp[1]));
      }
      tmp.clear();
    }

    Input.close();
    return v;
  }

  /// SSTDebug: SST Debug retrieve the values from the JSON target component at the target clock cycle
  SSTVALUE GetJSONDebugValues(std::string Component,
                              SSTCYCLE Cycle){
    SSTVALUE v;
    std::ifstream Input;
    std::string BinName = Component + "." + std::to_string(Cycle) + ".json";
    Input.open(BinName.c_str());
    if( !Input.is_open() )
      return v;

    std::string line;
    std::vector<std::string> tmp;
    // retrieve the first line
    std::getline(Input,line);
    if( line != "}" ){
      Input.close();
      return v;
    }

    while( std::getline(Input,line) ){
      if( line == "}" ){
        Input.close();
        return v;
      }
      SplitStr(line,':',tmp);
      if( tmp.size() == 2 ){
        tmp[0].erase(std::remove(tmp[0].begin(),tmp[0].end(),'\"'),tmp[0].end());
        tmp[1].erase(std::remove(tmp[1].begin(),tmp[1].end(),'\"'),tmp[1].end());
        tmp[1].erase(std::remove(tmp[1].begin(),tmp[1].end(),','),tmp[1].end());
        v.push_back(std::pair<std::string,std::string>(tmp[0],tmp[1]));
      }
      tmp.clear();
    }

    Input.close();
    return v;
  }

public:

  /// SSTDebug: SST Debug constructor
  SSTDebug()
    : Name("SSTDBG"), Path("./"){
  }

  /// SSTDebug: SST Debug overloaded constructor
  SSTDebug(std::string Name)
    : Name(Name), Path("./"){
  }

  /// SSTDebug: SST Debug overloaded constructor
  SSTDebug(std::string Name, std::string Path)
    : Name(Name), Path(Path){
  }

  /// SSTDebug: SST Debug destructor
  ~SSTDebug(){}

  /// SSTDebug: SST Debug data dump function
  template<typename T, typename U, typename... Args>
  bool dump(SSTCYCLE cycle, T t, U u, Args... args){

#ifdef SSTDBG_ASCII
    std::string BinName = Name + "." + std::to_string(cycle) + ".out";
#else
    std::string BinName = Name + "." + std::to_string(cycle) + ".json";
#endif
    Bin.open(BinName.c_str(), std::ios::out);
    if( !Bin.is_open() )
      return false;

#ifdef SSTDBG_ASCII
    // CSV
    Bin << t << "," << u << std::endl;
#else
    // JSON
    Bin << "{" << std::endl;
    Bin << "\"Element\": \"" << Name << "\"," << std::endl;
    Bin << "\"Cycle\": " << cycle;
    Bin << "," << std::endl << "\"" << t << "\": \""
        << u << "\"";
#endif

    __internal_dump(args...);

#ifndef SSTDBG_ASCII
    Bin << std::endl << "}" << std::endl;
#endif

    Bin.close();

    return true;
  }


  /// SSTDebug: SST Debug set the path
  void setPath(std::string P){ Path = P; }

  /// SSTDebug: Determines if the driving application is using ASCII output
  bool IsASCII(){
#ifdef SSTDBG_ASCII
    return true;
#else
    return false;
#endif
  }

  /// SSTDebug: Determines if the driving application is using JSON output
  bool IsJSON(){
#ifdef SSTDBG_ASCII
    return false;
#else
    return true;
#endif
  }

  /// SSTDebug: SST Debug retrieve the set of current clock values
  std::vector<SSTCYCLE> GetClockVals(){
    DIR *dir;
    struct dirent *ent;
#ifdef SSTDBG_ASCII
    std::string delim = ".out";
#else
    std::string delim = ".json";
#endif
    std::vector<SSTCYCLE> v;

    if((dir = opendir(Path.c_str())) != NULL){
      while((ent = readdir(dir)) != NULL){
        std::string tmp(ent->d_name);
        if(tmp.find(delim.c_str()) != std::string::npos){
          // positive match, split the name
          std::vector<std::string> TmpV;
          SplitStr(tmp,'.',TmpV);

          // get the second to last vector element
          // this is where the clock cycle is stored
          if( TmpV.size() >= 3 ){
            v.push_back(std::stoi(TmpV[TmpV.size()-2]));
          }
        }
      }
      closedir(dir);
    }

    // make the vector unique
    v.erase( std::unique( v.begin(), v.end() ), v.end() );

    return v;
  }

  /// SSTDebug: SST Debug retrieve the set of current output clocks for the target component
  std::vector<SSTCYCLE> GetClockValsByComponent(std::string Component){
    DIR *dir;
    struct dirent *ent;
#ifdef SSTDBG_ASCII
    std::string delim = ".out";
#else
    std::string delim = ".json";
#endif
    std::vector<SSTCYCLE> v;

    if((dir = opendir(Path.c_str())) != NULL){
      while((ent = readdir(dir)) != NULL){
        std::string tmp(ent->d_name);
        if(tmp.find(delim.c_str()) != std::string::npos){
          // positive match, split the name
          std::vector<std::string> TmpV;
          SplitStr(tmp,'.',TmpV);

          // we have the vector of tokens,
          // concat everything until we find an integer
          std::string FinalStr;
          unsigned i = 0;
          unsigned pos = 0;
          while( (i<TmpV.size()) &&
                 (!TmpV[i].empty()) &&
                 (std::all_of(TmpV[i].begin(), TmpV[i].end(), ::isdigit))){
            pos = i;
          }
          FinalStr = TmpV[0];
          for( unsigned i=1; i<pos; i++ ){
            FinalStr += ".";
            FinalStr += TmpV[i];
          }

          if( FinalStr == Component ){
            // get the second to last vector element
            // this is where the clock cycle is stored
            if( TmpV.size() >= 3 ){
              v.push_back(std::stoi(TmpV[TmpV.size()-2]));
            }
          }
        }
      }
      closedir(dir);
    }

    // make the vector unique
    v.erase( std::unique( v.begin(), v.end() ), v.end() );

    return v;
  }

  /// SSTDebug: SST Debug retrieve the set of current component names
  std::vector<std::string> GetComponents(){
    DIR *dir;
    struct dirent *ent;
#ifdef SSTDBG_ASCII
    std::string delim = ".out";
#else
    std::string delim = ".json";
#endif
    std::vector<std::string> v;

    if((dir = opendir(Path.c_str())) != NULL){
      while((ent = readdir(dir)) != NULL){
        std::string tmp(ent->d_name);
        if(tmp.find(delim.c_str()) != std::string::npos){
          // positive match, split the name
          std::vector<std::string> TmpV;
          SplitStr(tmp,'.',TmpV);

          // we have the vector of tokens,
          // concat everything until we find an integer
          std::string FinalStr;
          unsigned i = 0;
          unsigned pos = 0;
          while( (i<TmpV.size()) &&
                 (!TmpV[i].empty()) &&
                 (std::all_of(TmpV[i].begin(), TmpV[i].end(), ::isdigit))){
            pos = i;
          }
          FinalStr = TmpV[0];
          for( unsigned i=1; i<pos; i++ ){
            FinalStr += ".";
            FinalStr += TmpV[i];
          }

          v.push_back(FinalStr);
        }
      }
      closedir(dir);
    }

    // make the vector unique
    v.erase( std::unique( v.begin(), v.end() ), v.end() );

    return v;
  }

  /// SSTDebug: SST Debug retrieve the values from the target component at the target clock cycle
  SSTVALUE GetDebugValues(std::string Component,
                          SSTCYCLE Cycle){
#ifdef SSTDBG_ASCII
    return GetASCIIDebugValues(Component,Cycle);
#else
    return GetJSONDebugValues(Component,Cycle);
#endif
  }
};

#endif  // #ifndef _SSTDEBUG_H_

// EOF
