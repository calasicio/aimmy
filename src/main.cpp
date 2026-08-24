#include <iostream>
#include <thread>
#include <chrono>
#include <Windows.h>

#include "core/memory.hpp"
#include "game/offsets.hpp"
#include "game/structures.hpp"

#include "features/rcs.hpp"

#include "utils/config.hpp"

bool isApplicationRunning = true;

int main()
{

  std::cout << "[Aimmy] >> INFO: Setting up core... \n\n";

  Config::LoadSettings("settings.json");

  if (!g_pProcess.AttachProcess("cs2.exe"))
  {
    std::cerr << "[Aimmy] >> ERR: Could not locate or hook onto cs2.exe.\n";
    std::cout << "[Aimmy] >> INFO: Make sure Counter-Strike 2 is fully loaded before launching.\n";
    std::cin.get();
    return 1;
  }

  std::uintptr_t clientBaseAddress = g_pProcess.GetModule("client.dll").base;
  if (!clientBaseAddress)
  {
    std::cerr << "[Aimmy] >> ERR: Failed to find client.dll memory spaces.\n";
    g_pProcess.Close();
    std::cin.get();
    return 1;
  }

  std::cout << "[Aimmy] >> INFO: Connected successfully to CS2 Process Engine Handle!\n";
  std::cout << "[Aimmy] >> INFO: Found client.dll address at base boundary: 0x" << std::hex << clientBaseAddress << std::dec << "\n\n";

  std::cout << "--------------------------------------------------\n";
  std::cout << "                   HOW TO OPERATE                 \n";
  std::cout << "--------------------------------------------------\n";
  std::cout << "[*] RECOIL CONTROL: Automatic math kicks in whenever spraying.\n";
  std::cout << "[*] TRIGGERBOT: Hold [MOUSE 5 / XBUTTON2] key to look for targets.\n";
  std::cout << "[*] EXIT SYSTEM: Press the physical [END] keyboard key to close.\n";
  std::cout << "--------------------------------------------------\n\n";

  while (isApplicationRunning)
  {
    if (GetAsyncKeyState(VK_END) & 0x8000)
    {
      isApplicationRunning = false;
      break;
    }

    std::uintptr_t localPlayerPawnPointer = g_pProcess.read<std::uintptr_t>(
        clientBaseAddress + cs2_dumper::offsets::client_dll::dwLocalPlayerPawn);

    if (localPlayerPawnPointer != 0)
    {
      features::ExecuteRCS(localPlayerPawnPointer, clientBaseAddress);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  std::cout << "[Aimmy] >> INFO: Closing external runtime environments cleanly...\n";

  g_pProcess.Close();

  std::cout << "[Aimmy] >> INFO: Memory link fully broken. Closing helper tool safely.\n";
  return 0;
}
