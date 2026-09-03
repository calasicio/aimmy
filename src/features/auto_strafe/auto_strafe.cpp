#include "auto_strafe.hpp"

#include <Windows.h>

#include "core/engine/types/vector2.hpp"
#include "core/engine/types/vector3.hpp"
#include "utils/mouse/mouse.hpp"
#include "utils/keys/keys.hpp"
#include "utils/logger/logger.hpp"

void AutoStrafe::update(float dt)
{
  Cache::withLock([this, dt](const Cache &cache)
                  {
    if (!keys::isKeyPressed(VK_LMENU))
    {
      resetState();
      return;
    };

    bool userIsInterfering =
        (keys::isKeyPressed('W') && !this->state.W_KEY) ||
        (keys::isKeyPressed('A') && !this->state.A_KEY) ||
        (keys::isKeyPressed('S') && !this->state.S_KEY) ||
        (keys::isKeyPressed('D') && !this->state.D_KEY);

    if (userIsInterfering)
    {
      resetState();
      return;
    }

    Vector2 relVelocity = cache.localPlayer.relVelocity;

    if (std::abs(relVelocity.x) > 68)
    {
      if (relVelocity.x > 0.0f)
      {
        keys::sendKeyDown('S');
        this->state.S_KEY = true;
      }
      else
      {
        keys::sendKeyDown('W');
        this->state.W_KEY = true;
      }
    }
    else
    {
      if (this->state.S_KEY)
      {
        keys::sendKeyUp('S');
        this->state.S_KEY = false;
      }
      if (this->state.W_KEY)
      {
        keys::sendKeyUp('W');
        this->state.W_KEY = false;
      }
    }

    if (std::abs(relVelocity.y) > 68)
    {
      if (relVelocity.y > 0.0f)
      {
        keys::sendKeyDown('A');
        this->state.A_KEY = true;
      }
      else
      {
        keys::sendKeyDown('D');
        this->state.D_KEY = true;
      }
    }
    else
    {
      if (this->state.A_KEY)
      {
        keys::sendKeyUp('A');
        this->state.A_KEY = false;
      }
      if (this->state.D_KEY)
      {
        keys::sendKeyUp('D');
        this->state.D_KEY = false;
      }
    } });
}

void AutoStrafe::resetState()
{
  if (state.W_KEY)
  {
    state.W_KEY = false;
    keys::sendKeyUp('W');
  }

  if (state.A_KEY)
  {
    state.A_KEY = false;
    keys::sendKeyUp('A');
  }

  if (state.S_KEY)
  {
    state.S_KEY = false;
    keys::sendKeyUp('S');
  }

  if (state.D_KEY)
  {
    state.D_KEY = false;
    keys::sendKeyUp('D');
  }
}
