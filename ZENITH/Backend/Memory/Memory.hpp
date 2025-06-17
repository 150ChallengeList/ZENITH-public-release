#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <Windows.h>
#include <cstdint>
#include <vector>

class Memory {
public:

	std::vector<std::pair<DWORD64, DWORD64>> regions;
	HANDLE processHandle;

public:
	
	uint32_t processId;
	HWND windowHandle;

	uint32_t getProcessIdByName(const char* processName);

	DWORD64 signatureScan(const char* idaPattern, DWORD64 offset);

	bool attachToProcess(const char* processName);
	bool detachFromProcess();

	template <typename T>
	bool readMem(DWORD64 address, T outBuf, DWORD64 sizeOverride = sizeof(T)) {
		size_t readBytes = 0;
		if (!ReadProcessMemory(this->processHandle, reinterpret_cast<LPCVOID>(address), reinterpret_cast<LPVOID>(outBuf), sizeOverride, &readBytes)) {
			DWORD err = GetLastError();
			return false; // Read failed
		}
		return readBytes == sizeOverride;
	}

} extern memory;

#endif MEMORY_HPP