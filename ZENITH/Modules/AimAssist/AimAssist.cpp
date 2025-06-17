#include "AimAssist.hpp"
#include "../Globals/Globals.hpp"
#include <array>
#include <cmath>
#include <random>
#include <imgui.h>
#include <LucideDefinitions.hpp>
#include <AnimEngine/CustomWidgets.hpp>
#include "thread"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

std::string serializeConfig(const AimAssistConfig& cfg) {
    std::ostringstream oss;
    //
    //// General Settings
    //oss << "algorithm=" << static_cast<int>(cfg.algorithm) << "\n";
    //oss << "enableAssist=" << cfg.enableAssist << "\n";
    //oss << "hitObjectRadius=" << cfg.hitObjectRadius << "\n";
    //oss << "fov=" << cfg.fov << "\n";
    //oss << "power=" << cfg.power << "\n";
    //oss << "waitForClick=" << cfg.waitForClick << "\n";
    //oss << "legitMode=" << cfg.legitMode << "\n";
    //oss << "alwaysOnSync=" << cfg.alwaysOnSync << "\n";
    //oss << "reverse=" << cfg.reverse << "\n";
    //
    //size_t pointSize = easingPoints.size();
    //
    //int pS = 0;
    //
    //for (const auto& points : easingPoints) {
    //    if (pS == 0)
    //        oss << "points: ((" << "\n";
    //
    //    oss << "easingPoint" + std::to_string(pS) + ".x=" << points.x << "\n";
    //    oss << "easingPoint" + std::to_string(pS) + ".y=" << points.y << "\n";
    //
    //    pS++;
    //}
    //
    //oss << ")) endpoints" << "\n";
    //
    //size_t curveSize = easingCurves.size();
    //
    //int cS = 0;
    //
    //for (const auto& curves : easingCurves) {
    //    if (cS == 0)
    //        oss << "curves: ((" << "\n";
    //
    //    oss << "easingCurves" + std::to_string(cS) + ".startPos.x=" << curves.startPos->x << "\n";
    //    oss << "easingCurves" + std::to_string(cS) + ".startPos.y=" << curves.startPos->y << "\n";
    //    oss << "easingCurves" + std::to_string(cS) + ".midPos.x=" << curves.midPos->x << "\n";
    //    oss << "easingCurves" + std::to_string(cS) + ".midPos.y=" << curves.midPos->y << "\n";
    //    oss << "easingCurves" + std::to_string(cS) + ".endPos.x=" << curves.endPos->x << "\n";
    //    oss << "easingCurves" + std::to_string(cS) + ".endPos.y=" << curves.endPos->y << "\n";
    //
    //    cS++;
    //}
    //
    //oss << ")) endcurves" << "\n";
    //
    //// ZenithV2 settings
    //oss << "ZenithV2.enableBonus=" << cfg.ZenithV2.enableBonus << "\n";
    //oss << "ZenithV2.enableAngleCheck=" << cfg.ZenithV2.enableAngleCheck << "\n";
    //oss << "ZenithV2.alignmentBonus=" << cfg.ZenithV2.alignmentBonus << "\n";
    //oss << "ZenithV2.maxAngle=" << cfg.ZenithV2.maxAngle << "\n";
    //
    //oss << "ZenithV2.enableHumanization=" << cfg.ZenithV2.enableHumanization << "\n";
    //oss << "ZenithV2.noiseFrequencyX=" << cfg.ZenithV2.noiseFrequencyX << "\n";
    //oss << "ZenithV2.noiseFrequencyY=" << cfg.ZenithV2.noiseFrequencyY << "\n";
    //oss << "ZenithV2.noiseStrength=" << cfg.ZenithV2.noiseStrength << "\n";
    //oss << "ZenithV2.maxNoiseAmount=" << cfg.ZenithV2.maxNoiseAmount << "\n";
    //
    //oss << "ZenithV2.bezierP1.x=" << cfg.ZenithV2.bezierP1.x << "\n";
    //oss << "ZenithV2.bezierP1.y=" << cfg.ZenithV2.bezierP1.y << "\n";
    //oss << "ZenithV2.bezierP2.x=" << cfg.ZenithV2.bezierP2.x << "\n";
    //oss << "ZenithV2.bezierP2.y=" << cfg.ZenithV2.bezierP2.y << "\n";

    return oss.str();
}

