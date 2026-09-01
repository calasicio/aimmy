#pragma once

#include <cstdint>

namespace offsets
{
  inline std::ptrdiff_t dwGlobalVars = 0x20AF5F0;
  inline std::ptrdiff_t dwLocalPlayerController = 0x23A0F30;
  inline std::ptrdiff_t dwLocalPlayerPawn = 0x23C6268;
  inline std::ptrdiff_t dwEntityList = 0x2571220;
  inline std::ptrdiff_t dwSensitivity = 0x23C3578;
  inline std::ptrdiff_t dwSensitivity_sensitivity = 0x58;
  inline std::ptrdiff_t dwViewAngles = 0x23DC2F8;
  inline std::ptrdiff_t dwViewMatrix = 0x23CB830;

  inline std::ptrdiff_t c_hud = 0x2436710;

  inline std::ptrdiff_t dwWindowHeight = 0x9118D4;
  inline std::ptrdiff_t dwWindowWidth = 0x9118D0;

  namespace CCSGO_HudTree
  {
    constexpr std::uintptr_t Base = 0x258;  // DAT_182436710 + 600 (600 dec = 0x258)
    constexpr std::uintptr_t Count = 0x264; // *(uint *)(DAT_182436710 + 0x264)
    constexpr std::uintptr_t Pool = 0x268;  // *(longlong *)(DAT_182436710 + 0x268)
  }

  // Just decompile the game and look at the CCSGO_HudRadar struct, the offsets are there
  namespace CCSGO_HudRadar
  {
    constexpr std::uintptr_t IsRound = 0x60;
    constexpr std::uintptr_t MapTexturePosition = 0x190;
    constexpr std::uintptr_t VisibilitySizeMax = 0x19C;
    constexpr std::uintptr_t VisibilitySize = 0x1A0;
    constexpr std::uintptr_t MapTextureScale = 0x1B4;
    constexpr std::uintptr_t MaxVisibilitySquared = 0x1B8;
    constexpr std::uintptr_t OriginTextureDiff = 0x1D0;
    constexpr std::uintptr_t RadarScale = 0x17F8C;
  }

  namespace CGlobalVarsBase
  {
    inline std::ptrdiff_t m_iMaxClients = 0x10;
    inline std::ptrdiff_t m_strMapName = 0x180;
  }

  namespace C_BasePlayerPawn
  {
    inline std::ptrdiff_t m_vOldOrigin = 0x13B8;
  }

  namespace C_BaseModelEntity
  {
    inline std::ptrdiff_t m_vecViewOffset = 0xE78;
  }

  namespace C_CSPlayerPawn
  {
    inline std::ptrdiff_t m_iShotsFired = 0x1C8C;
    inline std::ptrdiff_t m_pAimPunchServices = 0x14B8;
  }

  namespace CGameSceneNode
  {
    inline std::ptrdiff_t m_vecOrigin = 0x80;

    namespace CSkeletonInstance
    {
      inline std::ptrdiff_t m_modelState = 0x140;
    }
  }

  namespace C_BaseEntity
  {
    inline std::ptrdiff_t m_pGameSceneNode = 0x330;
    inline std::ptrdiff_t m_iHealth = 0x34C;
    inline std::ptrdiff_t m_iTeamNum = 0x3E7;

    namespace CBasePlayerController
    {
      inline std::ptrdiff_t m_hPawn = 0x6BC;
      inline std::ptrdiff_t m_bIsLocalPlayerController = 0x788;
    }
  }

}