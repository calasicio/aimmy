#pragma once

#include <cstdint>
#include <cstddef>

namespace offsets
{
  std::ptrdiff_t dwLocalPlayerController = 0x23A0F30;
  std::ptrdiff_t dwLocalPlayerPawn = 0x23C6268;
  std::ptrdiff_t dwEntityList = 0x2571220;
  std::ptrdiff_t dwSensitivity = 0x23C3578;
  std::ptrdiff_t dwSensitivity_sensitivity = 0x58;

  namespace C_CSPlayerPawn
  {
    std::ptrdiff_t m_iShotsFired = 0x1C8C;
    std::ptrdiff_t m_pAimPunchServices = 0x14B8;
  }

  namespace CGameSceneNode
  {
    std::ptrdiff_t m_vecOrigin = 0x80;

    namespace CSkeletonInstance
    {
      std::ptrdiff_t m_modelState = 0x140;
    }
  }

  namespace C_BaseEntity
  {
    std::ptrdiff_t m_pGameSceneNode = 0x330;
    std::ptrdiff_t m_iHealth = 0x34C;
    std::ptrdiff_t m_iTeamNum = 0x3E7;
  }

}