#include "local_player.hpp"

#include "core/engine/cache/cache.hpp"
#include "core/engine/engine.hpp"
#include "core/offsets/offsets.hpp"
#include "utils/logger/logger.hpp"

struct CUtlVector_t
{
  uintptr_t count = 0;
  uintptr_t data = 0;
};

bool LocalPlayer::update()
{
  if (!Engine::getProcess())
    return false;

  if (!getController())
    return false;

  if (!getPawn())
    return false;

  if (!updatePawn())
    return false;

  return true;
}

bool LocalPlayer::getPawn()
{
  auto process = Engine::getProcess();
  auto client = Engine::getClient();

  this->pawn = process->read<std::uintptr_t>(client.base + offsets::player::localplayer::dwLocalPlayerPawn);
  return this->pawn != 0;
}

bool LocalPlayer::getController()
{
  auto process = Engine::getProcess();
  auto client = Engine::getClient();

  this->controller = process->read<std::uintptr_t>(client.base + offsets::player::localplayer::dwLocalPlayerController);
  return this->controller != 0;
}

bool LocalPlayer::updatePawn()
{
  if (!Player::updatePawn())
    return false;

  if (!this->isAlive)
    return true;

  auto process = Engine::getProcess();
  auto client = Engine::getClient();

  this->viewAngle = process->read<Vector3>(client.base + offsets::player::localplayer::dwViewAngles);

  updateVelocity();
  updateMovement();
  updateWeapon();

  Vector3 eyeOffset = process->read<Vector3>(this->pawn + offsets::entities::base::m_vecViewOffset);
  this->cameraPos = this->origin + eyeOffset;

  this->shotsFired = process->read<int>(this->pawn + offsets::player::pawn::m_iShotsFired);

  if (!updateAimPunch())
  {
    this->aimPunch = {0, 0, 0};
  }

  return true;
}

bool LocalPlayer::updateAimPunch()
{
  auto process = Engine::getProcess();

  const uintptr_t aimPunchServices = process->read<uintptr_t>(this->pawn + offsets::player::pawn::m_pAimPunchServices);
  if (!aimPunchServices)
    return false;

  CUtlVector_t aimPunchCache = process->read<CUtlVector_t>(aimPunchServices + 0x88);
  if (aimPunchCache.count <= 0 || aimPunchCache.count >= 0xFFFF)
    return false;

  this->aimPunch = process->read<Vector3>(aimPunchCache.data + (aimPunchCache.count - 1) * sizeof(Vector3));
  return true;
}

void LocalPlayer::updateVelocity()
{
  auto process = Engine::getProcess();

  this->velocity = process->read<Vector3>(this->pawn + offsets::entities::base::m_vecAbsVelocity);

  float yawRadians = this->viewAngle.y * (3.14159265f / 180.0f);
  float fwdX = std::cos(yawRadians);
  float fwdY = std::sin(yawRadians);
  float rgtX = std::sin(yawRadians);
  float rgtY = -std::cos(yawRadians);

  this->relVelocity.x = (this->velocity.x * fwdX) + (this->velocity.y * fwdY);
  this->relVelocity.y = (this->velocity.x * rgtX) + (this->velocity.y * rgtY);
}

void LocalPlayer::updateMovement()
{
  auto process = Engine::getProcess();

  uint32_t flags = process->read<uint32_t>(this->pawn + offsets::entities::base::m_fFlags);
  this->isOnGround = (flags & (1 << 0));

  this->moveType = process->read<uint8_t>(this->pawn + offsets::entities::base::m_MoveType);
}

void LocalPlayer::updateWeapon()
{
  auto process = Engine::getProcess();
  Cache &cache = Cache::getInstance();

  std::uintptr_t weaponServices = process->read<uintptr_t>(this->pawn + offsets::weapon::m_pWeaponServices);
  if (!weaponServices)
    return;

  uint32_t activeWeaponHandle = process->read<uint32_t>(weaponServices + offsets::weapon::m_hActiveWeapon);
  if (activeWeaponHandle == 0xFFFFFFFF || activeWeaponHandle == 0)
    return;

  uintptr_t listEntry = process->read<uintptr_t>(cache.game.entityList + 0x10 + 8 * ((activeWeaponHandle & 0x7FFF) >> 9));
  if (!listEntry)
    return;

  uintptr_t weaponEntity = process->read<uintptr_t>(listEntry + 0x70 * (activeWeaponHandle & 0x1FF));
  if (!weaponEntity)
    return;

  // this->currentWeaponID = process->read<uint16_t>(
  //     weaponEntity + offsets::weapon::m_AttributeManager + offsets::weapon::m_Item + offsets::weapon::m_iItemDefinitionIndex);

  uintptr_t vData = process->read<uintptr_t>(weaponEntity + offsets::entities::base::m_nSubclassID + 0x08);

  if (!vData) {
    return;
  }

  this->maxMovementSpeed = process->read<float>(vData + offsets::entities::base::m_flMaxSpeed);
}