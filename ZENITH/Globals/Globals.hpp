#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include <WindowManager/WindowManager.hpp>
#include <BeatmapParser/BeatmapParser.hpp>
#include <AimAssist/AimAssist.hpp>
#include <Scripts/Scripts.hpp>
#include <Replay/Replay.hpp>
#include <Osu/Osu.hpp>
#include <Cfg/Cfg.hpp>
#include <Windows.h>
#include <cstdint>

struct Globals {

	bool menuOpen = true;

	Cfg* cfg = nullptr;
	Osu* osu = nullptr;
	Window* window = nullptr;
	Scripts* scripts = nullptr;
	AimAssist* aimAssist = nullptr;
	ReplayBot* replayBot = nullptr;
	BeatmapParser* beatMapParser = nullptr;

} extern globals;

#endif GLOBALS_HPP