//
// _DebugConsole_cpp_
//
// Copyright (C) 2017-2023 Tactical Computing Laboratories, LLC
// All Rights Reserved
// contact@tactcomplabs.com
//
// See LICENSE in the top level directory for licensing details
//

#include "./DebugConsole.h"
#include "Utils.h"
#include "nlohmann/json.hpp"
#include "nlohmann/json_fwd.hpp"
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <future>
#include <iostream>
#include <mutex>
#include <signal.h>
#include <string>
#include <sys/wait.h>
#include <thread>
// #include <future>
#include <iostream>
using nlohmann::json;

namespace chrono = std::chrono;

// ============================
// DebugConsole::Init
// ============================
void DebugConsole::Init() {

  TitleComponent = Renderer([&] {
    return hbox({text("v0.0.1"), separatorLight(),
                 text("SST Debug UI") | center | flex, separatorLight(),
                 spinner(18, animationFrame)});
  });

  InputFieldComponent = Input(&userInput, "[enter your command here]");

  MsgFrameComponent = Renderer([&] {
    Elements DebugMsgElements{};
    for (const auto &message : MsgQueue) {
      DebugMsgElements.emplace_back(DebugMsgToFtxElement(message));
    }
    return vbox(std::move(DebugMsgElements));
  });

  MsgScrollerComponent = Scroller(MsgFrameComponent); //| Maybe(&TableView);

  MainContainerComponent =
      Container::Vertical({MsgScrollerComponent, InputFieldComponent});
  MainContainerComponent |=
      CatchEvent([&](Event e) { return ContainerEventHandler(e); });

  MainComponent = Renderer(MainContainerComponent, [&] {
    return vbox({TitleComponent->Render(), separatorLight(),
                 // A dbox allows us to draw components *over* each other
                 // What is being rendered here is a vertical stack, and then
                 // we render a horizontal stack over it, which is used to
                 // position the autocomplete window
                 dbox({vbox({MsgScrollerComponent->Render(), filler()}),
                       hbox({
                           filler(),
                           vbox({
                               // Move the autocomplete window to the bottom
                               filler() | yflex_shrink,
                               // Draw the autocomplete window
                               // autocompleteElement |
                               //   size(WIDTH, LESS_THAN, 40) |
                               //   size(HEIGHT, LESS_THAN, 60)
                           }),
                           // Move the autocomplete window a few characters to
                           // the left
                           // filler() | size(Direction::WIDTH,
                           //                 Constraint::EQUAL, 8)
                       })}) |
                     flex,
                 separatorLight(),
                 // Draw the input bar
                 hbox({text("> "), InputFieldComponent->Render() | focus |
                                       size(HEIGHT, EQUAL, 1)})}) |
           borderDouble;
  });

  std::this_thread::sleep_for(chrono::milliseconds(int(100.0f)));

  ListenerThread = std::thread([&] {
    // Don't immediately render text, wait for a little bit
    std::this_thread::sleep_for(chrono::milliseconds(1000));

    // OnLog( { "$y[SSTDebug] $gInitialised SSTDebugUI" } );
    OnLog({"$y[SSTDebug] $gType 'q|quit|exit' to quit this console"});

    screen.Loop(MainComponent);
  });

  // debugThread =
}

// std::filesystem::path DebugConsole::FindSST() {
//   std::filesystem::path SST_ROOT;
//   std::filesystem::path SST_PATH;
//   const char *SST_ENV = std::getenv("SST_ROOT");
//   if (SST_ENV == NULL) {
//     std::cerr << "SST_ROOT Variable Not Found";
//   } else {
//     std::filesystem::path PotentialPath =
//         std::filesystem::path(std::string(SST_ENV) + "/bin/sst");
//     SST_PATH = std::filesystem::exists(PotentialPath) ? PotentialPath
//                                                       : FindBinInPath("sst");
//   }

//   return SST_PATH;
// }

bool DebugConsole::DumpFileContents(const std::string &Filename) {
  std::ifstream input_file(Filename, std::ifstream::in);

  if (Filename.find(".json") != std::string::npos) {
    // JSON Output
    json j;
    input_file >> j;

    for (auto it = j.begin(); it != j.end(); ++it) {
      OnLog({'\t' + it.key() + ":\t" + it.value().dump()});
    }
  }

  else {
    std::string line;
    while (getline(input_file, line)) {
      OnLog({'\t' + line});
    }
  }
  return 0;
}

// void DebugConsole::ReadFile(const std::string &Filename) {
//   std::ifstream file(Filename, std::ios::in);

//   while (true) {
//     file.seekg(0, file.end);
//     int size = file.tellg();

