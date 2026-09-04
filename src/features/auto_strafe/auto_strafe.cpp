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
    bool userIsInterfering =
        (keys::isHumanHoldingKey('W')) ||
        (keys::isHumanHoldingKey('A')) ||
        (keys::isHumanHoldingKey('S')) ||
        (keys::isHumanHoldingKey('D'));

    if (userIsInterfering)
    {
      resetState();
      return;
    }

    if (!cache.localPlayer.isOnGround || cache.localPlayer.moveType != 2) {
      resetState();
      return;
    }

    Vector2 relVelocity = cache.localPlayer.relVelocity;

    if (std::abs(relVelocity.x) > cache.convars.svStopSpeed * 0.7)
    {
      if (relVelocity.x > 0.0f && !this->state.S_KEY)
      {
        keys::sendKeyDown('S');
        this->state.S_KEY = true;
      }
      else if (relVelocity.x < 0.0f && !this->state.W_KEY)
      {
        keys::sendKeyDown('W');
        this->state.W_KEY = true;
      }
    }
    else
    {
      if (this->state.S_KEY && shouldRelease(cache, relVelocity.x))
      {
        keys::sendKeyUp('S');
        this->state.S_KEY = false;
      }
      if (this->state.W_KEY && shouldRelease(cache, relVelocity.x))
      {
        keys::sendKeyUp('W');
        this->state.W_KEY = false;
      }
    }

    if (std::abs(relVelocity.y) > cache.convars.svStopSpeed * 0.7)
    {
      if (relVelocity.y > 0.0f && !this->state.A_KEY)
      {
        keys::sendKeyDown('A');
        this->state.A_KEY = true;
      }
      else if (relVelocity.y < 0.0f && !this->state.D_KEY)
      {
        keys::sendKeyDown('D');
        this->state.D_KEY = true;
      }
    }
    else
    {
      if (this->state.A_KEY && shouldRelease(cache, relVelocity.y))
      {
        keys::sendKeyUp('A');
        this->state.A_KEY = false;
      }
      if (this->state.D_KEY && shouldRelease(cache, relVelocity.y))
      {
        keys::sendKeyUp('D');
        this->state.D_KEY = false;
      }
    }
  });
}

bool AutoStrafe::shouldRelease(const Cache &cache, float currentVel)
{
  constexpr float GAME_TICK_INTERVAL = 1.0f / 64.0f;

  float vel = currentVel;

  for (int i = 0; i < 3; ++i)
  {
    float speed = std::abs(vel);
    float control = (speed < cache.convars.svStopSpeed) ? cache.convars.svStopSpeed : speed;
    float drop = control * cache.convars.svFriction * GAME_TICK_INTERVAL;
    speed = std::max<float>(0.0f, speed - drop);
    vel = (vel > 0.0f) ? speed : -speed;

    float accelSpeed = cache.convars.svAccelerate * GAME_TICK_INTERVAL * cache.localPlayer.maxMovementSpeed;
    if (currentVel > 0.0f)
    {
      vel -= accelSpeed;
    }
    else
    {
      vel += accelSpeed;
    }

    if ((currentVel > 0.0f && vel <= 0.0f) || (currentVel < 0.0f && vel >= 0.0f))
    {
      return true;
    }
  }

  return std::abs(currentVel) <= cache.convars.svStopSpeed * 0.7;
}

void AutoStrafe::resetState()
{
  if (state.W_KEY)
  {
    state.W_KEY = false;
    if (!keys::isHumanHoldingKey('W'))
      keys::sendKeyUp('W');
  }

  if (state.A_KEY)
  {
    state.A_KEY = false;
    if (!keys::isHumanHoldingKey('A'))
      keys::sendKeyUp('A');
  }

  if (state.S_KEY)
  {
    state.S_KEY = false;
    if (!keys::isHumanHoldingKey('S'))
      keys::sendKeyUp('S');
  }

  if (state.D_KEY)
  {
    state.D_KEY = false;
    if (!keys::isHumanHoldingKey('D'))
      keys::sendKeyUp('D');
  }
}
