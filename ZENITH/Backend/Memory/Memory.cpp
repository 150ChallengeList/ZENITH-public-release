#include "Memory.hpp"
#include <TlHelp32.h>
#include <iostream>
#include <sstream>
//#include <iomanip>

uint32_t Memory::getProcessIdByName(const char* processName) {

	HANDLE snapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snapShot == INVALID_HANDLE_VALUE) {
		printf("[-] Memory::getProcessIdByName 'snapShot == INVALID_HANDLE_VALUE' %s %d 0x%x\n", __FILE__, __LINE__, GetLastError());
		return 0;
	}

	PROCESSENTRY32 processEntry;
	processEntry.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32First(snapShot, &processEntry)) {
		printf("[-] Memory::getProcessIdByName '!Process32First(snapShot, &processEntry)' %s %d 0x%x\n", __FILE__, __LINE__, GetLastError());
		CloseHandle(snapShot);
		return 0;
	}

	do {
		char converted[MAX_PATH] = {};
		memset(converted, 0x00, MAX_PATH);

		size_t convertedAmount = 0;
		if (wcstombs_s(&convertedAmount, converted, MAX_PATH, processEntry.szExeFile, MAX_PATH))
			continue;

		if (memcmp(converted, processName, convertedAmount) != 0)
			continue;

		CloseHandle(snapShot);

		return processEntry.th32ProcessID;

	} while (Process32Next(snapShot, &processEntry));

	CloseHandle(snapShot);
	return 0;
}

bool Memory::attachToProcess(const char* processName) {

	uint32_t procId = this->getProcessIdByName(processName);
	if (procId == 0) {
		printf("[!] Memory::attachToProcess '%s' procId -> '%x'\n", processName, procId);
		return false;
	}
	this->processId = procId;
	printf("[i] Memory::attachToProcess '%s' procId -> '%x'\n", processName, procId);

	this->processHandle = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, procId);
	if (this->processHandle == INVALID_HANDLE_VALUE || this->processHandle == NULL)
		return false;
	printf("[i] Memory::attachToProcess '%s' processHandle -> '%x'\n", processName, this->processHandle);

	MEMORY_BASIC_INFORMATION mbi;
	DWORD64 address = 0;
	while (VirtualQueryEx(this->processHandle, reinterpret_cast<LPCVOID>(address), &mbi, sizeof(mbi))) {
		if (mbi.State != MEM_COMMIT || (mbi.Protect != PAGE_READWRITE && mbi.Protect != PAGE_EXECUTE_READWRITE)) {
			address += mbi.RegionSize;
			continue;
		}
		this->regions.emplace_back(reinterpret_cast<DWORD64>(mbi.BaseAddress), mbi.RegionSize);
		address += mbi.RegionSize;
	}

	auto findWindowByProcId = [](DWORD processId) -> HWND {
		DWORD64 windowHandle = processId;

		auto enumWindowsProc = [](HWND windowHandle, LPARAM lParam) -> BOOL {
			DWORD targetProcId = *reinterpret_cast<DWORD*>(lParam);

			DWORD windowProcessId;
			GetWindowThreadProcessId(windowHandle, &windowProcessId);
			if (windowProcessId != targetProcId)
				return TRUE;

			*reinterpret_cast<HWND*>(lParam) = windowHandle;
			return FALSE;
		};

		EnumWindows(enumWindowsProc, reinterpret_cast<LPARAM>(&windowHandle));
		return reinterpret_cast<HWND>(windowHandle);
	};

	this->windowHandle = findWindowByProcId(procId);
	printf("[i] Memory::attachToProcess '%s' hwnd -> '%x'\n", processName, this->windowHandle);

	return true;
}

DWORD64 Memory::signatureScan(const char* idaPattern, DWORD64 offset) {

	std::vector<uint8_t> pattern;
	std::istringstream iss(idaPattern);
	std::string token;

	while (iss >> token) {
		if (token == "?")
			pattern.emplace_back(0xCC);
		else 
			pattern.emplace_back(std::stoi(token, nullptr, 16));
	}
	size_t patternSize = pattern.size();

	for (const auto& region : this->regions) {
		
		std::vector<uint8_t> buffer(region.second);

		this->readMem<uint8_t*>(region.first, buffer.data(), region.second);

		for (size_t i = 0; i + patternSize <= region.second; i++) {
		
			bool found = true;
			for (size_t j = 0; j < patternSize; j++) {
				if (pattern[j] == 0xCC || buffer[i + j] == pattern[j])
					continue;
				found = false;
				break;
			}
			if (!found)
				continue;
		
			return region.first + i + offset;
		}
	}
	return 0;
}

bool Memory::detachFromProcess() {
	CloseHandle(this->processHandle);
	return true;
}