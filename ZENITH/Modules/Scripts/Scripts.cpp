#include <algorithm>
#include "Scripts.hpp"
#include "glm/glm.hpp"
#include <Globals.hpp>
#include <xmmintrin.h> // SIMD

Globals* globalPtr = nullptr;

void Scripts::renderMenu(void* state) {

}

bool Scripts::init(void* state) {
    globalPtr = reinterpret_cast<Globals*>(state);
    // Open base and math libraries
    this->lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::coroutine);

    // Vector2 (glm::vec2)
    this->lua.new_usertype<glm::vec2>("Vector2",
        sol::constructors<glm::vec2(), glm::vec2(float, float)>(),
        "x", &glm::vec2::x,
        "y", &glm::vec2::y
    );

    // Math helper functions
    this->lua.set_function("getDistance", [](const glm::vec2& a, const glm::vec2& b) -> double {
        return std::sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
        });
    this->lua.set_function("normalize", [](glm::vec2 a) -> glm::vec2 { // pass by value (safe)
        float length = std::sqrt(a.x * a.x + a.y * a.y);
        if (length == 0.0f)
            return glm::vec2(0.0f, 0.0f);
        return a / length;
        });
    this->lua.set_function("dot", [](const glm::vec2& a, const glm::vec2& b) {
        return a.x * b.x + a.y * b.y;
        });
    this->lua.set_function("min", [](double x, double y) {
        return x < y ? x : y;
        });
    this->lua.set_function("max", [](double x, double y) {
        return x > y ? x : y;
        });
    this->lua.set_function("clamp", [](double val, double min, double max) {
        return val > max ? max : (val < min ? min : val);
        });
    this->lua.set_function("sleep", [](double time) {
        return Sleep(time);
        });

    // Osu data
    this->lua.new_usertype<Osu>("Osu", sol::no_constructor,
        "curTime", &Osu::curTime,
        "curState", &Osu::curState,
        "curModState", &Osu::curModState,
        "curLeftKey", &Osu::curLeftKey,
        "curRightKey", &Osu::curRightKey,
        "isRetrying", &Osu::isRetrying,
        "isDead", &Osu::isDead
    );


    // Beatmap main struct
    this->lua.new_usertype<Beatmap>("Beatmap", sol::no_constructor,
        "timingPoints", &Beatmap::timingPoints,
        "hitObjects", &Beatmap::hitObjects,
        "difficulty", &Beatmap::difficulty
    );

    // Beatmap Difficulty
    this->lua.new_usertype<Beatmap::Difficulty>("Difficulty",
        "approachRate", &Beatmap::Difficulty::approachRate,
        "circleSize", &Beatmap::Difficulty::circleSize,
        "overallDifficulty", &Beatmap::Difficulty::overallDifficulty,
        "sliderMultiplier", &Beatmap::Difficulty::sliderMultiplier,
        "sliderTickRate", &Beatmap::Difficulty::sliderTickRate
    );

    // HitObject (single)
    this->lua.new_usertype<Beatmap::HitObject>("HitObject",
        "x", &Beatmap::HitObject::x,
        "y", &Beatmap::HitObject::y,
        "timeToHit", &Beatmap::HitObject::timeToHit,
        "isSlider", &Beatmap::HitObject::isSlider,
        "stackIndex", &Beatmap::HitObject::stackIndex
    );

    // HitObjects (collection)
    this->lua.new_usertype<Beatmap::HitObjects>("HitObjects",
        "objects", &Beatmap::HitObjects::objects
    );

    // TimingPoint (single)
    this->lua.new_usertype<Beatmap::TimingPoint>("TimingPoint",
        "beatLength", &Beatmap::TimingPoint::beatLength,
        "effects", &Beatmap::TimingPoint::effects,
        "meter", &Beatmap::TimingPoint::meter,
        "sampleIndex", &Beatmap::TimingPoint::sampleIndex,
        "sampleSet", &Beatmap::TimingPoint::sampleSet,
        "time", &Beatmap::TimingPoint::time,
        "uninherited", &Beatmap::TimingPoint::uninherited,
        "volume", &Beatmap::TimingPoint::volume
    );

    // TimingPoints (collection)
    this->lua.new_usertype<Beatmap::TimingPoints>("TimingPoints",
        "points", &Beatmap::TimingPoints::points
    );

    // init variables
    this->lua["osu"] = globalPtr->osu;

    this->lua["beatmap"] = globalPtr->beatMapParser->currentBeatmap;

    // Check for errors using protected_function_result
    this->stopThread = false;
    this->scriptThread = std::thread([this]() {
        sol::protected_function_result result = this->lua.script_file(
            "C:\\Users\\rushe\\Documents\\GitHub\\ZENITH\\ZENITH\\ZENITH\\Modules\\Scripts\\test.lua",
            sol::script_pass_on_error
        );

        if (!result.valid()) {
            sol::error error = result;
            printf("[Lua error] %s\n", error.what());
        }
        });

    return true;
}

void Scripts::cleanup() {
    if (this->scriptThread.joinable()) {
        this->stopThread = true;
        this->scriptThread.join();
    }
}