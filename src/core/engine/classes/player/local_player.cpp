#include "local_player.hpp"

#include "core/engine/engine.hpp"
#include "core/offsets/offsets.hpp"

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

  this->pawn = process->read<std::uintptr_t>(client.base + offsets::dwLocalPlayerPawn);
  return this->pawn != 0;
}

bool LocalPlayer::getController()
{
  auto process = Engine::getProcess();
  auto client = Engine::getClient();

  this->controller = process->read<std::uintptr_t>(client.base + offsets::dwLocalPlayerController);
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

  this->viewAngle = process->read<Vector3>(client.base + offsets::dwViewAngles);

  Vector3 eyeOffset = process->read<Vector3>(this->pawn + offsets::C_BaseModelEntity::m_vecViewOffset);
  this->cameraPos = this->origin + eyeOffset;

  this->shotsFired = process->read<int>(this->pawn + offsets::C_CSPlayerPawn::m_iShotsFired);

  if (!updateAimPunch())
  {
    this->aimPunch = {0, 0, 0};
  }

  return true;
}

bool LocalPlayer::updateAimPunch()
{
  auto process = Engine::getProcess();

  const uintptr_t aimPunchServices = process->read<uintptr_t>(this->pawn + offsets::C_CSPlayerPawn::m_pAimPunchServices);
  if (!aimPunchServices)
    return false;

  CUtlVector_t aimPunchCache = process->read<CUtlVector_t>(aimPunchServices + 0x88);
  if (aimPunchCache.count <= 0 || aimPunchCache.count >= 0xFFFF)
    return false;

  this->aimPunch = process->read<Vector3>(aimPunchCache.data + (aimPunchCache.count - 1) * sizeof(Vector3));
  return true;
}
