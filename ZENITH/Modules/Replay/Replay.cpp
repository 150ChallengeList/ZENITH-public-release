#include <iostream>
#include <Globals.hpp>
#include <Replay/Replay.hpp>

void printKeyFromCode(int keyCode) {
    if (keyCode < 0x30 || keyCode > 0x5A)
        return;

    char keyChar = static_cast<char>(keyCode);
}

int offset = -20;

void pressKey(WORD vk) {
    INPUT input = { 0 };
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = vk;
    input.ki.dwFlags = 0;
    SendInput(1, &input, sizeof(INPUT));
}
void releaseKey(WORD vk) {
    INPUT input = { 0 };
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = vk;
    input.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &input, sizeof(INPUT));
}

inline float ReplayBot::calculateOffset(float a, float b) {
    return (a + b) / 2.0f;
}

void ReplayBot::loadSingleReplay(const std::string& file) {

    this->currentReplay = ReplayDecoder::Decode(file);

    if (!this->currentReplay.ReplayFrames.empty()) {
        const auto& trimEnd = this->currentReplay.ReplayFrames.size() > 2 ?
            this->currentReplay.ReplayFrames.begin() + 2 :
            this->currentReplay.ReplayFrames.end();

        this->currentReplay.ReplayFrames.erase(this->currentReplay.ReplayFrames.begin(), trimEnd);
    }

    this->replayLoaded = !this->currentReplay.ReplayFrames.empty();
    std::cout << "[ReplayBot] Replay loaded: " << std::boolalpha << replayLoaded << std::endl;
}

void ReplayBot::loadDuoReplay(const std::string& file1, const std::string& file2) {
    
    this->currentReplay = ReplayDecoder::Decode(file1);
    this->currentSecondReplay = ReplayDecoder::Decode(file2);

    auto trimFrames = [](Replay& replay) {
        if (replay.ReplayFrames.empty())
            return;

        const auto& trimEnd = replay.ReplayFrames.size() > 2 ? 
            replay.ReplayFrames.begin() + 2 :
            replay.ReplayFrames.end();

        replay.ReplayFrames.erase(replay.ReplayFrames.begin(), trimEnd);
    };

    trimFrames(this->currentReplay);
    trimFrames(this->currentSecondReplay);

    bool bothLoaded = !this->currentReplay.ReplayFrames.empty() && !this->currentSecondReplay.ReplayFrames.empty();

    this->replayLoaded = bothLoaded;
}

void ReplayBot::renderMenu(void* state) {

}

bool ReplayBot::init(void* state) {
    this->currentFrameIndex = 0;
    this->loadSingleReplay("C:\\Users\\rushe\\Pictures\\1.osr");
    return true;
}

void ReplayBot::update(void* state) {
    Globals* globals = reinterpret_cast<Globals*>(state);

    if (this->enableReplayBot == false)
        return;

    if (globals->osu->curState != 2)
        return;

    const int time = globals->osu->curTime - offset;

    bool newFrame = false;
    glm::vec2 pos;
    if (currentFrameIndex < currentReplay.ReplayFrames.size() - 1 && currentReplay.ReplayFrames[currentFrameIndex + 1].Time <= time) {
        currentFrameIndex++;
        newFrame = true;
    }

    bool runningSlow = time - currentReplay.ReplayFrames[currentFrameIndex].Time > 16 && newFrame;

    if (runningSlow)
        while (currentFrameIndex < currentReplay.ReplayFrames.size() - 1 && currentReplay.ReplayFrames[currentFrameIndex].OsuKeys == OsuKeys::None && currentReplay.ReplayFrames[currentFrameIndex + 1].Time <= time)
            currentFrameIndex++;

    if (newFrame)
    {
        pos = glm::vec2(currentReplay.ReplayFrames[currentFrameIndex].X, currentReplay.ReplayFrames[currentFrameIndex].Y);
    }
    else if (currentReplay.ReplayFrames[currentFrameIndex].Time >= time)
    {
        int p1 = std::max(0, currentFrameIndex - 1);
        int p2 = currentFrameIndex;

        float t = currentReplay.ReplayFrames[p2].Time == currentReplay.ReplayFrames[p1].Time ? 0.f : std::max(0.f, (1 - (float)(currentReplay.ReplayFrames[p2].Time - globals->osu->curTime) / (currentReplay.ReplayFrames[p2].Time - currentReplay.ReplayFrames[p1].Time)));
        pos = glm::vec2(std::lerp(currentReplay.ReplayFrames[p1].X, currentReplay.ReplayFrames[p2].X, t), std::lerp(currentReplay.ReplayFrames[p1].Y, currentReplay.ReplayFrames[p2].Y, t));
    }
    else
    {
        int p1 = currentFrameIndex;
        int p2 = std::min((int)currentReplay.ReplayFrames.size() - 1, currentFrameIndex + 1);

        float t = currentReplay.ReplayFrames[p2].Time == currentReplay.ReplayFrames[p1].Time ? 0.f : std::max(0.f, (1 - (float)(currentReplay.ReplayFrames[p2].Time - globals->osu->curTime) / (currentReplay.ReplayFrames[p2].Time - currentReplay.ReplayFrames[p1].Time)));

        pos = glm::vec2(std::lerp(currentReplay.ReplayFrames[p1].X, currentReplay.ReplayFrames[p2].X, t), std::lerp(currentReplay.ReplayFrames[p1].Y, currentReplay.ReplayFrames[p2].Y, t));
    }

    OsuKeys key = currentReplay.ReplayFrames[currentFrameIndex].OsuKeys;

    // Store the current keys
    WORD KEY_LEFT = static_cast<WORD>(globals->osu->curLeftKey);
    WORD KEY_RIGHT = static_cast<WORD>(globals->osu->curRightKey);

    printf("%i\n", globals->osu->curLeftKey);
    printf("%i\n", globals->osu->curRightKey);


    if (!this->disableClicking) {
        if (key == OsuKeys::K1 && !isXDown) {
            pressKey(KEY_LEFT);
            isXDown = true;
        }
        else if (key != OsuKeys::K1 && isXDown) {
            releaseKey(KEY_LEFT);
            isXDown = false;
        }

        if (key == OsuKeys::K2 && !isYDown) {
            pressKey(KEY_RIGHT);
            isYDown = true;
        }
        else if (key != OsuKeys::K2 && isYDown) {
            releaseKey(KEY_RIGHT);
            isYDown = false;
        }
    }

    const float screenWidth = 1920.0f;
    const float screenHeight = 1080.0f;

    const float playfieldHeight = screenHeight * 0.805f;
    const float playfieldWidth = playfieldHeight * (4.0f / 3.0f);
    const float osuScale = playfieldWidth / 512.0f;

    const float offsetX = (screenWidth - playfieldWidth) / 2.0f;
    const float offsetY = (screenHeight - playfieldHeight) / 2.0f + 17;

    float cursorX = offsetX + pos.x * osuScale;
    float cursorY = offsetY + (this->flipReplay ? 384.f - pos.y : pos.y) * osuScale;

    SetCursorPos(static_cast<int>(cursorX), static_cast<int>(cursorY));

    //currentCursorPosition = GameField::FieldToDisplay(flipReplay ? Vec2(pos.x, 384.f - pos.y) : pos);
}