#ifndef BEATMAPPARSER_HPP
#define BEATMAPPARSER_HPP

#include <ClassManager/BaseClass.hpp>
#include <unordered_map>
#include <filesystem>
#include <Windows.h>

class Beatmap {
public:
    struct TimingPoint {
        int32_t time;
        float beatLength;
        int32_t meter;
        int32_t sampleSet;
        int32_t sampleIndex;
        int32_t volume;
        bool uninherited;
        int32_t effects;

        void parse(const std::string& line);
    };
    struct TimingPoints {
        std::vector<TimingPoint> points;

        void parse(const std::string& blockContent);
    };

    struct HitObject {
        int32_t x;
        int32_t y;
        int32_t timeToHit;
        bool isSlider;
        int stackIndex;

        void parse(const std::string& line, int scaleWidth, int scaleHeight, int previousX, int previousY, int previousStack);
    };
    struct HitObjects {
        std::vector<HitObject> objects;

        void parse(const std::string& blockContent, int scaleWidth, int scaleHeight);
    };

    struct Difficulty {
        float circleSize;
        float overallDifficulty;
        float approachRate;
        float sliderMultiplier;
        float sliderTickRate;

        void parse(const std::string& blockContent);
    };

public:
    
    Difficulty difficulty;
    TimingPoints timingPoints;
    HitObjects hitObjects;

    //parser.difficulty.circleSize

    std::string fileName;

    void parse(const std::filesystem::path& path, int scaleWidth, int scaleHeight);

    // Add the hashCode method
    int32_t hashCode() const {
        std::hash<std::string> hasher;
        return static_cast<int32_t>(hasher(fileName));
    }
};

class BeatmapParser : public BaseClass {
private:
    void* state = nullptr;
    HWND windowToWatch = NULL;

public:
    Beatmap currentBeatmap;
    uint32_t currentBeatmapId;

    bool isParsed;
    bool isFound;

    bool init(void* state) override;
    void watchWindow(HWND windowHandle);

    bool parse(const std::filesystem::path& path, int scaleWidth = 1920, int scaleHeight = 1080);
    bool parseAll();

    std::filesystem::path findMapFolder(int32_t setId);
    std::filesystem::path getCurrentMapPath(std::filesystem::path folderPath, int32_t mapId);

    void tick(void* state) override;
};

#endif // BEATMAPPARSER_HPP
