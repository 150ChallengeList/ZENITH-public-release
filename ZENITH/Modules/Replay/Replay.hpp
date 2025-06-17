#ifndef REPLAY_HPP
#define REPLAY_HPP

#include <Chiyo/Decoders/ReplayDecoder.h>
#include <ClassManager/BaseClass.hpp>
#include <string>

class ReplayBot : public BaseClass {
private:

    bool enableReplayBot = false;
    bool replayLoaded = false;
    bool flipReplay = false;
    bool disableAim = false;
    bool disableClicking = false;
    int currentFrameIndex = -1;

    Replay currentReplay;
    Replay currentSecondReplay;

    bool useDuo = false;
    char currentKey = '\0';

    bool isXDown = false;
    bool isYDown = false;

    float calculateOffset(float a, float b);

public:
    void loadSingleReplay(const std::string& file);
    void loadDuoReplay(const std::string& file1, const std::string& file2);

    void renderMenu(void* state) override;

    bool init(void* state) override;
    void update(void* state) override;
};

#endif REPLAY_HPP