#ifndef OSU_HPP
#define OSU_HPP

#include <ClassManager/BaseClass.hpp>
#include <filesystem>
#include <Windows.h>
#include <glm.hpp>

enum OSU_STATE : int32_t {
	MAINMENU = 0,
	EDIT = 1,
	PLAY = 2,
	EXIT,
	SELECTEDIT,
	SELECTPLAY,
	SELECTDRAWINGS,
	RANK,
	UPDATE,
	BUSY,
	UNKNOWN,
	LOBBY,
	MATCHSETUP,
	SELECTMULTI,
	RANKINGVS,
	ONLINESELECTION,
	OPTIONSOFFSETWIZARD,
	RANKINGTAGCOOP,
	RANKINGTEAM,
	BEATMAPIMPORT,
	PACKAGEUPDATER,
	BENCHMARK,
	TOURNEY,
	CHARTS
};

class Osu : public BaseClass {
private:

	bool getGameFolder();

	int32_t timePtr = 0;
	int32_t statePtr = 0;
	int32_t menuModsPtr = 0;
	int32_t mapPtr = 0;
	int32_t leftClickKeyPtr = 0;
	int32_t rightClickKeyPtr = 0;
	int32_t isRetryingPtr = 0;
	int32_t failedPtr = 0;
	int32_t beatmapBase = 0;
	int32_t curBeatmap = 0;
	int32_t curSetId = 0;
	int32_t curMapId = 0;

public:

	RECT windowRect;
	HWND windowHandle;

	int32_t curTime = 0;
	int32_t curState = 0;
	int32_t curModState = 0;
	int32_t curLeftKey = 0;
	int32_t curRightKey = 0;
	int32_t isRetrying = 0;
	int32_t isDead = 0;
	int32_t mapId = 0;
	int32_t setId = 0;

	bool isSet = false;

	std::filesystem::path gameFolder;

	bool init(void* state) override;
	void update(void* state) override;
	void tick(void* state) override;

	glm::vec2 convertAbsToOsu(glm::vec2 absPos);
	glm::vec2 convertOsuToAbs(glm::vec2 osuPos);

};

#endif OSU_HPP