#pragma once

#include <Windows.h>
#include <chrono>
#include <thread>

namespace keys
{
  inline void sendKeyDown(WORD vKey)
  {
    INPUT input = {0};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = vKey;
    input.ki.dwFlags = 0;
    SendInput(1, &input, sizeof(INPUT));
  }

  inline void sendKeyUp(WORD vKey)
  {
    INPUT input = {0};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = vKey;
    input.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &input, sizeof(INPUT));
  }

  inline void sendKeyPressTimed(WORD vKey, int durationMs)
  {
    sendKeyDown(vKey);
    std::this_thread::sleep_for(std::chrono::milliseconds(durationMs));
    sendKeyUp(vKey);
  }

  inline bool isKeyPressed(WORD vKey)
  {
    return (GetAsyncKeyState(vKey) & 0x8000) != 0;
  }
}