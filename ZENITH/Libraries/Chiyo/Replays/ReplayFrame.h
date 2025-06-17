#pragma once

#include <OsuDataTypes/Osu/Keys/CatchTheBeatKeys.h>
#include <OsuDataTypes/Osu/Keys/OsuKeys.h>
#include <OsuDataTypes/Osu/Keys/OsuManiaKeys.h>
#include <OsuDataTypes/Osu/Keys/TaikoKeys.h>

struct ReplayFrame {
	float X;
	float Y;
	int TimeDiff;
	int Time;
	OsuKeys OsuKeys;
	TaikoKeys TaikoKeys;
	CatchTheBeatKeys CatchTheBeatKeys;
	OsuManiaKeys OsuManiaKeys;
};
