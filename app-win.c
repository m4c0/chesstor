#include "vlk.h"

#include <knownfolders.h>
#include <shlobj.h>

#pragma comment(lib, "ole32")
#pragma comment(lib, "user32")

#define SCR_W 800
#define SCR_H 600

static LRESULT window_proc(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param) {
  switch (msg) {
    case WM_DESTROY:
      PostQuitMessage(0);
      return 0;

    case WM_MOUSEMOVE:
      //vlk_mouse_move(GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param));
      return 0;
    case WM_LBUTTONDOWN:
      //vlk_mouse_down(GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param));
      return 0;

    case WM_PAINT:
      return 0;
  }
  return DefWindowProc(hwnd, msg, w_param, l_param);
}

int WinMain(HINSTANCE h_instance, HINSTANCE h_prev, LPSTR cmd_line, int cmd_show) {
  HICON h_icon = LoadIcon(h_instance, "IDI_APPICON");

  WNDCLASSEX wcex  = {
    .cbSize        = sizeof(WNDCLASSEX),
    .style         = CS_HREDRAW | CS_VREDRAW,
    .lpfnWndProc   = &window_proc,
    .hInstance     = h_instance,
    .hIcon         = h_icon,
    .hCursor       = LoadCursor(NULL, IDC_ARROW),
    .hbrBackground = (HBRUSH)(COLOR_WINDOW + 1),
    .lpszClassName = "m4c0-window",
    .hIconSm       = h_icon,
  };
  if (!RegisterClassEx(&wcex)) {
    MessageBox(NULL, "Failed to register window class", "Unhandled error", 0);
    return 1;
  }

  DWORD style = WS_OVERLAPPEDWINDOW ^ WS_SIZEBOX ^ WS_MAXIMIZEBOX;

  char title[256];
  LoadString(h_instance, 101, title, sizeof(title));

  HWND hwnd = CreateWindow(
      "m4c0-window", title,
      style, CW_USEDEFAULT, CW_USEDEFAULT,
      SCR_W, SCR_H, 
      NULL, NULL, h_instance, NULL);
  if (!hwnd) {
    MessageBox(NULL, "Failed to create window", "Unhandled error", 0);
    return 1;
  }

  ShowWindow(hwnd, cmd_show);
  UpdateWindow(hwnd);

  MSG msg;
  while (GetMessage(&msg, 0, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  return msg.wParam;
}
