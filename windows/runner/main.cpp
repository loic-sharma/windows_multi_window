#include <algorithm>
#include <flutter_windows.h>
#include <iterator>
#include <windows.h>

#include "flutter_window.h"
#include "utils.h"

int APIENTRY wWinMain(_In_ HINSTANCE instance, _In_opt_ HINSTANCE prev,
                      _In_ wchar_t *command_line, _In_ int show_command) {
  // Attach to console when present (e.g., 'flutter run') or create a
  // new console when running with a debugger.
  if (!::AttachConsole(ATTACH_PARENT_PROCESS) && ::IsDebuggerPresent()) {
    CreateAndAttachConsole();
  }

  // Initialize COM, so that it is available for use in the library and/or
  // plugins.
  ::CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

  std::vector<std::string> command_line_arguments =
    GetCommandLineArguments();
  std::vector<const char*> entrypoint_argv;
  std::transform(
    command_line_arguments.begin(), command_line_arguments.end(),
    std::back_inserter(entrypoint_argv),
    [](const std::string& arg) -> const char* { return arg.c_str(); });
  FlutterDesktopEngineProperties engine_properties{
    /*assets_path=*/L"data\\flutter_assets",
    /*icu_data_path=*/L"data\\icudtl.dat",
    /*aot_library_path=*/L"data\\app.so",
    /*dart_entrypoint=*/nullptr,
    /*dart_entrypoint_argc=*/static_cast<int>(entrypoint_argv.size()),
    /*dart_entrypoint_argv=*/entrypoint_argv.empty() ? nullptr : entrypoint_argv.data(),
  };

  FlutterDesktopEngineRef engine{FlutterDesktopEngineCreate(&engine_properties)};

  // RegisterPlugins(engine.get());

  Win32Window::Point origin1{10, 10};
  Win32Window::Point origin2{50, 50};
  Win32Window::Size size{1280, 720};

  FlutterWindow window1{engine};
  FlutterWindow window2{engine};
  if (!window1.Create(L"Window #1", origin1, size)
    || !window2.Create(L"Window #2", origin2, size)) {
    return EXIT_FAILURE;
  }

  ::MSG msg;
  while (::GetMessage(&msg, nullptr, 0, 0)) {
    ::TranslateMessage(&msg);
    ::DispatchMessage(&msg);
  }

  ::CoUninitialize();
  return EXIT_SUCCESS;
}