//     if (size > 0) {
//       file.seekg(0, file.beg);
//       std::string buffer(size, ' ');
//       OnLog({buffer});
//     }
//     std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//   }
// }

//   OnLog({"USER INPUT = " + UserInput});

//   std::system(Conf.SSTPath.c_str());

//     OnLog({"Error: Encountered an error with SST : "});
//     OnLog({strerror(errno)});
//     return 0;
//   }
//   else{
//     // execvp successful launch
//     if( )
//     return 1;
//
//   }
//   return true;
// }
//

// Conf.pid = fork();
// const char *MyCommand = Conf.SSTPath.c_str();

// std::thread std::launch::async, [this](); {
//                            execvp(Conf.SSTPath.c_str(),
//                             &Conf.ArgV[Conf.ArgC]);
//                              return false;});
//                            // execvp(MyCommand, Conf.ArgV[Conf.ArgC]);

// int child_result = handle.get();
// if( child_result == -1 ){
//   std::cout << "Error : Encountered an error with SST : " <<
//   strerror(errno)
//   << std::endl; OnLog({"<<SSTDBG>> Error : Encountered an error with SST :
//   "}); return false;
// } else if( child_result == 0 ){
//     OnLog({"<<CHILD>> Result was 0"});
// }else{
//   // main event loop
//   OnLog({"$y[SSTParent] Ayo it's your dad"});
//   return ParentLoop();
// }

//   return true;
// }

// ============================
// DebugConsole::Shutdown
// ============================
void DebugConsole::Shutdown() {
  stopListening = true;
  screen.Post([&] { screen.ExitLoopClosure()(); });
  ListenerThread.join();
}

// ============================
// DebugConsole:OnLog
// ============================
void DebugConsole::OnLog(const DebugMsg &message) {
  std::unique_lock<std::mutex> lock(m_msg_mutex);
  MsgQueue.push_back(message);
  TimeToUpdate = -1.0f; // update ui
  lock.unlock();
}

// ============================
// DebugConsole::OnUpdate
// ============================
bool DebugConsole::OnUpdate(const float &deltaTime) {
  if (ExecuteCommand) {
    ConsumeCommand();
    ExecuteCommand = false;
    JumpToBottom = true;
  }

  if (stopListening) {
    return false;
  }

  TimeToUpdate -= deltaTime;
  if (TimeToUpdate <= 0.0f) {
    screen.PostEvent(Event::Custom);
    TimeToUpdate = 0.2f;
  }

  return true;
}

// ============================
// DebugConsole::ContainerEventHandler
// --> Called on a separate thread
// ============================
bool DebugConsole::ContainerEventHandler(Event e) {
  if (stopListening) {
    return true;
  }

  if (e == Event::ArrowUp // TODO: Make cycle through CommandHistory
      || e == Event::PageUp || e == Event::ArrowDown || e == Event::PageDown ||
      e == Event::Home || e == Event::End) {
    MsgScrollerComponent->OnEvent(e);
    return true;
  }

  // ===========
  // KEY: <CR>
  // ===========
  if (e == Event::Return) {
    if (!userInput.empty()) {
      ExecuteCommand = true;
      JumpToBottom = true;
    }
    return true;
  }

  // =============
  // KEY: Movement
  // =============
  else if (e.is_character() || e == Event::Backspace || e == Event::Tab ||
           e == Event::ArrowLeft || e == Event::ArrowRight) {
    InputFieldComponent->OnEvent(e);
    return true;
  }

  // It's time to update
  if (e == Event::Custom) {
    animationFrame++;

    if (JumpToBottom) {
      screen.PostEvent(Event::End);
      JumpToBottom = false;
    }
    return true;
  }

  return false;
}

// ============================
// DebugConsole::DebugMsgToFtxElement
// ============================
Element DebugConsole::DebugMsgToFtxElement(const DebugMsg &message) {
  static std::unordered_map<char, Color> ColorMap{
      {'r', Color::Red},        {'o', Color::Orange1},   {'y', Color::Yellow},
      {'g', Color::GreenLight}, {'b', Color::BlueLight}, {'p', Color::Pink1},
      {'w', Color::White},      {'G', Color::GrayLight}};

  char currentColor = 'w';
  Elements coloredTexts{};
  ElementDecorator textColor = color(ColorMap[currentColor]);
  std::string string;
  for (size_t i = 0; i < message.text.size(); i++) {
    if (message.text[i] == '$') {
      if (!string.empty()) {
        coloredTexts.emplace_back(text(string) | textColor);
        string.clear();
      }

      i++;
      if (i >= message.text.size()) {
        break;
      }

      currentColor = message.text[i];

      auto iterator = ColorMap.find(currentColor);
      if (iterator == ColorMap.end()) {
        textColor = color(Color::White);
      } else {
        textColor = color(iterator->second);
      }

      i++;
      if (i >= message.text.size()) {
        break;
      }
    }

    string += message.text[i];

    if (i == message.text.size() - 1) {
      coloredTexts.emplace_back(text(string) | textColor);
    }
  }

  return hbox({text(GenerateTimeString(message)), separator(), text(" "),
               hbox(std::move(coloredTexts))});
}

