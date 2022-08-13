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

  WINDOW *msg_win;
  WINDOW *console_win;
}DBGCFG;

#define CONSOLE_WIN_YDIM  5

void Console(DBGCFG &Conf){

  int ch;
  bool done = false;

  mvwprintw(Conf.console_win, 0,1, "COMMAND CONSOLE");
  mvwprintw(Conf.console_win, 1,1, "$> ");
  mvwprintw(Conf.msg_win, 0,1, "SST-DBG");
  mvwprintw(Conf.msg_win, 1,1, "SST_MSG");
  wrefresh(Conf.console_win);
  wrefresh(Conf.msg_win);

  // main console loop
  while( (ch = getch()) && !done ){
    switch( ch ){
      case 10:
        done = true;
        break;
      default:
        move(LINES-(CONSOLE_WIN_YDIM-1), 4);
        wrefresh(Conf.console_win);
        refresh();
        break;
    }
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
