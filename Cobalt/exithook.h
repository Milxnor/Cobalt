#pragma once

#include <iostream>

void (*RequestExitWithStatus)(bool Force, unsigned char Code);
void RequestExitWithStatusHook(bool Force, unsigned char Code) // 3 args newer version
{
	// printf("[VEH] RequestExitWithStatus Call Forced: %i ReturnCode: %u\n", Force, Code);
}

void (*RequestExit)(int Code);
void RequestExitHook(int Code)
{
	std::cout << "REQUEST EXIT CODE: " << Code << '\n';
}

void (*UnsafeEnvironmentPopup)(wchar_t** unknown1, unsigned __int8 _case, __int64 unknown2, char unknown3);

void UnsafeEnvironmentPopupHook(wchar_t** unknown1, unsigned __int8 _case, __int64 unknown2, char unknown3)
{
	// printf("[VEH] <UnsafeEnvironmentPopup Call with Case: %i\n", _case);
}