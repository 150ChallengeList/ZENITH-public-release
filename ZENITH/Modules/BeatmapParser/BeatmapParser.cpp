#include "../Globals/Globals.hpp"
#include "BeatmapParser.hpp"
#include "Windows.h"
#include <assert.h>
#include <fstream>

void Beatmap::TimingPoint::parse(const std::string& line) {

	std::vector<std::string> vars;

	std::stringstream ss(line);
	std::string item;
	while (std::getline(ss, item, ','))
		vars.emplace_back(item);

	this->time = std::stoi(vars[0]);
	this->beatLength = std::stof(vars[1]);
	this->meter = std::stoi(vars[2]);
	this->sampleSet = std::stoi(vars[3]);
	this->sampleIndex = std::stoi(vars[4]);
	this->volume = std::stoi(vars[5]);
	this->uninherited = static_cast<bool>(std::stoi(vars[6]));
	this->effects = std::stoi(vars[7]);
}
void Beatmap::TimingPoints::parse(const std::string& blockContent) {

	std::stringstream ss;
	ss << blockContent;

	std::string line;
	while (std::getline(ss, line))
		this->points.emplace_back().parse(line);

}

void Beatmap::HitObject::parse(const std::string& line, int scaleWidth, int scaleHeight, int previousX, int previousY, int previousStack) {
    std::vector<std::string> vars;
    std::stringstream ss(line);
    std::string item;

    while (std::getline(ss, item, ',')) {
        vars.emplace_back(item);
    }

    float playfieldHeight = scaleHeight * 0.8f;
    float playfieldWidth = playfieldHeight * (4.0f / 3.0f);  // Maintain aspect ratio

    float osuScale = playfieldWidth / 512.0f;

    // Ensure correct playfield centering
    float offsetX = (scaleWidth - playfieldWidth) / 2.0f;
    float offsetY = (scaleHeight - playfieldHeight) / 2.0f;

    int parsedX = std::stoi(vars[0]);
    int parsedY = std::stoi(vars[1]);

    // Default stack offset (osu! uses 6px per stack level)
    int stackOffset = (previousX == parsedX && previousY == parsedY) ? previousStack + 1 : 0;

    // Apply osu! stacking offset
    float osuStackOffset = -stackOffset * 6 * osuScale;

    this->x = static_cast<int32_t>(offsetX + static_cast<float>(parsedX) * osuScale + osuStackOffset);
    this->y = static_cast<int32_t>(offsetY + static_cast<float>(parsedY) * osuScale + osuStackOffset) + 17;

    this->timeToHit = std::stoi(vars[2]);

    int objectType = std::stoi(vars[3]);
    this->isSlider = (objectType & 2) != 0;

    // Store the stack index for the next hit object
    this->stackIndex = stackOffset;

}


void Beatmap::HitObjects::parse(const std::string& blockContent, int scaleWidth, int scaleHeight) {
    std::stringstream ss(blockContent);
    std::string line;

    // Variables to track the last hit object's position and stack index
    int lastX = -1, lastY = -1, lastStack = 0;

    while (std::getline(ss, line)) {
        // Create a new HitObject and parse it
        this->objects.emplace_back();
        HitObject& obj = this->objects.back();  // Get reference to the newly created object

        obj.parse(line, scaleWidth, scaleHeight, lastX, lastY, lastStack);

        // Update tracking variables for stacking logic
        lastX = obj.x;
        lastY = obj.y;
        lastStack = obj.stackIndex;
    }
}

void Beatmap::Difficulty::parse(const std::string& blockContent) {

	std::vector<std::string> varsToRead = {
		"CircleSize",
        "OverallDifficulty",
        "ApproachRate",
		"SliderMultiplier",
		"SliderTickRate"
	};

	for (const auto& var : varsToRead) {

		size_t varPos = blockContent.find(var);
		if (varPos == std::string::npos) {
			assert(false);
			return;
		}
		varPos += var.size() + 1; // +1 to skip :

		float varVal = std::stof(blockContent.substr(varPos, blockContent.find("\n", varPos) - varPos));

		if (var == "CircleSize")
			this->circleSize = varVal;

        if (var == "OverallDifficulty")
            this->overallDifficulty = varVal;

		if (var == "ApproachRate")
			this->approachRate = varVal;

		if (var == "SliderMultiplier")
			this->sliderMultiplier = varVal;

		if (var == "SliderTickRate")
			this->sliderTickRate = varVal;
	}
}

