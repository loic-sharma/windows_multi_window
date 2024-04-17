#include "flutter_window.h"

#include <optional>

#include "flutter/generated_plugin_registrant.h"
#include "flutter_windows_internal.h"

FlutterWindow::FlutterWindow(FlutterDesktopEngineRef engine)
  : engine_(engine) {}

FlutterWindow::~FlutterWindow() {}

bool FlutterWindow::OnCreate() {
  if (!Win32Window::OnCreate()) {
    return false;
  }

  RECT frame = GetClientArea();

  // The size here must match the window dimensions to avoid unnecessary surface
  // creation / destruction in the startup path.
  FlutterDesktopViewControllerProperties properties = {};
  properties.width = frame.right - frame.left;
  properties.height = frame.bottom - frame.top;

  controller_ = FlutterDesktopEngineCreateViewController(engine_, &properties);
  if (controller_ == nullptr) {
    return false;
  }

  FlutterDesktopViewRef view{ FlutterDesktopViewControllerGetView(controller_) };
  SetChildContent(FlutterDesktopViewGetHWND(view));
  return true;
}

void FlutterWindow::OnDestroy() {
  if (controller_) {
    FlutterDesktopViewControllerDestroy(controller_);
  }

  Win32Window::OnDestroy();
}

LRESULT
FlutterWindow::MessageHandler(HWND hwnd, UINT const message,
  WPARAM const wparam,
  LPARAM const lparam) noexcept {
  // Give Flutter, including plugins, an opportunity to handle window messages.
  if (controller_) {
    LRESULT result;
    bool handled = FlutterDesktopViewControllerHandleTopLevelWindowProc(
      controller_,
      hwnd,
      message,
      wparam,
      lparam,
      &result);

    if (handled) {
      return result;
    }
  }

  switch (message) {
  case WM_FONTCHANGE:
    FlutterDesktopEngineReloadSystemFonts(engine_);
    break;
  }

  return Win32Window::MessageHandler(hwnd, message, wparam, lparam);
}
