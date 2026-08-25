#pragma once
#include <cstdint>
#include <cstddef>

namespace cs2_dumper
{
  namespace offsets
  {
    namespace client_dll
    {
      constexpr std::ptrdiff_t dwLocalPlayerPawn = 0x23C7268;
      constexpr std::ptrdiff_t dwEntityList = 0x2572230;
      constexpr std::ptrdiff_t dwSensitivity = 0x23C4578;
      constexpr std::ptrdiff_t dwSensitivity_sensitivity = 0x58;
    }
  }

  namespace schemas
  {
    namespace client_dll
    {
      namespace C_BaseEntity
      {
        constexpr std::ptrdiff_t m_iHealth = 0x34C;
      }
      namespace C_CSPlayerPawn
      {
        constexpr std::ptrdiff_t m_iShotsFired = 0x1C8C;
        constexpr std::ptrdiff_t m_pAimPunchServices = 0x14B8;
      }
    }
  }
}
