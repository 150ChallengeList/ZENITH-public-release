#ifndef AIMASSIST_HPP
#define AIMASSIST_HPP

#include <glm.hpp>
#include <imgui.h>
#include <Windows.h>
#include <Cfg/Cfg.hpp>
#include <unordered_set>
#include <ClassManager/ClassManager.hpp>
#include <BeatmapParser/BeatmapParser.hpp>

enum ALGORITHM : uint8_t {
    ZENITH,
    ZENITHV2
};

struct AimAssistConfig {
    
    // Wich algorithm is selected
    ALGORITHM algorithm = ALGORITHM::ZENITHV2;

    // General settings
    bool enableAssist = false;
    float hitObjectRadius = 35.0f;       // Base hit object radius
    float fov = 120.0f;                  // Field of view for assist
    float power = 1.0f;                  // Base power/strength of assist

    bool waitForClick = false;           // Go back to real pos after the user clicked the circle
    bool legitMode = false;              // Uses less power to be more legit
    bool alwaysOnSync = true;            // Always does the needsCorrection
    bool reverse = false;

    struct {
        // Movement and smoothing
        float alignmentBonus = 1.0f;        // Bonus when moving toward target

        // Humanization
        bool enableHumanization = false;    // Enable subtle randomness
        float noiseFrequencyX = 0.02f;      // X noise frequency
        float noiseFrequencyY = 0.02f;      // Y noise frequency
        float noiseStrength = 0.2f;         // Base noise strength
        float maxNoiseAmount = 1.2f;        // Maximum noise offset
    } Zenith;

    struct {
        // Movement and smoothing
        bool enableBonus = false;
        bool enableAngleCheck = false;
        float alignmentBonus = 1.0f;        // Bonus when moving toward target
        float maxAngle = 365.0f;

        // Humanization
        bool enableHumanization = false;    // Enable subtle randomness
        float noiseFrequencyX = 0.02f;      // X noise frequency
        float noiseFrequencyY = 0.02f;      // Y noise frequency
        float noiseStrength = 0.2f;         // Base noise strength
        float maxNoiseAmount = 1.2f;        // Maximum noise offset

        //std::vector<struct BezierCurve> easingCurves;
        //std::unordered_set<ImVec2> easingPoints;

    } ZenithV2;

    // Default constructor
    AimAssistConfig() = default;
};

enum OsuMods {
    MOD_NF = 1 << 0,    // No Fail
    MOD_EZ = 1 << 1,    // Easy
    MOD_TD = 1 << 2,    // Touch Device
    MOD_HD = 1 << 3,    // Hidden
    MOD_HR = 1 << 4,    // Hard Rock
    MOD_SD = 1 << 5,    // Sudden Death
    MOD_DT = 1 << 6,    // Double Time
    MOD_RX = 1 << 7,    // Relax
    MOD_HT = 1 << 8,    // Half Time
    MOD_NC = 1 << 9,    // Nightcore (includes DT)
    MOD_FL = 1 << 10,   // Flashlight
    MOD_AT = 1 << 11,   // Autoplay
    MOD_SO = 1 << 12,   // Spun Out
    MOD_AP = 1 << 13,   // Auto Pilot (Relax2)
    MOD_PF = 1 << 14,   // Perfect (includes SD)
    MOD_4K = 1 << 15,   // 4 Keys (example of key mod)
    MOD_5K = 1 << 16,   // 5 Keys
    MOD_6K = 1 << 17,   // 6 Keys
    MOD_7K = 1 << 18,   // 7 Keys
    MOD_8K = 1 << 19,   // 8 Keys
    MOD_FI = 1 << 20,   // Fade In
    MOD_RL = 1 << 21,   // Random
    MOD_CN = 1 << 22,   // Cinema
    MOD_TP = 1 << 23,   // Target Practice
    MOD_9K = 1 << 24,   // 9 Keys
    MOD_CO = 1 << 25,   // Coop
    MOD_1K = 1 << 26,   // 1 Key
    MOD_3K = 1 << 27,   // 3 Keys
    MOD_2K = 1 << 28,   // 2 Keys
    // You can extend this for future or custom mods
};

struct AimAssistObj {
    int32_t x;
    int32_t y;

    int32_t startTime;
    int32_t hitTime;
    int32_t endTime;
};

class AimAssist : public BaseClass {
public:

    AimAssistConfig config;

    HHOOK mouseHook;

    int currentObjIndex = 0;
    AimAssistObj* currentObj = nullptr;
    AimAssistObj* nextObj = nullptr;

    int32_t extendTime = 100;

    std::vector<AimAssistObj> assistedObjects;
    int32_t curTime;
    int32_t oldTime;

    glm::vec2 currentMousePos;
    glm::vec2 lastAlgorithmPos;
    glm::vec2 newMousePos;
    glm::vec2 lastMousePos;

    int8_t isSlider : 1;
    int8_t isStream : 1;
    int8_t isHr : 1;
    int8_t isPlaying : 1;
    int8_t isSamePos : 1;

	bool init(void* state) override;
	void deInit(void* state) override;
    void renderMenu(void* state) override;

    void saveConfig(void* state, nlohmann::json* cfg) override;
    void loadConfig(void* state, const nlohmann::json& cfg) override;

    glm::vec2 Zenith(glm::vec2 pos);
    glm::vec2 ZenithV2(glm::vec2 pos);

    void update(void* state) override;

};

#endif AIMASSIST_HPP