void Beatmap::parse(const std::filesystem::path& path, int scaleWidth, int scaleHeight) {
	this->fileName = path.filename().string();

	std::ifstream file(path);
	std::stringstream ss;
	ss << file.rdbuf();
	file.close();
	std::string fileContent = ss.str();

	std::vector<std::string> blocksToRead = {
		"Difficulty",
		"TimingPoints",
		"HitObjects"
	};

	for (const auto& blockToFind : blocksToRead) {
		size_t blockStart = fileContent.find(blockToFind);
		if (blockStart == std::string::npos) {
			assert(false);
			return;
		}

		blockStart += blockToFind.size() + 2; // +2 -> skip ]\n
		const std::string& blockData = fileContent.substr(blockStart, fileContent.find("\n\n", blockStart) - blockStart);

		if (blockToFind == "Difficulty")
			this->difficulty.parse(blockData);

		if (blockToFind == "TimingPoints")
			this->timingPoints.parse(blockData);

		if (blockToFind == "HitObjects")
			this->hitObjects.parse(blockData, scaleWidth, scaleHeight);

	}
}

bool BeatmapParser::init(void* state) {
	this->state = state;

    return true;
}

void BeatmapParser::watchWindow(HWND windowHandle) {
	this->windowToWatch = windowHandle;
}

bool BeatmapParser::parse(const std::filesystem::path& path, int scaleWidth, int scaleHeight) {

	this->currentBeatmap.parse(path, scaleWidth, scaleHeight);

	return true;
}

bool BeatmapParser::parseAll() {

	Globals* globals = reinterpret_cast<Globals*>(this->state);

	for (const auto& file : std::filesystem::recursive_directory_iterator(globals->osu->gameFolder)) {
		const std::filesystem::path& path = file.path();
		if (path.extension().string() != ".osu")
			continue;
		this->parse(path);
	}

	return true;
}

std::filesystem::path BeatmapParser::findMapFolder(int32_t setId) {
    Globals* globals = reinterpret_cast<Globals*>(this->state);
    const std::filesystem::path& gameFolder = globals->osu->gameFolder / "Songs";
    std::string setIdStr = std::to_string(setId);

    for (const auto& entry : std::filesystem::directory_iterator(gameFolder)) {
		if (!entry.is_directory())
			continue;

        const std::string& folderName = entry.path().filename().string();
		if (folderName.rfind(setIdStr, 0) != 0) // starts with setId
			continue;

        return entry.path();
    }

    return {}; // return empty path if not found
}

std::filesystem::path BeatmapParser::getCurrentMapPath(std::filesystem::path folderPath, int32_t mapId) {
	std::string mapIdStr = std::to_string(mapId);

	for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
		if (!entry.is_regular_file())
			continue;

		const std::filesystem::path& filePath = entry.path();

		if (filePath.extension() != ".osu") 
			continue;

		std::ifstream file(filePath);
		if (!file.is_open()) 
			continue;

		std::string line;
		while (std::getline(file, line)) {
			if (line.rfind("BeatmapID:", 0) != 0)
				continue;

			std::string value = line.substr(10); // skip "BeatmapID:"
			value.erase(0, value.find_first_not_of(" \t")); // trim left whitespace

			if (value != mapIdStr)
				continue;

			return filePath;
		}
	}

	return {}; // return empty path if not found
}

void BeatmapParser::tick(void* state) {
	this->state = state;
	Globals* globals = reinterpret_cast<Globals*>(this->state);

	if (globals->osu->curState != OSU_STATE::PLAY) {
		this->isParsed = false;
		this->currentBeatmapId = 0;
		return;
	}

	if (globals->osu->curTime == 0) {
		if (this->currentBeatmapId != 0)
			return;

		printf("parsing %i\n", globals->osu->mapId);
		printf("parsing %i\n", globals->osu->setId);

		std::filesystem::path songsFolder = this->findMapFolder(globals->osu->setId);

		if (songsFolder.empty())
			return;

		std::filesystem::path mapPath = this->getCurrentMapPath(songsFolder, globals->osu->mapId);

		if (mapPath.empty())
			return;

		if (!isParsed) {
			this->currentBeatmap.parse(mapPath.string(), 1920, 1080);
			this->isParsed = true;
			this->currentBeatmapId = globals->osu->mapId; // Placeholder
			printf("%i, %i\n", this->currentBeatmap.hitObjects.objects[2].x, this->currentBeatmap.hitObjects.objects[2].y);
			printf("[Beatmap Parser] Parsed: %s\n", mapPath.string().c_str());
		}
	}
}
