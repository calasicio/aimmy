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
    velocity = Vector3{0, 0, 0};
    speed = 0;
    return true;
  }

  this->origin = process->read<Vector3>(this->pawn + offsets::player::pawn::m_vOldOrigin);
  this->velocity = process->read<Vector3>(this->pawn + offsets::entities::base::m_vecAbsVelocity);
  this->speed = this->velocity.length();
  this->teamNum = process->read<uint8_t>(this->pawn + offsets::entities::base::m_iTeamNum);

  int shotsFired_ = process->read<int>(this->pawn + offsets::player::pawn::m_iShotsFired);
  this->shotsFired.old = this->shotsFired.current;
  this->shotsFired.current = shotsFired_;

  this->spottedMask = process->read<uint32_t>(this->pawn + offsets::player::pawn::m_entitySpottedState + offsets::player::pawn::m_bSpottedByMask);

  bool isDefusing_ = process->read<bool>(this->pawn + offsets::player::pawn::m_bIsDefusing);
  this->isDefusing.old = this->isDefusing.current;
  this->isDefusing.current = isDefusing_;

  bool isGrabbingHostage_ = process->read<bool>(this->pawn + offsets::player::pawn::m_bIsGrabbingHostage);
  this->isGrabbingHostage.old = this->isGrabbingHostage.current;
  this->isGrabbingHostage.current = isGrabbingHostage_;

  updateSoundStates();

  return true;
}

/*
 * TODO:
 * - Landing sounds (also somehow handle the silent landing too)
 * - Utility sounds
 *      |-- Get all the utlities thrown by a player,
 *          get the bouncing sound, explosion sound, unpin sound
 */

bool Player::updateSoundStates()
{
  auto process = Engine::getProcess();

  bool isMakingSound = false;

  // DEFUSING
  if (!this->isDefusing.old && this->isDefusing.current)
  {
    this->lastSoundMade.soundName = "DEFUSING";
    this->lastSoundMade.radius = 700;

    isMakingSound = true;
  }

  // GRABBING_HOSTAGE
  if (!this->isGrabbingHostage.old && this->isGrabbingHostage.current)
  {
    this->lastSoundMade.soundName = "GRABBING_HOSTAGE";
    this->lastSoundMade.radius = 700;

    isMakingSound = true;
  }

  // SHOOTING
  if (this->shotsFired.old == 0 && this->shotsFired.current > 0)
  {
    this->lastSoundMade.soundName = "SHOOTING";
    this->lastSoundMade.radius = 999999;

    isMakingSound = true;
  }

  // RUNNING
  if (this->speed >= 135)
  {
    this->lastSoundMade.soundName = "RUNNING";
    this->lastSoundMade.radius = 1000;

    isMakingSound = true;
  }

  if (isMakingSound)
  {
    this->lastSoundMade.timestamp = std::chrono::steady_clock::now();
  }

  return true;
}