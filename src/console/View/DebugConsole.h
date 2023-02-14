#include <condition_variable>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <filesystem>
#include <future>
#include <mutex>

using namespace ftxui;

#include "./Scroller.h"
#include "./DebugMsg.h"

// ============
// DebugConsole
// ============
class DebugConsole final {
public:
  void Init();
  void Shutdown();

  void OnLog( const DebugMsg& message );
  bool OnUpdate( const float& deltaTime );
  
  bool WriteHistory( const std::string& Filename );
  bool DumpFileContents(const std::string &Filename);

private:
  // std::future<int> sst_future;
  bool sst_is_executing { false };
  std::mutex m_sstMutex;
  std::mutex m_msg_mutex;
  std::condition_variable cv;
  std::future<void> sst_proc;

  std::string DebugCmd {""};
  
  // Handle CLI Events (input & scrolling)
  std::vector<std::string> CommandHistory;
  int ExecuteSST(std::string SSTCmd);

  bool ContainerEventHandler( Event e );
  void ConsumeCommand();
  void PrintHelp();
  void UpdateAutocomplete();

  bool IsInputValid() const;
  std::string GetCommandName() const;

  static const char* GenerateTimeString( const DebugMsg& message );
  static Element DebugMsgToFtxElement( const DebugMsg& message );

  void ExecuteCmd(std::string debug_shell_cmd);

  void WatcherThread();

  private:
    bool Exited;
    std::filesystem::path DumpDirectory;
    bool stopListening{ false };
    std::vector<DebugMsg> MsgQueue{};

    std::thread ListenerThread;
    float TimeToUpdate{ 0.1f };
    bool JumpToBottom{ false };
    bool ExecuteCommand{ false };

    std::string userInput{ "" };

    // Screen Object
    ScreenInteractive screen = ScreenInteractive::Fullscreen();
    // Animation Frame in top right
    int animationFrame{ 0 };
    // Title Bar
    Component TitleComponent{};
    Component InputFieldComponent{};
    Component MsgFrameComponent{};
    Component MsgScrollerComponent{};

    Component MainContainerComponent{}; // Includes MsgFrameComponent & InputFieldComponent
    Component MainComponent{};

}; // EOF


