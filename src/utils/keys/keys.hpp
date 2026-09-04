#pragma once

#include <Windows.h>
#include <atomic>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <thread>
#include <unordered_set>

namespace keys
{
  inline std::atomic<bool> debugLogging{false};

  namespace detail
  {
    inline std::mutex stateMutex;
    inline std::unordered_set<WORD> systemHeld;
    inline std::unordered_set<WORD> humanHeld;

    inline HHOOK hHook = nullptr;
    inline std::thread hookThread;
    inline std::atomic<bool> running{false};
    inline DWORD hookThreadId = 0;

    inline void logEvent(WORD vKey, bool isDown, bool injected)
    {
      if (!debugLogging)
        return;

      auto now = std::chrono::system_clock::now();
      auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                    now.time_since_epoch()) %
                1000;
      auto t = std::chrono::system_clock::to_time_t(now);

      std::tm tmBuf;
      localtime_s(&tmBuf, &t);

      std::cout << std::put_time(&tmBuf, "%H:%M:%S") << '.'
                << std::setfill('0') << std::setw(3) << ms.count() << " | ";

      std::cout << "vk=0x" << std::hex << vKey << std::dec;

      std::cout << " | " << (isDown ? "DOWN" : "UP  ")
                << " | " << (injected ? "SYSTEM" : "HUMAN")
                << '\n';
    }

    inline LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
    {
      if (nCode == HC_ACTION)
      {
        auto *kb = reinterpret_cast<KBDLLHOOKSTRUCT *>(lParam);
        bool injected = (kb->flags & LLKHF_INJECTED) != 0;
        bool isDown = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
        bool isUp = (wParam == WM_KEYUP || wParam == WM_SYSKEYUP);

        if (isDown || isUp)
          logEvent((WORD)kb->vkCode, isDown, injected);

        if (!injected)
        {
          std::lock_guard<std::mutex> lock(stateMutex);
          if (isDown)
            humanHeld.insert((WORD)kb->vkCode);
          else if (isUp)
            humanHeld.erase((WORD)kb->vkCode);
        }
      }
      return CallNextHookEx(hHook, nCode, wParam, lParam);
    }

    inline void hookThreadProc()
    {
      hookThreadId = GetCurrentThreadId();

      hHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc,
                               GetModuleHandle(nullptr), 0);

      if (!hHook)
      {
        std::cerr << "ERROR: SetWindowsHookEx failed, code=" << GetLastError() << '\n';
        return;
      }

      SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);

      MSG msg;
      while (running && GetMessage(&msg, nullptr, 0, 0))
      {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }

      if (hHook)
      {
        UnhookWindowsHookEx(hHook);
        hHook = nullptr;
      }
    }
  }

  inline void startHookThread()
  {
    if (detail::running)
      return;
    detail::running = true;
    detail::hookThread = std::thread(detail::hookThreadProc);
  }

  inline void stopHookThread()
  {
    if (!detail::running)
      return;
    detail::running = false;

    if (detail::hookThreadId != 0)
      PostThreadMessage(detail::hookThreadId, WM_QUIT, 0, 0);

    if (detail::hookThread.joinable())
      detail::hookThread.join();
  }

  struct HookGuard
  {
    HookGuard() { startHookThread(); }
    ~HookGuard() { stopHookThread(); }
    HookGuard(const HookGuard &) = delete;
    HookGuard &operator=(const HookGuard &) = delete;
  };

  inline void sendKeyDown(WORD vKey)
  {
    INPUT input = {0};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = vKey;
    input.ki.dwFlags = 0;
    SendInput(1, &input, sizeof(INPUT));

    std::lock_guard<std::mutex> lock(detail::stateMutex);
    detail::systemHeld.insert(vKey);
  }

  inline void sendKeyUp(WORD vKey)
  {
    INPUT input = {0};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = vKey;
    input.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &input, sizeof(INPUT));

    std::lock_guard<std::mutex> lock(detail::stateMutex);
    detail::systemHeld.erase(vKey);
  }

  inline bool isSystemHoldingKey(WORD vKey)
  {
    std::lock_guard<std::mutex> lock(detail::stateMutex);
    return detail::systemHeld.count(vKey) != 0;
  }

  inline bool isHumanHoldingKey(WORD vKey)
  {
    std::lock_guard<std::mutex> lock(detail::stateMutex);
    return detail::humanHeld.count(vKey) != 0;
  }
}