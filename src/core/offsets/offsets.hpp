#pragma once

#include <cstdint>

namespace offsets
{
  inline std::ptrdiff_t dwLocalPlayerController = 0x23A0F30;
  inline std::ptrdiff_t dwLocalPlayerPawn = 0x23C6268;
  inline std::ptrdiff_t dwEntityList = 0x2571220;
  inline std::ptrdiff_t dwSensitivity = 0x23C3578;
  inline std::ptrdiff_t dwSensitivity_sensitivity = 0x58;

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
  }

}