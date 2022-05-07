// Trainer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#include "mem.h"
#include "proc.h"
#include <TlHelp32.h>
#include <iostream>

int main()
{
    HANDLE hProcess = 0;

    uintptr_t moduleBase = 0, localPlayerPtr = 0, healthAddr = 0;
    bool bHealth = false, bAmmo = false, bRecoil = false;

    const int newValue = 444;

    DWORD procId = proc::GetProcId(L"ac_client.exe");

    if (procId)
    {
        hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);
        moduleBase = proc::GetModuleBaseAddress(procId, L"ac_client.exe");
        localPlayerPtr = moduleBase + 0x10F4F4;
        healthAddr = proc::FindDMAAddy(hProcess, localPlayerPtr, { 0xF8 });
    }
    else
    {
        std::cout << "didnt find the process.\n";
        std::cin.get();
    }

    DWORD dwExit = 0;

    while (GetExitCodeProcess(hProcess, &dwExit) && dwExit == STILL_ACTIVE)
    {
        if (GetAsyncKeyState(VK_NUMPAD1) & 1)
        {
            bHealth = !bHealth;
        }
        if (GetAsyncKeyState(VK_NUMPAD2) & 1)
        {
            bAmmo = !bAmmo;
            if (bAmmo)
            {
                mem::PatchEx((BYTE*)(moduleBase + 0x637e9), (BYTE*)"\xFF\x06", 2, hProcess);
            }
            else
            {
                mem::PatchEx((BYTE*)(moduleBase + 0x637e9), (BYTE*)"\xFF\x0E",2, hProcess);
            }
        }
        if (GetAsyncKeyState(VK_NUMPAD3) & 1)
        {
            bRecoil = !bRecoil;
            if (bRecoil)
            {
                mem::NopEx((BYTE*)(moduleBase + 0x63786), 10, hProcess);
            }
            else
            {
                mem::PatchEx((BYTE*)(moduleBase + 0x63786), (BYTE*)"\x50\x8d\x4c\x24\x1c\x51\x8b\xce\xff\xd2", 10, hProcess);
            }
        }
        if (GetAsyncKeyState(VK_INSERT) & 1)
        {
            return 0;
        }
        // freezes
        if (bHealth)
        {
            mem::PatchEx((BYTE*)healthAddr, (BYTE*)&newValue, sizeof(newValue), hProcess);
        }
        Sleep(10);
    }
    std::cout << "Process not found, press enter to exit\n";
    std::cin.get();

}

