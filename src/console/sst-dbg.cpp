//
// _sst-dbg_cpp_
//
// Copyright (C) 2017-2023 Tactical Computing Laboratories, LLC
// All Rights Reserved
// contact@tactcomplabs.com
//
// See LICENSE in the top level directory for licensing details
//

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <cstring>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>

typedef struct{
  bool isHelp;
  bool isOneShot;
  bool isIter;
  bool isRand;
  unsigned sec;
  unsigned ArgC;
  pid_t pid;
  std::string SSTArgs;
  std::string SSTPath;
}CLICONF;

CLICONF Conf;

void PrintHelp(){
  std::cout << " Usage: sst-dbg [OPTIONS] -- /path/to/sst arg1 arg2 arg3 ..." << std::endl;
  std::cout << " Options:" << std::endl;
  std::cout << "\t-h|-help|--help             : Print help menu" << std::endl;
  std::cout << "\t-i|-iter|--iter sec         : Dump the state every `sec` seconds" << std::endl;
  std::cout << "\t-o|-oneshot|--oneshot sec   : Perform a single dump at time=`sec`" << std::endl;
  std::cout << "\t-r|-rand|--rand             : Dump the state on a random cadence between 0-60 seconds" << std::endl;
}

void SplitStr(const std::string& s, char delim,
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

bool SetupSSTArgs(CLICONF &Conf){
  std::vector<std::string> v;
  SplitStr(Conf.SSTArgs,' ',v);

  if( v.size() <= 1 ){
    std::cout << "Error : not enough arguments" << std::endl;
    PrintHelp();
    return false;
  }

  if( v[0] == "sst" ){
    // the first arg is a relative path, derive the actual path
    char name[] = "/tmp/sstpathXXXXXX";
    int fd = mkstemp(name);
    std::string tmpsys = "which sst >> " + std::string(name);
    system(tmpsys.c_str());
    std::ifstream Input;
    Input.open(name);
    std::getline(Input,Conf.SSTPath);
    Input.close();
  }else{
    // the first arg should be the full path
    // ensure that 'sst' appears in the string
    std::vector<std::string> v2;
    SplitStr(v[0],'/',v2);
    if( (v2[v2.size()-1] != "sst") &&
        (v2[v2.size()-1] != "mpirun") ){
      std::cout << "Error : could not find SST or MPIRUN executables in " << v[0] << std::endl;
      return false;
    }
    Conf.SSTPath = v[0];
  }

  // rewrite the SSTArgs to exclude the binary
  std::string TmpStr;
  for( unsigned i=1; i<v.size(); i++ ){
    TmpStr += v[i];
    TmpStr += " ";
  }

  Conf.SSTArgs = TmpStr;

  return true;
}

bool ParseArgs(int argc, char **argv, CLICONF &Conf){
  // set the default program options
  Conf.isHelp     = false;
  Conf.isOneShot  = false;
  Conf.isIter     = false;
  Conf.isRand     = false;
  Conf.sec        = 0;
  Conf.ArgC       = 2;

  for( int i=1; i<argc; i++ ){
    std::string s(argv[i]);

    if( (s=="-h") || (s=="-help") || (s=="--help") ){
      // help selected
      Conf.isHelp = true;
      return true;
    }else if( s=="--" ){
      std::string TmpSST;
      if( i+1 > (argc-1) ){
        std::cout << "Error : no SST arguments found" << std::endl;
        std::cout << "argc = " << argc << std::endl;
        return false;
      }
      Conf.ArgC = i+1;  // save the start of the SST args
      for( unsigned j=(i+1); j<argc; j++ ){
        std::string Tmp(argv[j]);
        TmpSST += Tmp;
        TmpSST += " ";
      }
      i = argc; // end the parser
      Conf.SSTArgs = TmpSST;
    }else if( (s=="-i") || (s=="-iter") || (s=="--iter") ){
      if( i+1 > (argc-1) ){
        std::cout << "Error : --iter requires an argument" << std::endl;
        return false;
      }
      std::string P(argv[i+1]);
      Conf.isIter    = true;
      Conf.isOneShot = false;
      Conf.isRand    = false;
      Conf.sec = stoi(P);
      i++;
    }else if( (s=="-o") || (s=="-oneshot") || (s=="--oneshot") ){
      if( i+1 > (argc-1) ){
        std::cout << "Error : --iter requires an argument" << std::endl;
        return false;
      }
      std::string P(argv[i+1]);
      Conf.isIter    = false;
      Conf.isOneShot = true;
      Conf.isRand    = false;
      Conf.sec = stoi(P);
      i++;
    }else if( (s=="-r") || (s=="-rand") || (s=="--rand") ){
      Conf.isIter    = false;
      Conf.isOneShot = false;
      Conf.isRand    = true;
      srand(time(NULL));
      Conf.sec = (unsigned)(rand()%60);
      if( Conf.sec == 0 )
        Conf.sec++;
    }else{
      // parsing error
      std::cout << "Error : unknown option : " << s << std::endl;
      std::cout << "Use \"--help\" for the help menu" << std::endl;
      return false;
    }
  }

  if( !SetupSSTArgs(Conf) ){
    return false;
  }

  return true;
}

bool SanityCheck(CLICONF Conf){
  if( Conf.isIter || Conf.isOneShot ){
    if( Conf.sec == 0 ){
      std::cout << "Error : Iterative or one shot configurations must specify timings of > 0 seconds" << std::endl;
      return false;
    }
  }

  if( (!Conf.isIter) &&
      (!Conf.isOneShot) &&
      (!Conf.isRand) ){
    std::cout << "Error : No runtime arguments found" << std::endl;
    std::cout << "      : use --iter, --oneshot or --rand" << std::endl;
    std::cout << "      : See --help" << std::endl;
    return false;
  }

  if( Conf.SSTArgs.size() == 0 ){
    std::cout << "Error : No SST arguments present" << std::endl;
    return false;
  }

  return true;
}

bool IsAlive(pid_t Child, bool &Exited){
  int status = 0;
  pid_t result = waitpid(Child, &status, WNOHANG);
  if( result == 0 ){
    // still alive
    return true;
  }else if( result == -1 ){
    // error occurred
    Exited = false;
    return false;
  }else{
    // child exited successfully
    Exited = true;
    return false;
  }
}

bool ChildWait(pid_t Child){
  int status = 0;
  pid_t result = waitpid(Child,&status,0);
  if( result == -1 ){
    return false;
  }
  return true;
}

bool TriggerDump(CLICONF &Conf){
  std::cout << "<<SSTDBG>>               ------------------ DUMP ------------------              <<SSTDBG>>" << std::endl;
  kill(Conf.pid, SIGUSR2);
  return true;
}

bool ParentLoop(CLICONF &Conf){
  // first check to see that our child is still alive
  bool Exited = false;
  if( !IsAlive(Conf.pid, Exited) ){
    if( Exited ){
      // successful child exit
      return true;
    }else{
      std::cout << "<<SSTDBG>> Error : SST process exited with an error" << std::endl;
      return false;
    }
  }

  unsigned dumps = 0;

  // Child process is still alive
  do{
    if( Exited ){
      // child process has exited successfully
      std::cout << "<<SSTDBG>> Execution complete" << std::endl;
      return true;
    }else if( Conf.isOneShot && (dumps == 1) ){
      // wait for the child pid to complete
      std::cout << "WAITING FOR CHILD TO END" << std :: endl;
      if( !ChildWait(Conf.pid) ){
        std::cout << "<<SSTDBG>> Error : SST child process exited with an error" << std::endl;
        return false;
      }
    }

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point end;
    do{
      std::this_thread::yield();
      end = std::chrono::steady_clock::now();
    }while(std::chrono::duration_cast<std::chrono::seconds>(end-begin).count() < Conf.sec);

    // trigger the dump
    if( IsAlive(Conf.pid,Exited) ){
      if( !TriggerDump(Conf) ){
        // encountered an error
        // kill the child process and close everything
        kill(Conf.pid,SIGQUIT);
        return false;
      }
    }

    dumps++;

  }while(IsAlive(Conf.pid,Exited));

  return Exited;
}

bool ExecuteSST(CLICONF &Conf, char **argv){

  Conf.pid = fork();

  if( Conf.pid == -1 ){
    // error occurred
    std::cout << "Error : Cannot fork child SST process" << std::endl;
    return false;
  }else if( Conf.pid == 0 ){
    // child process
    std::cout << "<<SSTDBG>> Executing SST" << std::endl;
    std::cout << "<<SSTDBG>> " << Conf.SSTPath << " " << Conf.SSTArgs << std::endl;

    if( execvp(Conf.SSTPath.c_str(),
               &argv[Conf.ArgC]) == -1 ){
      std::cout << "Error : Encountered an error with SST : " << strerror(errno) << std::endl;
      return false;
    }
  }else{
    // main event loop
    return ParentLoop(Conf);
  }

  return true;
}

void ctrlc_handler(int sig){
  kill(Conf.pid, SIGINT);
}

int main(int argc, char **argv){

  if( !ParseArgs(argc, argv, Conf) ){
    return -1;
  }

  if( Conf.isHelp ){
    PrintHelp();
    return 0;
  }

  // Setup the default SIGUSR2 behavior to ignore the signal
  // This allows our child process to continue executing in the event
  // that we attempt to trigger a dump command before SST has initialized
  // it's internal signal handlers
  signal(SIGUSR2,SIG_IGN);

  // Setup a signal handler to catch ctrl-c
  // This allows us to catch the control-c (SIGINT) signal and send it to
  // the SST process
  signal(SIGINT, ctrlc_handler);

  // Sanity check the args
  if( !SanityCheck(Conf) ){
    return -1;
  }

  // Execute
  if( !ExecuteSST(Conf,argv) ){
    return -1;
  }

  return 0;
}

// EOF
