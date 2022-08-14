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
#include <string>
#include <vector>
#include <ncurses.h>

// Data structures
typedef struct{
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
}DBGCFG;

typedef struct{
  std::string cmd;        // command string
  bool (*func)(DBGCFG *); // function callback
  bool end;               // end of the command list
}CMDHANDLER;

// Prototypes for commands
bool CmdExit(DBGCFG* Conf);
bool CmdRun(DBGCFG* Conf);
bool CmdDump(DBGCFG* Conf);
bool CmdKill(DBGCFG* Conf);
bool CmdPause(DBGCFG* Conf);
bool CmdHelp(DBGCFG* Conf);
bool CmdClear(DBGCFG* Conf);

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
  return false;
}

bool CmdDump(DBGCFG* Conf){
  return false;
}

bool ProcessCommand(DBGCFG &Conf, char *str){
  std::string TmpCmd(str);
  unsigned i = 0;
  bool (*func)(DBGCFG *);

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

int main(int argc, char **argv){
  DBGCFG Conf;
  InitConfig(Conf);
  Console(Conf);
  return 0;
}

// EOF
