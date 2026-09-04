#pragma once

#include <Windows.h>
#include <unordered_map>
#include <unordered_set>
#include <mutex>

namespace keys
{
  namespace detail
  {
    inline std::mutex stateMutex;
    inline std::unordered_set<WORD> systemHeld;
    inline std::unordered_set<WORD> humanHeld;
    inline HHOOK hHook = nullptr;

    inline LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
    {
      if (nCode == HC_ACTION)
      {
        auto *kb = reinterpret_cast<KBDLLHOOKSTRUCT *>(lParam);
        bool injected = (kb->flags & LLKHF_INJECTED) != 0;

        if (!injected)
        {
          std::lock_guard<std::mutex> lock(stateMutex);
          if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
            humanHeld.insert(kb->vkCode);
          else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP)
            humanHeld.erase(kb->vkCode);
        }
      }
      return CallNextHookEx(hHook, nCode, wParam, lParam);
    }
  }

  inline void installHook()
  {
    if (!detail::hHook)
      detail::hHook = SetWindowsHookEx(WH_KEYBOARD_LL, detail::LowLevelKeyboardProc,
                                       GetModuleHandle(nullptr), 0);
  }

  inline void uninstallHook()
  {
    if (detail::hHook)
    {
      UnhookWindowsHookEx(detail::hHook);
      detail::hHook = nullptr;
    }
  }

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