void applyConfig(AimAssistConfig& cfg, const std::unordered_map<std::string, std::string>& map) {
    auto get = [&](const std::string& key, auto& out) {
        if (!map.count(key)) return;
        std::istringstream(map.at(key)) >> out;
        };

    int algoInt;
    get("algorithm", algoInt); cfg.algorithm = static_cast<ALGORITHM>(algoInt);

    get("enableAssist", cfg.enableAssist);
    get("hitObjectRadius", cfg.hitObjectRadius);
    get("fov", cfg.fov);
    get("power", cfg.power);
    get("waitForClick", cfg.waitForClick);
    get("legitMode", cfg.legitMode);
    get("alwaysOnSync", cfg.alwaysOnSync);
    get("reverse", cfg.reverse);

    /*easingPoints.clear();
    easingCurves.clear();

    for (int i = 0;; i++) {
        std::string valX = "easingPoint" + std::to_string(i) + ".x";
        std::string valY = "easingPoint" + std::to_string(i) + ".y";

        if (!map.count(valX) || !map.count(valY))
            break;
   
        ImVec2 point;
        get(valX, point.x);
        get(valY, point.y);

        easingPoints.push_back(point);
    }

    for (int i = 0;; i++) {
        std::string startX = "easingCurves" + std::to_string(i) + "startPos.x";
        std::string startY = "easingCurves" + std::to_string(i) + "startPos.y";
        std::string midX = "easingCurves" + std::to_string(i) + "midPos.x";
        std::string midY = "easingCurves" + std::to_string(i) + "midPos.y";
        std::string endX = "easingCurves" + std::to_string(i) + "endPos.x";
        std::string endY = "easingCurves" + std::to_string(i) + "endPos.y";

        if (!map.count(startX) || !map.count(startY) || !map.count(midX) || !map.count(midY) || !map.count(endX) || !map.count(endY))
            break;

        auto start = std::make_shared<ImVec2>();
        auto mid = std::make_shared<ImVec2>();
        auto end = std::make_shared<ImVec2>();

        get(startX, start->x);
        get(startY, start->x);
        get(midX, mid->x);
        get(midY, mid->x);
        get(endX, end->x);
        get(endY, end->x);

        easingCurves.emplace_back(start, mid, end);
    }*/

    get("ZenithV2.enableBonus", cfg.ZenithV2.enableBonus);
    get("ZenithV2.enableAngleCheck", cfg.ZenithV2.enableAngleCheck);
    get("ZenithV2.alignmentBonus", cfg.ZenithV2.alignmentBonus);
    get("ZenithV2.maxAngle", cfg.ZenithV2.maxAngle);

    get("ZenithV2.enableHumanization", cfg.ZenithV2.enableHumanization);
    get("ZenithV2.noiseFrequencyX", cfg.ZenithV2.noiseFrequencyX);
    get("ZenithV2.noiseFrequencyY", cfg.ZenithV2.noiseFrequencyY);
    get("ZenithV2.noiseStrength", cfg.ZenithV2.noiseStrength);
    get("ZenithV2.maxNoiseAmount", cfg.ZenithV2.maxNoiseAmount);

    //get("ZenithV2.bezierP1.x", cfg.ZenithV2.bezierP1.x);
    //get("ZenithV2.bezierP1.y", cfg.ZenithV2.bezierP1.y);
    //get("ZenithV2.bezierP2.x", cfg.ZenithV2.bezierP2.x);
    //get("ZenithV2.bezierP2.y", cfg.ZenithV2.bezierP2.y);
}

