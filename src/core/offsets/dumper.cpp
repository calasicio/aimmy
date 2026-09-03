#include "dumper.hpp"

#include <format>
#include <string>
#include <iostream>

#include "offsets.hpp"
#include "core/engine/engine.hpp"
#include "utils/filesystem/filesystem.hpp"
#include "utils/json.hpp"
#include "utils/logger/logger.hpp"

bool Dumper::init()
{
  return getInstance().initImpl();
}

bool Dumper::initImpl()
{
  logger::info("Finding `c_hud` offset...");
  findCHud();

  logger::info("Finding `CCVar` offset...");
  findCCVar();

  if (!runDumper())
    return false;

  return loadOffsets();
}

bool Dumper::findCHud()
{
  auto process = Engine::getProcess();
  auto client = Engine::getClient();

  // 48 89 5C 24 20 57 48 83 EC 20 0F B6 DA 48 8B F9
  std::vector<uint8_t> signature = {0x48, 0x89, 0x5C, 0x24, 0x20, 0x57, 0x48, 0x83, 0xEC, 0x20, 0x0F, 0xB6, 0xDA, 0x48, 0x8B, 0xF9};
  std::vector<bool> mask = {true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true};

  std::uintptr_t sigAddr = process->FindSignature(client, signature, mask);

  if (sigAddr != 0)
  {
    logger::info("Found HUD manager function at: " + std::to_string(sigAddr));

    std::uintptr_t displacementAddress = sigAddr + 0xEF + 3;

    int32_t relativeOffset = process->read<int32_t>(displacementAddress);

    uintptr_t cHudAbsoluteAddress = displacementAddress + 4 + relativeOffset;

    offsets::c_hud = cHudAbsoluteAddress - client.base;

    char hexStr[32];
    sprintf_s(hexStr, "0x%llX", offsets::c_hud);
    logger::info(std::string("Offset `c_hud` found at ") + hexStr);
  }
  else
  {
    logger::error(
        "Could not find `c_hud` offset, using default offset value. Radar may or may not work.");

    return false;
  }

  return true;
}

/*
Module: tier0.dll
Pattern: 4C 8D 3D ? ? ? ? 0F 28 45
Type: RIP-relative LEA (lea r15, [rip+...])
Displacement offset: +3
Instruction size: 7
*/
bool Dumper::findCCVar()
{
  auto process = Engine::getProcess();
  auto tier0 = Engine::getTier0();

  std::vector<uint8_t> signature = {0x4C, 0x8D, 0x3D, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x28, 0x45};
  std::vector<bool> mask = {true, true, true, false, false, false, false, true, true, true};

  std::uintptr_t sigAddr = process->FindSignature(tier0, signature, mask);

  if (sigAddr != 0)
  {
    int32_t rel = 0;
    process->read_raw(sigAddr + 3, &rel, sizeof(rel));
    uintptr_t cvarIf = sigAddr + 7 + rel;

    offsets::CCVars = cvarIf - tier0.base;

    char hexStr[32];
    sprintf_s(hexStr, "0x%llX", offsets::CCVars);
    logger::info(std::string("Offset `CCVar` found at ") + hexStr);
  }
  else
  {
    logger::error(
        "Could not find `CCVar` offset. This is a critical error and the program will likely crash.");

    return false;
  }

  return true;
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
        std::string("Offset `") + std::string(name) + "` was not found or is invalid");

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
      offsets::dwGlobalVars,
      "dwGlobalVars",
      {"client.dll", "dwGlobalVars"});

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
      offsets::dwViewAngles,
      "dwViewAngles",
      {"client.dll", "dwViewAngles"});

  success &= readOffset(
      offsetsData,
      offsets::dwViewMatrix,
      "dwViewMatrix",
      {"client.dll", "dwViewMatrix"});

  success &= readOffset(
      offsetsData,
      offsets::dwWindowHeight,
      "dwWindowHeight",
      {"engine2.dll", "dwWindowHeight"});

  success &= readOffset(
      offsetsData,
      offsets::dwWindowWidth,
      "dwWindowWidth",
      {"engine2.dll", "dwWindowWidth"});

  // C_BasePlayerPawn
  success &= readOffset(
      clientDLLData,
      offsets::C_BasePlayerPawn::m_vOldOrigin,
      "m_vOldOrigin",
      {"client.dll", "classes", "C_BasePlayerPawn", "fields", "m_vOldOrigin"});

  // C_BaseModelEntity
  success &= readOffset(
      clientDLLData,
      offsets::C_BaseModelEntity::m_vecViewOffset,
      "m_vecViewOffset",
      {"client.dll", "classes", "C_BaseModelEntity", "fields", "m_vecViewOffset"});

  // C_CSPlayerPawn
  success &= readOffset(
      clientDLLData,
      offsets::C_CSPlayerPawn::m_iShotsFired,
      "m_iShotsFired",
      {"client.dll", "classes", "C_CSPlayerPawn", "fields", "m_iShotsFired"});

  success &= readOffset(
      clientDLLData,
      offsets::C_CSPlayerPawn::m_pAimPunchServices,
      "m_pAimPunchServices",
      {"client.dll", "classes", "C_CSPlayerPawn", "fields", "m_pAimPunchServices"});

  // CGameSceneNode
  success &= readOffset(
      clientDLLData,
      offsets::CGameSceneNode::m_vecOrigin,
      "m_vecOrigin",
      {"client.dll", "classes", "CGameSceneNode", "fields", "m_vecOrigin"});

  // |- CSkeletonInstance
  success &= readOffset(
      clientDLLData,
      offsets::CGameSceneNode::CSkeletonInstance::m_modelState,
      "m_modelState",
      {"client.dll", "classes", "CSkeletonInstance", "fields", "m_modelState"});

  // C_BaseEntity
  success &= readOffset(
      clientDLLData,
      offsets::C_BaseEntity::m_pGameSceneNode,
      "m_pGameSceneNode",
      {"client.dll", "classes", "C_BaseEntity", "fields", "m_pGameSceneNode"});

  success &= readOffset(
      clientDLLData,
      offsets::C_BaseEntity::m_iHealth,
      "m_iHealth",
      {"client.dll", "classes", "C_BaseEntity", "fields", "m_iHealth"});

  success &= readOffset(
      clientDLLData,
      offsets::C_BaseEntity::m_iTeamNum,
      "m_iTeamNum",
      {"client.dll", "classes", "C_BaseEntity", "fields", "m_iTeamNum"});

  // |- CBasePlayerController
  success &= readOffset(
      clientDLLData,
      offsets::C_BaseEntity::CBasePlayerController::m_hPawn,
      "m_hPawn",
      {"client.dll", "classes", "CBasePlayerController", "fields", "m_hPawn"});

  success &= readOffset(
      clientDLLData,
      offsets::C_BaseEntity::CBasePlayerController::m_bIsLocalPlayerController,
      "m_bIsLocalPlayerController",
      {"client.dll", "classes", "CBasePlayerController", "fields", "m_bIsLocalPlayerController"});

  return success;
}