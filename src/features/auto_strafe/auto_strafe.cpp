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
    if (!mouse::isButtonPressed(5)) {
      resetState();
      return;
    };

    bool userIsInterfering = 
        (keys::isKeyPressed('W') && !this->state.W_KEY.state) ||
        (keys::isKeyPressed('A') && !this->state.A_KEY.state) ||
        (keys::isKeyPressed('S') && !this->state.S_KEY.state) ||
        (keys::isKeyPressed('D') && !this->state.D_KEY.state);

    if (userIsInterfering)
    {
      resetState();
      return;
    }

    Vector2 relVelocity = cache.localPlayer.relVelocity;

    if (!this->state.S_KEY.state && !this->state.W_KEY.state) 
    {
      if (std::abs(relVelocity.x) > 10.0f) {
        if (relVelocity.x > 0.0f) {
          float computedTime = getTimeToStop(cache, relVelocity.x);
          if (computedTime > 0.0f) {
            keys::sendKeyDown('S');
            logger::info("Setting state to: S_KEY -> true, " + std::to_string(computedTime));
            this->state.S_KEY.state = true;
            this->state.S_KEY.time = computedTime;
          }
        } else {
          float computedTime = getTimeToStop(cache, relVelocity.x);
          if (computedTime > 0.0f) {
            keys::sendKeyDown('W');
            logger::info("Setting state to: W_KEY -> true, " + std::to_string(computedTime));
            this->state.W_KEY.state = true;
            this->state.W_KEY.time = computedTime;
          }
        }
      }
    }

    if (!this->state.A_KEY.state && !this->state.D_KEY.state) 
    {
      if (std::abs(relVelocity.y) > 10.0f) {
        if (relVelocity.y > 0.0f) {
          float computedTime = getTimeToStop(cache, relVelocity.y);
          if (computedTime > 0.0f) {
            keys::sendKeyDown('A');
            logger::info("Setting state to: A_KEY -> true, " + std::to_string(computedTime));
            this->state.A_KEY.state = true;
            this->state.A_KEY.time = computedTime;
          }
        } else {
          float computedTime = getTimeToStop(cache, relVelocity.y);
          if (computedTime > 0.0f) {
            keys::sendKeyDown('D');
            logger::info("Setting state to: D_KEY -> true, " + std::to_string(computedTime));
            this->state.D_KEY.state = true;
            this->state.D_KEY.time = computedTime;
          }
        }
      }
    }

    if (this->state.W_KEY.state) {
      if (this->state.W_KEY.time > 0.0f) {
        this->state.W_KEY.time -= dt * 1000;
      } else {
        this->state.W_KEY.state = false;
        keys::sendKeyUp('W');
      }
    }

    if (this->state.S_KEY.state) {
      if (this->state.S_KEY.time > 0.0f) {
        this->state.S_KEY.time -= dt * 1000;
      } else {
        this->state.S_KEY.state = false;
        keys::sendKeyUp('S');
      }
    }

    if (this->state.A_KEY.state) {
      if (this->state.A_KEY.time > 0.0f) {
        this->state.A_KEY.time -= dt * 1000;
      } else {
        this->state.A_KEY.state = false;
        keys::sendKeyUp('A');
      }
    }

    if (this->state.D_KEY.state) {
      if (this->state.D_KEY.time > 0.0f) {
        this->state.D_KEY.time -= dt * 1000;
      } else {
        this->state.D_KEY.state = false;
        keys::sendKeyUp('D');
      }
    } });
}

void AutoStrafe::resetState()
{
  if (state.W_KEY.state)
  {
    state.W_KEY.state = false;
    state.W_KEY.time = 0.0f;
    keys::sendKeyUp('W');
  }

  if (state.A_KEY.state)
  {
    state.A_KEY.state = false;
    state.A_KEY.time = 0.0f;
    keys::sendKeyUp('A');
  }

  if (state.S_KEY.state)
  {
    state.S_KEY.state = false;
    state.S_KEY.time = 0.0f;
    keys::sendKeyUp('S');
  }

  if (state.D_KEY.state)
  {
    state.D_KEY.state = false;
    state.D_KEY.time = 0.0f;
    keys::sendKeyUp('D');
  }
}

float AutoStrafe::getTimeToStop(const Cache &cache, float speed)
{
  float currentSpeed = std::abs(speed);

  constexpr float targetAccuracySpeed = 34.0f;

  if (currentSpeed <= targetAccuracySpeed)
    return 0.0f;

  float accel = cache.convars.accelerate;
  float stopSpeed = cache.convars.stopSpeed;
  float friction = cache.convars.friction;
  float maxWepSpeed = cache.localPlayer.maxMovementSpeed;

  float pushForce = accel * maxWepSpeed;

  float totalTimeSeconds = 0.0f;

  if (currentSpeed > stopSpeed)
  {
    float numerator = (friction * currentSpeed) + pushForce;
    float denominator = (friction * stopSpeed) + pushForce;

    totalTimeSeconds += (1.0f / friction) * std::log(numerator / denominator);
    currentSpeed = stopSpeed;
  }

  if (currentSpeed > targetAccuracySpeed)
  {
    float constantDecelRate = (stopSpeed * friction) + pushForce;
    totalTimeSeconds += (currentSpeed - targetAccuracySpeed) / constantDecelRate;
  }

  return totalTimeSeconds * 1000.0f;
}
