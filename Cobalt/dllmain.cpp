
#include <Windows.h>
#include <iostream>
#include <detours.h>
#include "curlhook.h"
#include "exithook.h"
#include <MinHook/MinHook.h>

#define DetoursEasy(address, hook) \
	DetourTransactionBegin(); \
    DetourUpdateThread(GetCurrentThread()); \
    DetourAttach(reinterpret_cast<void**>(&address), hook); \
    DetourTransactionCommit();

void returnNone() { return; }

auto FindPushWidget()
{
    // OnlinePresence call
    auto pattern = sigscan("48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC 30 48 8B E9 49 8B D9 48 8D 0D ? ? ? ? 49 8B F8 48 8B F2 E8 ? ? ? ? 4C 8B CF 48 89 5C 24 ? 4C 8B C6 48 8B D5 48 8B 48 78");

    if (!pattern)
        pattern = sigscan("48 8B C4 4C 89 40 18 48 89 50 10 48 89 48 08 55 53 56 57 41 54 41 55 41 56 41 57 48 8D 68 B8 48 81 EC ? ? ? ? 65 48 8B 04 25"); // 26.00+ or sum

    if (!pattern)
        pattern = sigscan("48 8B C4 48 89 58 10 48 89 70 18 48 89 78 20 55 41 56 41 57 48 8D 68 A1 48 81 EC ? ? ? ? 65 48 8B 04 25 ? ? ? ? 48 8B F9 B9 ? ? ? ? 49"); // 28.00+ or sum

    return pattern;
}

void Hook(void* Target, void* Detour)
{
#ifdef USE_MINHOOK
    MH_CreateHook(Target, Detour, nullptr);
    MH_EnableHook(Target);
#else
    Memcury::VEHHook::AddHook(Target, Detour);
#endif
}

bool FixMemoryLeak() // 8.51
{
    auto memoryleak = sigscan("4C 8B DC 55 57 41 56 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 48 8B 01 41 B6");

    if (!memoryleak)
    {
        return false;
    }

    Hook((void*)memoryleak, returnNone);
    return true;
}

void InitializeEOSCurlHook()
{
}

bool InitializeCurlHook()
{
    auto CurlEasySetOptAddr = sigscan("89 54 24 10 4C 89 44 24 18 4C 89 4C 24 20 48 83 EC 28 48 85 C9 75 08 8D 41 2B 48 83 C4 28 C3 4C");

    if (!CurlEasySetOptAddr)
    {
        std::cout << "Fallback!\n";

        while (!CurlEasySetOptAddr)
        {
            CurlEasySetOptAddr = sigscan("89 54 24 10 4C 89 44 24 18 4C 89 4C 24 20 48 83 EC 28 48 85 C9 75 08 8D 41 2B 48 83 C4 28 C3 4C");
            Sleep(200);
        }
    }

    if (!CurlEasySetOptAddr) // impossibel ol
    {
        std::cout << "Failed to find CurlEasySetOptAddr!\n";
        return false;
    }

    auto CurlSetOptAddr = sigscan("48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 48 83 EC 30 33 ED 49 8B F0 48 8B D9");

    if (!CurlSetOptAddr)
    {
        CurlSetOptAddr = sigscan("48 89 5C 24 08 48 89 6C 24 10 56 57 41 56 48 83 EC 50 33 ED 49 8B F0 8B DA 48 8B F9");

        if (!CurlSetOptAddr)
            CurlSetOptAddr = sigscan("48 89 5C 24 ? 55 56 57 41 56 41 57 48 83 EC 50 33 DB 49 8B F0 48 8B F9 8B EB 81 FA"); // tested 28.00 // fixed crash
    }

    if (!CurlSetOptAddr)
    {
        std::cout << "Failed to find CurlSetOptAddr! But we will go ahead..\n";
    }

    CurlEasySetOpt = decltype(CurlEasySetOpt)(CurlEasySetOptAddr);
    CurlSetOpt = decltype(CurlSetOpt)(CurlSetOptAddr);

    if (FindPushWidget())
    {
        DetoursEasy(CurlEasySetOpt, CurlEasySetOptDetour);
    }
    else
    {
        // TODO find a better way to "bypass" UAC (aka switch off VEH hooks)

        Hook(CurlEasySetOpt, CurlEasySetOptDetour);
    }

    return true;
}