// ============================
// DebugConsole::GenerateTimeString
// ============================
const char *DebugConsole::GenerateTimeString(const DebugMsg &message) {
  // mmm:ss.ssss
  static char buffer[16];

  const int iTime = message.timeSubmitted;
  const int seconds = int(message.timeSubmitted) % 60;
  const int minutes = iTime / 60;

  const float flSeconds = seconds + (message.timeSubmitted - iTime);

  sprintf(buffer, "%03i:%06.3f ", minutes, flSeconds);
  return buffer;
}

// ============================
// DebugConsole::IsInputValid
// ============================
bool DebugConsole::IsInputValid() const {
  if (userInput.empty()) {
    return false;
  }

  if (userInput[0] == ' ' || userInput[0] == '\t' || userInput[0] == '\n') {
    return false;
  }

  return true;
}

// ============================
// DebugConsole::GetCommandName
// ============================
std::string DebugConsole::GetCommandName() const {
  if (!IsInputValid()) {
    return "";
  }

  // Simple, one-word command
  const size_t firstSpace = userInput.find_first_of(" ");
  if (firstSpace == std::string::npos) {
    return userInput;
  }

  return userInput.substr(0, firstSpace);
}

void DebugConsole::PrintHelp() {
  OnLog({"Usage: $g[sst-dbg] [OPTIONS] -- /path/to/sst arg1 arg2 arg3 ..."});
}

void DebugConsole::ExecuteCmd(std::string debug_shell_cmd) {
  // std::string cmd_to_run = "sst-dbg --iter 1 -- sst ./2NodeTest_long.py > "
  //                          "previous_sst_output.txt 2>&1";

  std::string new_cmd =
      "sst-dbg " + debug_shell_cmd + " > prev_sst_output.txt 2>&1";

  OnLog({new_cmd});
  int result = std::system(new_cmd.c_str());

  sst_is_executing = false;

  DebugCmd.clear();
}

void DebugConsole::WatcherThread() {
  std::unique_lock<std::mutex> lock(m_sstMutex);
  cv.wait(lock, [this] {
    return sst_proc.wait_for(std::chrono::seconds(1)) ==
           std::future_status::ready;
  });
}

// ============================
// DebugConsole::ConsumeCommand
// ============================
void DebugConsole::ConsumeCommand() {
  if (userInput == "q" || userInput == "quit" || userInput == "exit") {
    stopListening = true;
    return;
  } else if (userInput == "help") {
    PrintHelp();
  } else if (userInput.find("!") == 0) {
    std::unique_lock<std::mutex> lock(m_msg_mutex);
    std::string shellCmd = userInput.erase(userInput.find('!'), 1);
    std::system(shellCmd.c_str());
    lock.unlock();
    std::this_thread::sleep_for(chrono::seconds(2));
  } else if (userInput.find("ls") != std::string::npos) {
    // std::unique_lock<std::mutex> lock(m_msg_mutex);
    for (const auto filename : std::filesystem::directory_iterator(".")) {
      OnLog({filename.path()});
    }
    // lock.unlock();
  } else if (userInput.find("run") != std::string::npos) {
    std::string runStr = "run";
    std::string::size_type pos = userInput.find("run");
    DebugCmd = userInput.erase(pos, runStr.length());
    // OnLog({"$b--------- Starting SST Process -----------"});
    std::unique_lock<std::mutex> lock(m_sstMutex);

    if (sst_is_executing) {
      OnLog({"SST is already running ya dingus."});
      lock.unlock();
    } else {
      // no existing sst proc running
      sst_is_executing = true;
      sst_proc = std::async(std::launch::async, &DebugConsole::ExecuteCmd, this,
                            DebugCmd);
      lock.unlock();

      std::thread([this] { WatcherThread(); }).detach();
    }
  } else {
    OnLog({"$r[SSTDebug] Invalid/Unknown Command. Type `help` for possible "
           "commands "});
  }
  CommandHistory.emplace_back(userInput);
  userInput.clear();
}
