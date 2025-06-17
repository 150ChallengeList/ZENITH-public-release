#include "Osu.hpp"
#include <Globals.hpp>
#include <Memory/Memory.hpp>
#include <shlobj.h>

Memory memory;

bool Osu::getGameFolder() {
	char path[MAX_PATH];
	if (!SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, path)))
		return false;
	std::filesystem::path osuFolder(path);
	osuFolder /= "osu!";

	if (!std::filesystem::exists(osuFolder))
		return false;

	this->gameFolder = osuFolder;
	return true;
}

bool Osu::init(void* state) {

	if (!this->getGameFolder())
		return false;

	if (!memory.attachToProcess("osu!.exe"))
		return false;

	this->windowHandle = memory.windowHandle;

	// mov eax,[03326930]
	DWORD64 timeUsage = memory.signatureScan("a1 ? ? ? ? a3 ? ? ? ? 83 C4 38 5e 5f", 1);
	if (!memory.readMem<int32_t*>(timeUsage, &this->timePtr, 4)) {
		printf("[osu] Failed to initialize 'this->timePtr'\n");
		return false;
	}
	//this->timePtr += 0x32C;

	// mov eax,[03326964]
	DWORD64 menuStateUsage = memory.signatureScan("A1 ? ? ? ? A3 ? ? ? ? 83 3D ? ? ? ? ? ? ? ? ? ? ? B9 ? ? ? ? E8", 6);
	if (!memory.readMem<int32_t*>(menuStateUsage, &this->statePtr, 4)) {
		printf("[osu] Failed to initialize 'this->statePtr'\n");
		return false;
	}

	DWORD64 currentModsUsage = memory.signatureScan("81 0d ? ? ? ? 00 08 00 00 c7", 2);
	if (!memory.readMem<int32_t*>(currentModsUsage, &this->menuModsPtr, 4)) {
		printf("[osu] Failed to initialize 'this->menuModsPtr'\n");
		return false;
	}

	DWORD64 beatmapUsage = memory.signatureScan("F8 01 74 04 83 65", -0xc);
	// Pointer to current beat map address
	if (!memory.readMem<int32_t*>(beatmapUsage, &this->beatmapBase, 4)) {
		printf("[osu] Failed to initialize 'this->beatmapBase'\n");
		return false;
	}

	//DWORD64 isRetryingUsage = memory.signatureScan("8B CE FF 15 ? ? ? ? C6 05 ? ? ? ? ? 8D 65 F4", 0xB);
	//if (!memory.readMem<int32_t*>(isRetryingUsage, &this->isRetryingPtr, 4)) {
	//	printf("[osu] Failed to initialize 'this->isRetryingPtr'\n");
	//	return false;
	//} // idk why it only reads 3 bytes but this is retarded (should work if it actually read the right 4 bytes and not fucking 3 bytes)

	DWORD64 rightClickUsage = memory.signatureScan("FF FF FF FF 03 00 00 00 ? 00 00 00 04 00 00 00 FF FF FF FF", 8);
	if (!memory.readMem<int32_t*>(rightClickUsage, &this->curRightKey, 1)) {
		printf("[osu] Failed to initialize 'this->curRightKey'\n");
		return false;
	}

	DWORD64 leftClickUsage = memory.signatureScan("FF FF FF FF 02 00 00 00 ? 00 00 00 03 00 00 00 FF", 8);
	if (!memory.readMem<int32_t*>(leftClickUsage, &this->curLeftKey, 1)) {
		printf("[osu] Failed to initialize 'this->curLeftKey'\n");
		return false;
	}

	//DWORD64 failedUsage = memory.signatureScan("8B 15 F0 6A 11 03 89 90 3C 01 00 00 80 3D F8 69 11 03 00 74 57 80 3D", 0xE);
	//if (!memory.readMem<int32_t*>(failedUsage, &this->failedPtr, 4)) {
	//	printf("[osu] Failed to initialize 'this->failedPtr'\n");
	//	return false;
	//}

	return true;
};

void Osu::update(void* state) {

	Globals* globals = reinterpret_cast<Globals*>(state);

	memory.readMem<int32_t*>(this->timePtr, &this->curTime, 4);
	memory.readMem<int32_t*>(this->statePtr, &this->curState, 4);
	memory.readMem<int32_t*>(this->menuModsPtr, &this->curModState, 4);
	memory.readMem<int32_t*>(this->isRetryingPtr, &this->isRetrying, 1);
	memory.readMem<int32_t*>(this->failedPtr, &this->isDead, 1);

	// mov esi, eax
	// cmp dword ptr
	memory.readMem<int32_t*>(this->beatmapBase, &this->curBeatmap, 4); // Read comment / address to current beatmap, loop that since position changes if beatmap changes
	// beatmap name + 0x90 or + 0x310 (not sure)

	this->curSetId = this->curBeatmap + 0xcc;
	this->curMapId = this->curBeatmap + 0xc8;

	memory.readMem<int32_t*>(this->curMapId, &this->mapId, 4);
	memory.readMem<int32_t*>(this->curSetId, &this->setId, 4);

	//wprintf(L"%ls\n", this->fileName);
};
void Osu::tick(void* state) {
	GetWindowRect(this->windowHandle, &this->windowRect);
}


glm::vec2 Osu::convertAbsToOsu(glm::vec2 absPos) {
	return {
		absPos.x - this->windowRect.left,
		absPos.y - this->windowRect.top
	};
}
glm::vec2 Osu::convertOsuToAbs(glm::vec2 osuPos) {
	return {
		osuPos.x + this->windowRect.left,
		osuPos.y + this->windowRect.top
	};
}