void InitializeExitHook()
{
    if (!FindPushWidget())
    {
        std::cout << "Failed to find PushWidget (This may be fine)!\n";

        /*
        auto RequestExitWithStatusAddr = sigscan("40 53 48 83 EC 40 80 3D ? ? ? ? ? 0F B6 D9 72 3A 48 8B 05"); // S9

        std::cout << "RequestExitWithStatusAddr: " << RequestExitWithStatusAddr << '\n';

        RequestExitWithStatus = decltype(RequestExitWithStatus)(RequestExitWithStatusAddr);
        Memcury::VEHHook::AddHook(RequestExitWithStatus, RequestExitWithStatusHook);

        auto UnsafeEnvironmentPopupAddr = sigscan("4C 8B DC 55 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 49 89 73 E8 33 F6 49 89 7B E0 0F B6 FA"); // S9

        std::cout << "UnsafeEnvironmentPopupAddr: " << UnsafeEnvironmentPopupAddr << '\n';

        UnsafeEnvironmentPopup = decltype(UnsafeEnvironmentPopup)(UnsafeEnvironmentPopupAddr);
        Memcury::VEHHook::AddHook(UnsafeEnvironmentPopup, UnsafeEnvironmentPopupHook);

        auto RequestExitAddrs = Memcury::Scanner::FindPatterns("40 53 48 83 EC 30 80 3D ? ? ? ? ? 0F B6 D9 72 33 48 8B 05 ? ? ? ? 4C 8D 44 24 ? 48 89 44 24 ? 41 B9 ? ? ? ? 0F"); // S9

        std::cout << "RequestExitAddrs: " << RequestExitAddrs.size() << '\n';

        for (auto RequestExitAddr : RequestExitAddrs)
        {
            RequestExit = decltype(RequestExit)(Memcury::Scanner(RequestExitAddr).Get());
            Memcury::VEHHook::AddHook(RequestExit, RequestExitHook);
        }
        */


        return;
    }

    auto UnsafeEnvironmentPopupAddr = sigscan("4C 8B DC 55 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 49 89 73 F0 49 89 7B E8 48 8B F9 4D 89 63 E0 4D 8B E0 4D 89 6B D8");

    if (!UnsafeEnvironmentPopupAddr)
    {
        UnsafeEnvironmentPopupAddr = sigscan("4C 8B DC 55 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ?");

        if (!UnsafeEnvironmentPopupAddr)
            UnsafeEnvironmentPopupAddr = sigscan("48 89 5C 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 80 B9 ? ? ? ? ? 48 8B DA 48 8B F1");
    }

    if (!UnsafeEnvironmentPopupAddr)
    {
        std::cout << "Failed to find UnsafeEnvironmentPopupAddr (This may be fine)!\n";
    }

    // probably unnneeeded
    auto RequestExitWithStatusAddr = sigscan("48 89 5C 24 ? 57 48 83 EC 40 41 B9 ? ? ? ? 0F B6 F9 44 38 0D ? ? ? ? 0F B6 DA 72 24 89 5C 24 30 48 8D 05 ? ? ? ? 89 7C 24 28 4C 8D 05 ? ? ? ? 33 D2 48 89 44 24 ? 33 C9 E8 ? ? ? ?");

    if (!RequestExitWithStatusAddr)
    {
        RequestExitWithStatusAddr = sigscan("48 8B C4 48 89 58 18 88 50 10 88 48 08 57 48 83 EC 30"); // ion know whta version this for

        if (!RequestExitWithStatusAddr)
            RequestExitWithStatusAddr = sigscan("4C 8B DC 49 89 5B 08 49 89 6B 10 49 89 73 18 49 89 7B 20 41 56 48 83 EC 30 80 3D ? ? ? ? ? 49 8B"); // dk how often this change
    }

    if (!RequestExitWithStatusAddr)
    {
        std::cout << "Failed to find RequestExitWithStatusAddr (This may be fine)!\n";
    }

    DetoursEasy(UnsafeEnvironmentPopupAddr, UnsafeEnvironmentPopupHook);
    DetoursEasy(RequestExitWithStatusAddr, RequestExitWithStatusHook);
}

DWORD WINAPI Main(LPVOID)
{
#ifdef SHOW_WINDOWS_CONSOLE
    AllocConsole();

    FILE* fptr;
    freopen_s(&fptr, "CONOUT$", "w+", stdout);
#endif SHOW_WINDOWS_CONSOLE

#ifndef URL_HOST // todo staticassert?
    std::cout << "\n\n\n!!!!!!! URL_HOST IS NOT DEFINED !!!!!!!\n\n\n\n";
#else
    std::cout << "Redirecting to " << URL_PROTOCOL << "://" << URL_HOST << ":" << URL_PORT << '\n';
#endif

    std::cout << "Initializing Cobalt (made by Milxnor#3531).\n";

    std::cout << "Credits:\n\n";

    std::cout << "Memcury - https://github.com/kem0x/Memcury\n";
    std::cout << "Neonite++ for most of the signatures and curl hook - https://github.com/PeQuLeaks/NeonitePP-Fixed/tree/1.4\n\n";

#ifdef USE_MINHOOK
    MH_Initialize();
#else
    Memcury::VEHHook::Init();
#endif

    bool curlResult = InitializeCurlHook();
    InitializeEOSCurlHook();
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

#if 0
    Sleep(3000);

    if (!FixMemoryLeak())
    {
        std::cout << "Failed to apply memory leak fix (this may be fine)!\n";
    }
    else
    {
        std::cout << "Applied memory leak fix!\n";
    }
#endif

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

