/*
	by Fyyre, 2020 - 2022

	other credits:
	hooker framework, rokups: https://github.com/rokups/hooker
	HDE32, Vyacheslav Patkov
	zeffy, prxdll_templates

	love to: hFiref0x, X, E an K.

	Released in the event some find it useful
*/

#include "Inc.h"
#include "hooker.hpp"
#include "sup.h"

long volatile g_initialized = false;

static FARPROC GetSystemTimeAsFileTimePtr = nullptr;
static decltype(&GetSystemTimeAsFileTime) GetSystemTimeAsFileTimeOriginal = nullptr;

typedef struct _CLIENT_ID
{
	HANDLE UniqueProcess;
	HANDLE UniqueThread;
} CLIENT_ID, * PCLIENT_ID;

#include <pshpack4.h>

typedef struct _KKSYSTEM_TIME
{
	ULONG LowPart;
	LONG High1Time;
	LONG High2Time;
} KKSYSTEM_TIME, * PKKSYSTEM_TIME;

#include <poppack.h>

typedef struct _KKUSER_SHARED_DATA
{
	ULONG TickCountLowDeprecated;
	ULONG TickCountMultiplier;

	volatile KKSYSTEM_TIME InterruptTime;
	volatile KKSYSTEM_TIME SystemTime;
	volatile KKSYSTEM_TIME TimeZoneBias;
} KKUSER_SHARED_DATA, * PKKUSER_SHARED_DATA;

#define MM_SHARED_USER_DATA_VA 0x7FFE0000	// hurray for static addresses!
#define UUSER_SHARED_DATA ((_KKUSER_SHARED_DATA * const)MM_SHARED_USER_DATA_VA)

#ifndef UserSharedData
#define UserSharedData UUSER_SHARED_DATA
#endif

extern "C"
{
	FARPROC OrignalD3DPERF_BeginEvent;
	FARPROC OrignalD3DPERF_EndEvent;
	FARPROC OrignalD3DPERF_GetStatus;
	FARPROC OrignalD3DPERF_QueryRepeatFrame;
	FARPROC OrignalD3DPERF_SetMarker;
	FARPROC OrignalD3DPERF_SetOptions;
	FARPROC OrignalD3DPERF_SetRegion;
	FARPROC OrignalDebugSetLevel;
	FARPROC OrignalDebugSetMute;
	FARPROC OrignalDirect3D9EnableMaximizedWindowedModeShim;
	FARPROC OrignalDirect3DCreate9;
	FARPROC OrignalDirect3DCreate9Ex;
	FARPROC OrignalDirect3DShaderValidatorCreate9;
	FARPROC OrignalPSGPError;
	FARPROC OrignalPSGPSampleTexture;
}

VOID
WINAPI
GetSystemTimeAsFileTime_h(
	LPFILETIME lpSystemTimeAsFileTime
)
{
	HMODULE hMod = NULL;
	LARGE_INTEGER SystemTime{};
	CLIENT_ID ClientId{};

	do {
		SystemTime.HighPart = UserSharedData->SystemTime.High1Time;
		SystemTime.LowPart = UserSharedData->SystemTime.LowPart;
	} while (SystemTime.HighPart != UserSharedData->SystemTime.High2Time);

	lpSystemTimeAsFileTime->dwLowDateTime = SystemTime.LowPart;
	lpSystemTimeAsFileTime->dwHighDateTime = SystemTime.HighPart;

	if ( _InterlockedCompareExchange( &g_initialized, 1, 0) == 1 )
	{
		/* if ( insert here if binary is packed with WL or whatever,
				most easy this to do is check address for non-vm opcode,
				etc.. 
		) */
		{
			_InterlockedIncrement(&g_initialized);
			
			// add code here, as this is called often
			// it's an ideal location for installing hooks
			// inside of Winlicensed, etc binaries.

			 MessageBoxW(NULL, L"Hi", L"Hooked", MB_OK);

			// unhook this, so we don't make games laggy, etc.
			hooker::unhook(GetSystemTimeAsFileTimePtr, GetSystemTimeAsFileTimeOriginal);
		}
	}
}

