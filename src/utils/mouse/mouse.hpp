#pragma once

#include <Windows.h>

namespace mouse
{
  inline void moveMouseRelative(int x, int y)
  {
    INPUT input = {0};
    input.type = INPUT_MOUSE;
    input.mi.dx = x;
    input.mi.dy = y;
    input.mi.dwFlags = MOUSEEVENTF_MOVE;
    SendInput(1, &input, sizeof(INPUT));
  }

  inline bool isButtonPressed(int button)
  {
    int vKey = 0;

    switch (button)
    {
    case 1:
      vKey = VK_LBUTTON;
      break;
    case 2:
      vKey = VK_RBUTTON;
      break;
    case 3:
      vKey = VK_MBUTTON;
      break;
    case 4:
      vKey = VK_XBUTTON1;
      break;
    case 5:
      vKey = VK_XBUTTON2;
      break;
    default:
      return false;
    }

    return (GetAsyncKeyState(vKey) & 0x8000) != 0;
  }
}