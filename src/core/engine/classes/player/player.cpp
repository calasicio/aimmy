#include "player.hpp"

#include "core/engine/engine.hpp"
#include "core/offsets/offsets.hpp"

bool Player::update()
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

bool Player::getController()
{
  auto process = Engine::getProcess();

  this->controller = process->read<DWORD64>(this->listEntry + (this->index + 1) * 0x70);

  return this->controller != 0;
}

bool Player::getPawn()
{
  auto process = Engine::getProcess();
  auto client = Engine::getClient();

  auto entityPawnAddress = process->read<uintptr_t>(this->controller + offsets::player::controller::m_hPawn);

  if (!entityPawnAddress)
    return false;

  auto entityPawnListEntry = process->read<uintptr_t>(this->entityList + 0x10 + 0x8 * ((entityPawnAddress & 0x7FFF) >> 9));

  if (!entityPawnListEntry)
    return false;

  this->pawn = process->read<uintptr_t>(entityPawnListEntry + 0x70 * (entityPawnAddress & 0x1FF));

  return this->pawn != 0;
}

bool Player::updatePawn()
{
  auto process = Engine::getProcess();

  this->health = process->read<int>(pawn + offsets::entities::base::m_iHealth);
  this->isAlive = health > 0;

  if (!isAlive)
  {
    origin = Vector3{0, 0, 0};
    return true;
  }

  this->origin = process->read<Vector3>(this->pawn + offsets::player::pawn::m_vOldOrigin);
  this->teamNum = process->read<uint8_t>(this->pawn + offsets::entities::base::m_iTeamNum);

  return true;
}