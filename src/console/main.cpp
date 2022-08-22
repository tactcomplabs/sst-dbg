//
// _main_cpp_
//
// Copyright (C) 2017-2022 Tactical Computing Laboratories, LLC
// All Rights Reserved
// contact@tactcomplabs.com
//
// See LICENSE in the top level directory for licensing details
//

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <ncurses.h>

#include "SSTDebug.h"

// Data structures
typedef struct{

  // -- basic configuration
  std::string sst;      // SST command to execute
  bool isHelp;          // Is the help menu selected?

  // -- window configuration
  int rows;             // total rows
  int cols;             // total columns
  int console_startx;   // console window start-x
  int console_starty;   // console window start-y
  int console_rows;     // console window rows
  int console_cols;     // console window columns
  int msg_startx;       // msg window start-x
  int msg_starty;       // msg window start-y
  int msg_rows;         // msg window rows
  int msg_cols;         // msg window columns
  WINDOW *msg_win;      // message window (captured from SST output)
  WINDOW *console_win;  // user console window

  // -- process configuration
  pid_t pid;            // child pid
  int filedes[2];       // child file descriptors

}DBGCFG;

typedef struct{
  std::string cmd;        // command string
  bool (*func)(DBGCFG *); // function callback
  bool end;               // end of the command list
}CMDHANDLER;

// Prototypes for commands
bool CmdExit(DBGCFG* Conf);     // Exit the application
bool CmdRun(DBGCFG* Conf);      // Run the SST sim
bool CmdDump(DBGCFG* Conf);     // Dump the SST data
bool CmdKill(DBGCFG* Conf);     // Kill the running SST sim
bool CmdPause(DBGCFG* Conf);    // Pause the SST sim
bool CmdHelp(DBGCFG* Conf);     // Print the help menu
bool CmdClear(DBGCFG* Conf);    // Clear the message screen

// Command list
CMDHANDLER Cmds[] = {
  {"run",   CmdRun,   false},
  {"dump",  CmdDump,  false},
  {"kill",  CmdKill,  false},
  {"pause", CmdPause, false},
  {"exit",  CmdExit,  false},
  {"clear", CmdClear, false},
  {"help",  CmdHelp,  false},
  {"null",  nullptr,  true}
};

#define CONSOLE_WIN_YDIM  5
#define MAX_STR           1024

bool CmdHelp(DBGCFG* Conf){
  mvwprintw(Conf->console_win, 2,3,
            "Commands: run dump kill pause clear exit help");
  wrefresh(Conf->console_win);
  return false;
}

bool CmdExit(DBGCFG* Conf){
  if( Conf->pid > 0 ){
    // kill the child process
  }
  return true;
}

bool CmdClear(DBGCFG* Conf){
  return false;
}

bool CmdPause(DBGCFG* Conf){
  return false;
}

bool CmdKill(DBGCFG* Conf){
  return false;
}

bool CmdRun(DBGCFG* Conf){
  if( Conf->pid > 0 ){
    mvwprintw(Conf->console_win, 2,1, "ERROR: SST is already running");
    wrefresh(Conf->console_win);
    return false;
  }

  // create the named pipe
  if( pipe(Conf->filedes) == -1 ){
    mvwprintw(Conf->console_win, 2,1, "ERROR: Cannot create pipe for SST child process");
    wrefresh(Conf->console_win);
    return false;
  }

  // fork the child process
  Conf->pid = fork();
  if( Conf->pid == -1 ){
    // error occurred
    mvwprintw(Conf->console_win, 2,1, "ERROR: Cannot fork child SST process; check the path");
    wrefresh(Conf->console_win);
    close(Conf->filedes[1]);
    close(Conf->filedes[0]);
    return false;
  }else if( Conf->pid == 0 ){
    // child process
    while ((dup2(Conf->filedes[1], STDOUT_FILENO) == -1) && (errno == EINTR)) {}
    close(Conf->filedes[1]);
    close(Conf->filedes[0]);
    if( execl("/", Conf->sst.c_str(), (char*)0) == -1 ){
      mvwprintw(Conf->console_win, 2,1, "ERROR: Encountered error in executing SST; check the path");
      wrefresh(Conf->console_win);
      wrefresh(Conf->console_win);
      return false;
    }
  }
  close(Conf->filedes[1]);

  return false;
}

bool CmdDump(DBGCFG* Conf){
  return false;
}

