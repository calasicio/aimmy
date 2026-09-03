#include "renderer.hpp"
#include "window/window.hpp"

#include "gui/frontend/overlay/overlay.hpp"

#include "core/engine/engine.hpp"
#include "core/cheat/cheat.hpp"
#include "utils/logger/logger.hpp"

bool Renderer::init()
{
  return getInstance().initImpl();
}

void Renderer::thread()
{
  return getInstance().threadImpl();
}

void Renderer::destroy()
{
  return getInstance().destroyImpl();
}

bool Renderer::getIsOpen()
{
  return getInstance().isOpen;
}

bool Renderer::getIsFocused()
{
  return getInstance().isFocused;
}

bool Renderer::initImpl()
{
  if (!Window::SpawnWindow())
  {
    logger::fatal("Failed to create window");
    return false;
  }

  if (!Window::CreateDevice())
  {
    logger::fatal("Failed to create device");
    return false;
  }

  if (!Window::CreateImGui())
  {
    logger::fatal("Failed to create ImGui");
    return false;
  }

  Overlay::init();

  // Focus the game
  SetForegroundWindow(Engine::getProcess()->hwnd_);

  Window::SetAffinity(Window::hwnd, WindowAffinity::Invisible);

  Window::vsync = false;

  // We want the main thread to call render
  // And lock it
  // std::thread(Thread).detach();

  logger::info("Successfully initialized renderer...");
  return true;
}

void Renderer::destroyImpl()
{
  isRunning = false; // Prepare to stop thread loop
  logger::info("Successfully programed renderer destruction...");
}

void Renderer::threadImpl()
{
  while (isRunning)
  {
    const auto now = std::chrono::steady_clock::now();

    render();

    // If the game is not focused dont do states,
    // or will start focusing game & overlay
    if (this->isFocused && handleState())
      continue; // It will cause flickering if we handle window order after window closes

    handleWindowOrder();

    std::this_thread::sleep_until(now + std::chrono::milliseconds(4));
  }

  Cheat::destroy();

  // Once exited, destroy everything
  Window::DestroyImGui();
  Window::DestroyDevice();
  Window::DespawnWindow();
}

void Renderer::render()
{
  Window::StartRender();

  Overlay::render();

  Window::EndRender();
}

bool Renderer::handleState()
{
  isRunning = Window::shouldRun; // From the window event handler

  bool pressed_end = (GetAsyncKeyState(VK_END) & 0x8000);

  if (pressed_end)
    this->isRunning = false;

  return false;
}

bool Renderer::handleWindowOrder()
{
  auto p = Engine::getProcess();

  if (!p || (!p->hwnd_ && !p->UpdateHWND()))
    return false;

  // Check if game window is still valid, if not, most likely game closed
  if (!IsWindow(p->hwnd_))
    this->isRunning = false;

  static bool overlay_visible = true;
  auto foreground = GetForegroundWindow();
  this->isFocused = (foreground == Window::hwnd || foreground == p->hwnd_);

  if (!this->isFocused && overlay_visible)
  {
    logger::info("Hiding overlay window as the game its not focused");
    ShowWindow(Window::hwnd, SW_HIDE);
    overlay_visible = false;
    return true;
  }

  if (!overlay_visible && this->isFocused)
  {
    logger::info("Showing overlay window as the game is focused");
    ShowWindow(Window::hwnd, SW_SHOW);
    overlay_visible = true;
    return true;
  }

  static RECT last_rect = {0, 0, 0, 0};

  RECT window_rect;
  if (!GetWindowRect(p->hwnd_, &window_rect))
    return false;

  // All good, no movements from the client
  if (memcmp(&window_rect, &last_rect, sizeof(RECT)) == 0)
    return true;

  RECT client_rect;
  if (!GetClientRect(p->hwnd_, &client_rect))
    return false;

  POINT top_left = {client_rect.left, client_rect.top};
  POINT bottom_right = {client_rect.right, client_rect.bottom};

  ClientToScreen(p->hwnd_, &top_left);
  ClientToScreen(p->hwnd_, &bottom_right);

  RECT screen_rect = {top_left.x, top_left.y, bottom_right.x, bottom_right.y};

  SetWindowPos(
      Window::hwnd,
      HWND_TOPMOST,
      screen_rect.left,
      screen_rect.top,
      screen_rect.right - screen_rect.left,
      screen_rect.bottom - screen_rect.top,
      SWP_NOACTIVATE | SWP_SHOWWINDOW);

  last_rect = window_rect;

  return true;
}