//
// clone of d3d9 module and related support functions -- 26.10.22
//
HMODULE clone_d3d9()
{
	WCHAR szBuffer[MAX_PATH + 1];

	// let's do this, hFiref0x style <3
	_strcpy(szBuffer, TEXT("\\\\?\\globalroot\\systemroot\\sys"));
	_strcat(szBuffer, TEXT("tem32"));
	_strcat(szBuffer, TEXT("\\d3d9.dll"));

	// Replace with LdrLoadDll... *shrug*
	HMODULE d3d9_handle = LoadLibraryExW(szBuffer, NULL, 0);

	// resolve imports from the original d3d9.dll -- do without calling GetProcAddress
	if (d3d9_handle)
	{
		OrignalD3DPERF_BeginEvent = (FARPROC)get_func_by_name(d3d9_handle, "D3DPERF_BeginEvent");
		OrignalD3DPERF_EndEvent = (FARPROC)get_func_by_name(d3d9_handle, "D3DPERF_EndEvent");
		OrignalD3DPERF_GetStatus = (FARPROC)get_func_by_name(d3d9_handle, "D3DPERF_GetStatus");
		OrignalD3DPERF_QueryRepeatFrame = (FARPROC)get_func_by_name(d3d9_handle, "D3DPERF_QueryRepeatFrame");
		OrignalD3DPERF_SetMarker = (FARPROC)get_func_by_name(d3d9_handle, "D3DPERF_SetMarker");
		OrignalD3DPERF_SetOptions = (FARPROC)get_func_by_name(d3d9_handle, "D3DPERF_SetOptions");
		OrignalD3DPERF_SetRegion = (FARPROC)get_func_by_name(d3d9_handle, "D3DPERF_SetRegion");
		OrignalDebugSetLevel = (FARPROC)get_func_by_name(d3d9_handle, "DebugSetLevel");
		OrignalDebugSetMute = (FARPROC)get_func_by_name(d3d9_handle, "DebugSetMute");
		OrignalDirect3D9EnableMaximizedWindowedModeShim = (FARPROC)get_func_by_name(d3d9_handle, "Direct3D9EnableMaximizedWindowedModeShim");
		OrignalDirect3DCreate9 = (FARPROC)get_func_by_name(d3d9_handle, "Direct3DCreate9");
		OrignalDirect3DCreate9Ex = (FARPROC)get_func_by_name(d3d9_handle, "Direct3DCreate9Ex");
		OrignalDirect3DShaderValidatorCreate9 = (FARPROC)get_func_by_name(d3d9_handle, "Direct3DShaderValidatorCreate9");
		OrignalPSGPError = (FARPROC)get_func_by_name(d3d9_handle, "PSGPError");
		OrignalPSGPSampleTexture = (FARPROC)get_func_by_name(d3d9_handle, "PSGPSampleTexture");
	}
	return d3d9_handle;
}

//
// end of d3d9 clone functions, etc.
//

// my template hook for games etc that are protected with
// WL, VMP, etc.. works with Windows 11 22H2
BOOL
APIENTRY
DllMain(
	const HINSTANCE instance,
	const DWORD reason,
	const PVOID reserved)
{
	switch (reason) {

	case DLL_PROCESS_ATTACH:
	{
		clone_d3d9();
		GetSystemTimeAsFileTimePtr = (FARPROC)get_func_by_name(get_module_by_name(L"kernel32.dll"), "GetSystemTimeAsFileTime");
		GetSystemTimeAsFileTimeOriginal = hooker::hook<decltype(&GetSystemTimeAsFileTime)>(GetSystemTimeAsFileTimePtr, &GetSystemTimeAsFileTime_h, HOOKER_HOOK_FAT);
		DisableThreadLibraryCalls(instance);
		return TRUE;
	}
	break;

	case DLL_PROCESS_DETACH:
		break;
	}

	return TRUE;
}