glm::vec2 AimAssist::Zenith(glm::vec2 pos) {
    if (!this->isPlaying)
        return pos;

    glm::vec2 hitObjectPos = {
       this->currentObj->x,
       this->currentObj->y
    };

    float distance = glm::distance(pos, hitObjectPos);
    if (distance > this->config.fov)
        return pos;

    if (distance < std::numeric_limits<float>::epsilon())
        return pos;

    float ratio = (distance / this->config.fov) * this->config.power;
    return pos * (1.0f - ratio) + (hitObjectPos * ratio);
}

/// <summary>
/// Selfmade algorithm for this cheat
/// </summary>
/// <param name="pos"> -> position of the current cursor pos (unedited)</param>
/// <returns>new cursor pos with calculated offset</returns>
glm::vec2 AimAssist::ZenithV2(glm::vec2 pos) {
    if (!this->isPlaying)
        return pos;

    glm::vec2 hitObjectPos = {
        this->currentObj->x,
        this->currentObj->y
    };

    float distance = glm::distance(pos, hitObjectPos);

    if (distance > this->config.fov || distance < std::numeric_limits<float>::epsilon())
        return pos;

    if (this->curTime < this->currentObj->startTime)
        return pos;

    float gotoRatio = static_cast<float>(this->curTime - this->currentObj->startTime) / this->extendTime;
    gotoRatio = glm::clamp(gotoRatio, 0.0f, 1.0f);

    float ratio = gotoRatio;

    // Only go back if the next position is different to the current pos to prevent weird movement for bursts etc
    if (!this->isSamePos) {
        float goBackRatio = static_cast<float>(this->curTime - this->currentObj->hitTime) / this->extendTime;
        goBackRatio = glm::clamp(goBackRatio, 0.0f, 1.0f);
        ratio -= goBackRatio;
    }

    glm::vec2 normalizedDist = glm::normalize(hitObjectPos - pos);
    glm::vec2 velocity = glm::normalize(pos - this->lastMousePos); // direction of mouse movement

    float velocityDot = glm::clamp(glm::dot(velocity, normalizedDist), -1.0f, 1.0f);
    float angle = std::acos(velocityDot); // angle between movement direction and target direction

    const float maxAngle = glm::radians(this->config.ZenithV2.maxAngle);
    if (angle > maxAngle && this->config.ZenithV2.enableAngleCheck)
        return pos;

    float power = this->config.legitMode ? this->config.power / 2.0f : this->config.power;
    if (angle <= maxAngle && this->config.ZenithV2.enableBonus)
        power = this->config.ZenithV2.alignmentBonus;

    ratio *= power;

    float curvedRatio = 0.0f;
    //if (this->config.reverse) {
    //    if (ratio >= 0.0f)
    //        curvedRatio = EvaluateCubicBezier(ratio, this->config.ZenithV2.bezierP1.y, this->config.ZenithV2.bezierP2.y);
    //    else
    //        curvedRatio = EvaluateCubicBezier(-ratio, this->config.ZenithV2.bezierP2.y, this->config.ZenithV2.bezierP1.y);
    //}
    //else {
    //    curvedRatio = EvaluateCubicBezier(ratio, this->config.ZenithV2.bezierP1.y, this->config.ZenithV2.bezierP2.y);
    //}

    glm::vec2 result = pos * (1.0f - curvedRatio) + hitObjectPos * curvedRatio;

    if (this->config.ZenithV2.enableHumanization) {
        float time = static_cast<float>(this->curTime);

        float noiseX = static_cast<float>(std::sin(time * this->config.ZenithV2.noiseFrequencyX * 2.0f * M_PI));
        float noiseY = static_cast<float>(std::cos(time * this->config.ZenithV2.noiseFrequencyY * 2.0f * M_PI));

        glm::vec2 noise = glm::vec2(noiseX, noiseY) * this->config.ZenithV2.noiseStrength;
        noise = glm::clamp(noise, -this->config.ZenithV2.maxNoiseAmount, this->config.ZenithV2.maxNoiseAmount);

        result += noise;
    }

    return result;
}

