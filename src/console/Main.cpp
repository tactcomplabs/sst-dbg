//
// _Main_cpp_
//
// Copyright (C) 2017-2023 Tactical Computing Laboratories, LLC
// All Rights Reserved
// contact@tactcomplabs.com
//
// See LICENSE in the top level directory for licensing details
//

// #include "SSTDebug.h"
#include "SSTDebugDir.h"
#include "View/DebugConsole.h"
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <signal.h>

namespace chrono = std::chrono;
namespace this_thread = std::this_thread;
using json = nlohmann::json;

constexpr float UpdateInterval = 1.0f / 20.0f;
constexpr float PingInterval = 1.0f;

void Wait(float seconds) {
  this_thread::sleep_for(chrono::milliseconds(int(seconds * 1000.0f)));
}

int main(int argc, char *argv[]) {
  // ===============
  // SST Specific
  // ===============
  // -- Setup default SIGUSR2 behavior to ignore the signal
  //    This is necessary in case we attempt to trigger a dump before SST has
  //    initialized its own internal signal handlers
  signal(SIGUSR2, SIG_IGN);

  DebugConsole view{};

  // ==========
  // Debug File
  // ==========
  std::vector<std::string> DumpFiles = {};
  std::vector<std::vector<std::string>> DebugData;
  std::string DebugDir = argv[1]; // FIXME: Add error handling
  // std::filesystem::path DebugDirPath = DebugDir.value_or("./");

  std::unique_ptr<DumpDir> DumpDirectory(new DumpDir(argv[1], ".json"));

  view.Init();

  bool sst_running = false;
  float directory_scan_interval = 2.0f;

  view.OnLog({"$y[SSTDebug] Searching for existing sst-process..."});
  view.OnLog({"$y[SSTDebug] None found... Type `(run|r) -- <sst-args> to start "
              "a debug process"});

  const std::vector<std::string> &DumpsToProcess =
      DumpDirectory->get_unprocessed_files();
  // view.OnLog({"Files found:"});
  // for (auto dumpfile : DumpsToProcess)
  //   view.OnLog({dumpfile});

  // Capture time program is launched to distinguish NEW dump files vs
  // pre-existing. Not sure if this is the best way but it seems to work
  std::filesystem::file_time_type ProgramLaunchTime =
      std::filesystem::file_time_type::clock::now();

  // view.SetSSTPath(view.FindSST());

  while (view.OnUpdate(UpdateInterval)) {
    Wait(UpdateInterval);

    if (DumpsToProcess.size() > 0) {

      // process files
      for (unsigned i = 0; i < DumpsToProcess.size(); i++) {
        if (std::filesystem::last_write_time(DumpsToProcess.at(i)) >
            ProgramLaunchTime) {
          view.OnLog({"$b[SSTDebug] New Dump Found: " + DumpsToProcess.at(i)});
          view.DumpFileContents(DumpsToProcess.at(i));
          std::string FileToDump =
              DumpDirectory->ProcessFile(DumpsToProcess.at(i));
        }
      }
      directory_scan_interval -= UpdateInterval;
    }
  }

  view.Shutdown();
  return 0;
}
