#pragma once

void (*RequestExitWithStatus)(bool Force, unsigned char Code);
void RequestExitWithStatusHook(bool Force, unsigned char Code)
{
	// printf("[VEH] RequestExitWithStatus Call Forced: %i ReturnCode: %u\n", Force, Code);
}

void (*UnsafeEnvironmentPopup)(wchar_t** unknown1, unsigned __int8 _case, __int64 unknown2, char unknown3);

void UnsafeEnvironmentPopupHook(wchar_t** unknown1, unsigned __int8 _case, __int64 unknown2, char unknown3)
{
	// printf("[VEH] <UnsafeEnvironmentPopup Call with Case: %i\n", _case);
}