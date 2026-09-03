#pragma once

#include <cstdint>
#include "core/engine/types/vector2.hpp"
#include "core/engine/types/vector3.hpp"

class Player
{
public:
  Player() {}
  Player(int index, uintptr_t entityList, uintptr_t listEntry)
      : index(index), entityList(entityList), listEntry(listEntry) {}

  virtual ~Player() = default;

  virtual bool update();

public:
  int8_t index = -1;

  std::uintptr_t pawn;
  std::uintptr_t controller;

  int health;
  bool isAlive;
  int teamNum;
  Vector3 origin;

private:
  uintptr_t entityList;
  uintptr_t listEntry;

private:
  bool getPawn();
  bool getController();

protected:
  virtual bool updatePawn();
};
