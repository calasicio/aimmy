#include "dumper.hpp"

#include <format>
#include <string>
#include <iostream>

#include "offsets.hpp"
#include "utils/filesystem/filesystem.hpp"
#include "utils/json.hpp"
#include "utils/logger/logger.hpp"

bool Dumper::init()
{
  return getInstance().initImpl();
}

bool Dumper::initImpl()
{
  if (!runDumper())
    return false;

  return loadOffsets();
}

bool Dumper::runDumper()
{
  if (!filesystem::isFile("cs2-dumper.exe"))
  {
    logger::fatal(
        "Could not find cs2-dumper.exe, "
        "please make sure it is in the same directory as aimmy");

    return false;
  }

  return std::system("cs2-dumper.exe --file-types json") == 0;
}

bool readOffset(
    const json &data,
    std::ptrdiff_t &destination,
    std::string_view name,
    std::initializer_list<std::string_view> path)
{
  if (!json_utils::read(data, destination, path))
  {
    logger::error(
        std::format(
            "Offset `{}` was not found or is invalid",
            name));

    return false;
  }

  return true;
}

bool Dumper::loadOffsets()
{
  if (!filesystem::isDirectory("output"))
  {
    logger::error(
        "Could not find `output` directory, "
        "please make sure `cs2-dumper.exe` has been run successfully");

    return false;
  }

  if (!filesystem::isFile("output/offsets.json"))
  {
    logger::error(
        "Could not find `output/offsets.json`, "
        "please make sure `cs2-dumper.exe` has been run successfully");

    return false;
  }

  if (!filesystem::isFile("output/client_dll.json"))
  {
    logger::error(
        "Could not find `output/client_dll.json`, "
        "please make sure `cs2-dumper.exe` has been run successfully");

    return false;
  }

  const auto offsetsData =
      filesystem::readJson("output/offsets.json");

  const auto clientDLLData =
      filesystem::readJson("output/client_dll.json");

  if (offsetsData.empty())
  {
    logger::error("`output/offsets.json` is empty");
    return false;
  }

  if (clientDLLData.empty())
  {
    logger::error("`output/client_dll.json` is empty");
    return false;
  }

  // ======================================
  // Offset Map

  bool success = true;

  // Base Offsets
  success &= readOffset(
      offsetsData,
      offsets::dwLocalPlayerController,
      "dwLocalPlayerController",
      {"client.dll", "dwLocalPlayerController"});

  success &= readOffset(
      offsetsData,
      offsets::dwLocalPlayerPawn,
      "dwLocalPlayerPawn",
      {"client.dll", "dwLocalPlayerPawn"});

  success &= readOffset(
      offsetsData,
      offsets::dwEntityList,
      "dwEntityList",
      {"client.dll", "dwEntityList"});

  success &= readOffset(
      offsetsData,
      offsets::dwSensitivity,
      "dwSensitivity",
      {"client.dll", "dwSensitivity"});

  success &= readOffset(
      offsetsData,
      offsets::dwSensitivity_sensitivity,
      "dwSensitivity_sensitivity",
      {"client.dll", "dwSensitivity_sensitivity"});

  // C_CSPlayerPawn
  success &= readOffset(
      clientDLLData,
      offsets::C_CSPlayerPawn::m_iShotsFired,
      "m_iShotsFired",
      {"client.dll", "classes", "C_CSPlayerPawn", "fields", "m_iShotsFired"});

  success &= readOffset(
      offsetsData,
      offsets::C_CSPlayerPawn::m_pAimPunchServices,
      "m_pAimPunchServices",
      {"client.dll", "classes", "C_CSPlayerPawn", "fields", "m_pAimPunchServices"});

  // CGameSceneNode
  success &= readOffset(
      offsetsData,
      offsets::CGameSceneNode::m_vecOrigin,
      "m_vecOrigin",
      {"client.dll", "classes", "CGameSceneNode", "fields", "m_vecOrigin"});

  // |- CSkeletonInstance
  success &= readOffset(
      offsetsData,
      offsets::CGameSceneNode::CSkeletonInstance::m_modelState,
      "m_modelState",
      {"client.dll", "classes", "CSkeletonInstance", "fields", "m_modelState"});

  // C_BaseEntity
  success &= readOffset(
      offsetsData,
      offsets::C_BaseEntity::m_pGameSceneNode,
      "m_pGameSceneNode",
      {"client.dll", "classes", "C_BaseEntity", "fields", "m_pGameSceneNode"});

  success &= readOffset(
      offsetsData,
      offsets::C_BaseEntity::m_iTeamNum,
      "m_iTeamNum",
      {"client.dll", "classes", "C_BaseEntity", "fields", "m_iTeamNum"});

  return success;
}