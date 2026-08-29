#include "rcs.hpp"

#include <string>

#include "core/engine/cache/cache.hpp"
#include "utils/mouse/mouse.hpp"
#include "utils/logger/logger.hpp"

void RCS::update(float dt)
{
  auto snapshot = Cache::copySnapshot();

  if (!snapshot.localPlayer.isAlive)
  {
    oldAimPunch = {0.0, 0.0};
    return;
  }

  if (snapshot.localPlayer.shotsFired <= 1)
  {
    oldAimPunch = {0.0f, 0.0f};
    return;
  }

  Vector3 aimPunch = snapshot.localPlayer.aimPunch;
  Vector2 currentAimPunch = {aimPunch.x, aimPunch.y};

  Vector2 deltaPunch = (currentAimPunch - oldAimPunch) * -1.0 * 3.37;

  Vector2 moveAmount = {
      (deltaPunch.y / snapshot.globals.sensitivity) / -0.022f,
      (deltaPunch.x / snapshot.globals.sensitivity) / 0.022f};

  int moveXAmountInPixel = static_cast<int>(moveAmount.x);
  int moveYAmountInPixel = static_cast<int>(moveAmount.y);

  logger::debug(
      "aimPunch = { x: " +
      std::to_string(aimPunch.x) +
      ", y: " +
      std::to_string(aimPunch.y) +
      ", z: " +
      std::to_string(aimPunch.z) +
      " }");

  logger::debug(
      "currentAimPunch = { x: " +
      std::to_string(currentAimPunch.x) +
      ", y: " +
      std::to_string(currentAimPunch.y) +
      " }");

  logger::debug(
      "oldAimPunch = { x: " +
      std::to_string(oldAimPunch.x) +
      ", y: " +
      std::to_string(oldAimPunch.y) +
      " }");

  logger::debug(
      "deltaPunch = { x: " +
      std::to_string(deltaPunch.x) +
      ", y: " +
      std::to_string(deltaPunch.y) +
      " }");

  logger::debug(
      "sensitivity = " +
      std::to_string(snapshot.globals.sensitivity));

  logger::debug(
      "moveAmount = { x: " +
      std::to_string(moveAmount.x) +
      ", y: " +
      std::to_string(moveAmount.y) +
      " }");

  logger::debug(
      "moveXAmountInPixel = " +
      std::to_string(moveXAmountInPixel) +
      ", moveYAmountInPixel = " +
      std::to_string(moveYAmountInPixel));
  logger::debug(
      "RCS | punch(" +
      std::to_string(aimPunch.x) + ", " +
      std::to_string(aimPunch.y) +
      ") old(" +
      std::to_string(oldAimPunch.x) + ", " +
      std::to_string(oldAimPunch.y) +
      ") delta(" +
      std::to_string(deltaPunch.x) + ", " +
      std::to_string(deltaPunch.y) +
      ") sens=" +
      std::to_string(snapshot.globals.sensitivity) +
      " move(" +
      std::to_string(moveAmount.x) + ", " +
      std::to_string(moveAmount.y) +
      ") pixels(" +
      std::to_string(moveXAmountInPixel) + ", " +
      std::to_string(moveYAmountInPixel) +
      ")");

  if (moveXAmountInPixel != 0 || moveYAmountInPixel != 0)
  {
    mouse::moveMouseRelative(moveXAmountInPixel, moveYAmountInPixel);
  }

  oldAimPunch = currentAimPunch;
};