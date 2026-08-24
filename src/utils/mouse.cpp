#include "mouse.hpp"
#include <Windows.h>

void utils::MoveMouseRelative(int x, int y)
{
  INPUT input = {0};
  input.type = INPUT_MOUSE;
  input.mi.dx = x;
  input.mi.dy = y;
  input.mi.dwFlags = MOUSEEVENTF_MOVE;
  SendInput(1, &input, sizeof(INPUT));
}