LRESULT CALLBACK mouseHookProc(int code, WPARAM wParam, LPARAM lParam) {
    
    AimAssist* self = globals.aimAssist;

    if (code < 0)
        return CallNextHookEx(self->mouseHook, code, wParam, lParam);

    if (wParam != WM_MOUSEMOVE) 
        return CallNextHookEx(self->mouseHook, code, wParam, lParam);

    MSLLHOOKSTRUCT* curMouseStruct = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);
    if (curMouseStruct == nullptr)
        return CallNextHookEx(self->mouseHook, code, wParam, lParam);
    
    glm::vec2 osuMousePos = globals.osu->convertAbsToOsu({ 
        curMouseStruct->pt.x, 
        curMouseStruct->pt.y 
        });

    self->lastAlgorithmPos = self->ZenithV2(osuMousePos);

    self->newMousePos = globals.osu->convertOsuToAbs(self->lastAlgorithmPos);

    SetCursorPos(static_cast<int>(self->newMousePos.x), static_cast<int>(self->newMousePos.y));

    return 1;
}

bool AimAssist::init(void* state) {
    this->mouseHook = SetWindowsHookEx(WH_MOUSE_LL, mouseHookProc, GetModuleHandle(NULL), 0);

    return true;
}

void AimAssist::deInit(void* state) {
    UnhookWindowsHookEx(this->mouseHook);
}
bool sex;

void AimAssist::renderMenu(void* state) {

    if (globals.cfg->configs.empty()) {

        globals.cfg->configs.emplace_back("Config1", "TestCfg", time(nullptr) - 20, "Stood");
        globals.cfg->configs.emplace_back("Config2", "TestCfg", time(nullptr) - 30, "Mika");
        globals.cfg->configs.emplace_back("Config3", "TestCfg", time(nullptr) - 30, "Mika");
        globals.cfg->configs.emplace_back("Config4", "TestCfg", time(nullptr) - 30, "Mika");
        globals.cfg->configs.emplace_back("Config5", "TestCfg", time(nullptr) - 30, "Mika");
        globals.cfg->configs.emplace_back("Config6", "TestCfg", time(nullptr) - 30, "Mika");
        globals.cfg->configs.emplace_back("Config7", "TestCfg", time(nullptr) - 30, "Mika");
        globals.cfg->configs.emplace_back("Config8", "TestCfg", time(nullptr) - 30, "Mika");
        globals.cfg->configs.emplace_back("Config9", "TestCfg", time(nullptr) - 30, "Mika");
        globals.cfg->configs.emplace_back("Config10", "TestCfg", time(nullptr) - 30, "Mika");

    }

    ImGuiStyle* style = &ImGui::GetStyle();
    const float bottomContentWidth = ImGui::GetWindowWidth();
    const float halfSize = bottomContentWidth / 2.0f - style->FramePadding.x / 2;
    
    ImGui::BeginChild("##LeftContent", ImVec2(halfSize, 0.0f));
    {
        ImGui::cBeginChild("General", ImVec2(0.0f, 250));
        {
            ImGui::Switch("Enable", &this->config.enableAssist);
            ImGui::Switch("Legit Mode", &this->config.legitMode);
            ImGui::Switch("Wait For Click", &this->config.waitForClick);
            ImGui::SliderFlt("Power", &this->config.power, 0.0f, 1.0f, "%.2fx");
            ImGui::SliderFlt("FoV", &this->config.fov, 0.0f, 960.0f, "%.2f");
        }
        ImGui::cEndChild();
    
        ImGui::cBeginChild("Animation", ImVec2(0.0f, 145));
        {
        }
        ImGui::cEndChild();
    }
    ImGui::cEndChild();
    
    ImGui::SameLine();
    
    ImGui::BeginChild("##RightContent", ImVec2(halfSize, 0.0f));
    {
        ImGui::cBeginChild("Algorithm", ImVec2(0.0f, 145));
        {
            static int selected = 0;
            ImGui::ComboBox("TestBox", {
                { "Zenith", "Distance based", ICON_LC_DNA },
                { "ZenithV2", "Timing based", ICON_LC_ACTIVITY }
                },
                &selected);
        }
        ImGui::cEndChild();
    
        ImGui::cBeginChild("Humanization", ImVec2(0.0f, 300));
        {
            ImGui::SliderFlt("Alignment Bonus", &config.ZenithV2.alignmentBonus, 0.0f, 1.0f, "%.2f");
            ImGui::SliderFlt("Strength", &config.ZenithV2.noiseStrength, 0.0f, 1.0f, "%.3fx");
            ImGui::SliderFlt("Noise X", &config.ZenithV2.noiseFrequencyX, 0.0f, 0.1f, "%.3f px");
            ImGui::SliderFlt("Noise Y", &config.ZenithV2.noiseFrequencyY, 0.0f, 0.1f, "%.3f px");
            ImGui::SliderFlt("Max Noise", &config.ZenithV2.maxNoiseAmount, 0.0f, 3.0f, "%.3f px");
        }
        ImGui::cEndChild();
    }
    ImGui::cEndChild();
}

