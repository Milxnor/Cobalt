#include <Windows.h>
#include <iostream>
#include <detours.h>
#include "curlhook.h"
#include "exithook.h"

#define DetoursEasy(address, hook) \
	DetourTransactionBegin(); \
    DetourUpdateThread(GetCurrentThread()); \
    DetourAttach(reinterpret_cast<void**>(&address), hook); \
    DetourTransactionCommit();

auto FindPushWidget()
{
    return sigscan("48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC 30 48 8B E9 49 8B D9 48 8D 0D ? ? ? ? 49 8B F8 48 8B F2 E8 ? ? ? ? 4C 8B CF 48 89 5C 24 ? 4C 8B C6 48 8B D5 48 8B 48 78");
}

bool InitializeCurlHook()
{
    auto CurlEasySetOptAddr = sigscan("89 54 24 10 4C 89 44 24 18 4C 89 4C 24 20 48 83 EC 28 48 85 C9 75 08 8D 41 2B 48 83 C4 28 C3 4C");

    if (!CurlEasySetOptAddr)
    {
        CurlEasySetOptAddr = sigscan("89 54 24 10 4C 89 44 24 18 4C 89 4C 24 20 48 83 EC 28 48 85 C9 75 08 8D 41 2B 48 83 C4 28 C3 4C");
    }

    if (!CurlEasySetOptAddr)
    {
        std::cout << "Failed to find CurlEasySetOptAddr!\n";
        return false;
    }

    curl_easy_setopt_original = decltype(curl_easy_setopt_original)(CurlEasySetOptAddr);

    if (FindPushWidget())
    {
        DetoursEasy(curl_easy_setopt_original, Hijacked_curl_easy_setopt);
    }
    else
    {
        Memcury::VEHHook::AddHook(curl_easy_setopt_original, Hijacked_curl_easy_setopt); // TODO find a better way to "bypass" UAC.
    }

    return true;
}

void InitializeExitHook()
{
    if (!FindPushWidget())
        return;

    auto UnsafeEnvironmentPopupAddr = sigscan("4C 8B DC 55 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 49 89 73 F0 49 89 7B E8 48 8B F9 4D 89 63 E0 4D 8B E0 4D 89 6B D8");

    if (!UnsafeEnvironmentPopupAddr)
    {
        UnsafeEnvironmentPopupAddr = sigscan("4C 8B DC 55 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ?");
    }

    auto RequestExitWithStatusAddr = sigscan("48 89 5C 24 ? 57 48 83 EC 40 41 B9 ? ? ? ? 0F B6 F9 44 38 0D ? ? ? ? 0F B6 DA 72 24 89 5C 24 30 48 8D 05 ? ? ? ? 89 7C 24 28 4C 8D 05 ? ? ? ? 33 D2 48 89 44 24 ? 33 C9 E8 ? ? ? ?");

    if (!RequestExitWithStatusAddr)
    {
        RequestExitWithStatusAddr = sigscan("48 8B C4 48 89 58 18 88 50 10 88 48 08 57 48 83 EC 30");
    }

    DetoursEasy(UnsafeEnvironmentPopupAddr, UnsafeEnvironmentPopupHook);
    DetoursEasy(RequestExitWithStatusAddr, RequestExitWithStatusHook);
}

DWORD WINAPI Main(LPVOID)
{
    AllocConsole();

    FILE* fptr;
    freopen_s(&fptr, "CONOUT$", "w+", stdout);

    bool curlResult = InitializeCurlHook();
    InitializeExitHook();

    bool result = curlResult;

    if (result)
    {
        std::cout << "Cobalt v0.1 initialized sucessfully.\n";
    }
    else
    {
        MessageBoxA(0, "Failed to initialize!", "Cobalt", MB_ICONERROR);
    }

    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(0, 0, Main, 0, 0, 0);
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