bool ProcessCommand(DBGCFG &Conf, char *str){
  std::string TmpCmd(str);
  unsigned i = 0;
  bool (*func)(DBGCFG *);

  // main command loop
  //
  // if a function returns "true" then either an error
  // has occurred or the user has asked to exit
  //
  // if we exit the do-while loop, then the command was not found
  do{
    if( TmpCmd == Cmds[i].cmd ){
      // execute it
      func = Cmds[i].func;
      return (*func)(&Conf);
    }
    i++;
  }while( !Cmds[i].end );
  mvwprintw(Conf.console_win, 2,1, "ERROR: UNKNOWN COMMAND:");
  mvwprintw(Conf.console_win, 3,1, str);
  wrefresh(Conf.console_win);

  return false;
}

void Console(DBGCFG &Conf){

  int ch;
  bool done = false;
  char str[MAX_STR];

  mvwprintw(Conf.console_win, 0,1, "COMMAND CONSOLE");
  mvwprintw(Conf.console_win, 1,1, "$> ");
  mvwprintw(Conf.msg_win, 0,1, "SST-DBG");
  wrefresh(Conf.console_win);
  wrefresh(Conf.msg_win);

  // main console loop
  while(!done){
    getstr(str);
    move(LINES-(CONSOLE_WIN_YDIM-1), 4);
    wrefresh(Conf.console_win);
    wclear(Conf.console_win);
    box(Conf.console_win, 0, 0);
    mvwprintw(Conf.console_win, 0,1, "COMMAND CONSOLE");
    mvwprintw(Conf.console_win, 1,1, "$> ");
    wrefresh(Conf.console_win);
    done = ProcessCommand(Conf, str);
  }

  delwin(Conf.msg_win);
  delwin(Conf.console_win);

  endwin();
}

void InitConfig(DBGCFG &Conf){

  // initialization ncurses
  initscr();
  nocbreak();

  // enable F1, F2, etc...
  keypad(stdscr, TRUE);

  // init the internal data structure
  Conf.rows = LINES;
  Conf.cols = COLS;

  Conf.msg_rows       = LINES-CONSOLE_WIN_YDIM-1;
  Conf.msg_cols       = COLS-1;
  Conf.msg_starty     = 1;
  Conf.msg_startx     = 1;

  Conf.console_rows   = CONSOLE_WIN_YDIM;
  Conf.console_cols   = COLS-1;
  Conf.console_startx = LINES-CONSOLE_WIN_YDIM;
  Conf.console_starty = 1;

  Conf.pid            = -1;
  Conf.filedes[0]     = -1;
  Conf.filedes[1]     = -1;

  // create the windows
  Conf.msg_win      = newwin(Conf.msg_rows, Conf.msg_cols,
                             Conf.msg_starty, Conf.msg_startx);
  Conf.console_win  = newwin(Conf.console_rows, Conf.console_cols,
                             Conf.console_startx, Conf.console_starty);

  // set the message window to scroll
  scrollok(Conf.msg_win, TRUE);

  refresh();
  box(Conf.console_win, 0, 0);
  box(Conf.msg_win, 0, 0);

  // move the cursor
  move(LINES-(CONSOLE_WIN_YDIM-1), 4);
  wrefresh(Conf.console_win);
  wrefresh(Conf.msg_win);
}

bool ParseArgs(int argc, char **argv, DBGCFG &Conf){
  // set the default program opts
  Conf.isHelp = false;

  for( int i=1; i<argc; i++ ){
    std::string s(argv[i]);

    if( (s=="-h") || (s=="-help") || (s=="--help") ){
      // help selected
      Conf.isHelp = true;
      return true;
    }else if( s=="--" ){
      // start parsing the SST args
      std::string TmpSST;
      if( i+1 > (argc-1) ){
        std::cout << "Error : no SST arguments found" << std::endl;
        return false;
      }
      for( unsigned j=(i+1); j<argc; j++ ){
        std::string Tmp(argv[j]);
        TmpSST += Tmp;
        TmpSST += " ";
      }
      Conf.sst = TmpSST;
    }else{
      // parsing error
      std::cout << "Error : unknown option : " << s << std::endl;
      std::cout << "Use \"--help\" for the help menu" << std::endl;
      return false;
    }
  }

  return true;
}

void PrintHelp(){
  std::cout << " Usage: sst-dbg -- /path/to/sst arg1 arg2 arg3 ..." << std::endl;
  std::cout << " Options:" << std::endl;
  std::cout << "\t-h|-help|--help                      : Print help menu" << std::endl;
}

int main(int argc, char **argv){
  DBGCFG Conf;

  if( !ParseArgs(argc, argv, Conf) ){
    return false;
  }

  if( Conf.isHelp ){
    PrintHelp();
    return 0;
  }

  InitConfig(Conf);
  Console(Conf);

  return 0;
}

// EOF