void AimAssist::saveConfig(void* state, nlohmann::json* cfg) {
    
}

void AimAssist::loadConfig(void* state, const nlohmann::json& cfg) {

}

void AimAssist::update(void* state) {
    Globals* globals = reinterpret_cast<Globals*>(state);

    if (!this->config.enableAssist)
        return;

    if (globals->osu->curState != 2) {
        this->isPlaying = false;
        this->currentObjIndex = 0;
        this->currentObj = nullptr;
        this->assistedObjects.clear();
        globals->beatMapParser->currentBeatmap.hitObjects.objects.clear();
        return;
    }

    this->curTime = globals->osu->curTime;

    if (globals->osu->isRetrying == 1 || globals->osu->isDead == 1) {
        currentObjIndex = 0;
        return;
    }

    if (this->curTime < 200) {
        this->currentObjIndex = 0;
        return;
    }
    
    this->oldTime = globals->osu->curTime;

    size_t hitObjectsAmount = globals->beatMapParser->currentBeatmap.hitObjects.objects.size();

    if (hitObjectsAmount == 0)
        return;

    if (this->assistedObjects.empty()) {

        this->assistedObjects.reserve(hitObjectsAmount);

        for (int i = 0; i < hitObjectsAmount; i++) {
            // Get the current obj and the next obj
            Beatmap::HitObject* currentObj = &globals->beatMapParser->currentBeatmap.hitObjects.objects[i];
            
            Beatmap::HitObject* nextObj = nullptr;
            if (i < hitObjectsAmount - 1)
                nextObj = &globals->beatMapParser->currentBeatmap.hitObjects.objects[i + 1];
            
            // Calculate start and end time
            int32_t startTime = currentObj->timeToHit - this->extendTime;
            int32_t endTime = currentObj->timeToHit + this->extendTime;

            // Check wether we need a correction in the end time
            if (nextObj) {
                bool needsCorrection = (nextObj->timeToHit - currentObj->timeToHit) < this->extendTime;
                if (needsCorrection || this->config.alwaysOnSync)
                    endTime = currentObj->timeToHit + this->extendTime + (this->extendTime / 2);
            }
            
            this->assistedObjects.emplace_back(
                currentObj->x, 
                currentObj->y, 
                startTime, 
                currentObj->timeToHit, 
                endTime
            );
        }

        this->currentObj = &this->assistedObjects[this->currentObjIndex];
    }

    if (!this->currentObj)
        return; // Object is invalid

    if (this->curTime < this->currentObj->endTime)
        return; // Do not increment, its not the end yet

    this->currentObjIndex++;

    if (this->currentObjIndex > hitObjectsAmount)
        return; // We are out of objects

    this->currentObj = &this->assistedObjects[this->currentObjIndex];
    this->nextObj = &this->assistedObjects[this->currentObjIndex + 1];

    if (this->currentObj->x == this->nextObj->x && this->currentObj->y == this->nextObj->y)
        this->isSamePos = true;
    else
        this->isSamePos = false;

    this->isPlaying = true;
}
