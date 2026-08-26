#pragma once

#include <cstdint>
#include <cstddef>

namespace offsets
{
  std::ptrdiff_t dwLocalPlayerPawn;
  std::ptrdiff_t dwEntityList;
  std::ptrdiff_t dwSensitivity;
  std::ptrdiff_t dwSensitivity_sensitivity;

  namespace playerPawn
  {
    std::ptrdiff_t m_iShotsFired;
    std::ptrdiff_t m_pAimPunchServices;
  }
}