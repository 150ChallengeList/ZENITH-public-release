#include <fstream>
#include <sstream>
#include "Cfg.hpp"
#include <AnimEngine/CustomWidgets.hpp>


bool Cfg::writeToCfg(const std::string& data, const std::string& filename, char key = 0x75) {
    std::ofstream file(filename, std::ios::binary);
    // Encrypt cfg
    for (char c : data) {
        file.put(c);
    }
    file.close();

    return true;
}

std::string Cfg::readCfg(const std::string& filename, char key = 0x75) {
    std::ifstream file(filename, std::ios::binary);
    std::string data;
    char c;
    // Decrypt cfg
    while (file.get(c)) {
        data += c;
    }
    return data;
}

std::unordered_map<std::string, std::string> Cfg::parseConfig(const std::string& rawConfig) {
    std::unordered_map<std::string, std::string> config;
    std::istringstream iss(rawConfig);
    std::string line;

    while (std::getline(iss, line)) {
        size_t pos = line.find('=');
        if (pos == std::string::npos) continue;
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        config[key] = value;
    }
    return config;
}

void Cfg::renderMenu(void* state) {

    ImGuiStyle* style = &ImGui::GetStyle();
    const float bottomContentWidth = ImGui::GetWindowWidth();
    const float halfSize = bottomContentWidth / 2.0f - style->FramePadding.x / 2;

    static Config* selectedCfg = &this->configs.front();
    ImGui::BeginChild("##LeftContent", ImVec2(halfSize, 0.0f));
    {
        ImGui::ListCfgs("Configurations", &this->configs, &selectedCfg);
    }
    ImGui::cEndChild();

    ImGui::SameLine();

    ImGui::BeginChild("##RightContent", ImVec2(halfSize, 0.0f));
    {
        ImGui::ShowCfgInfo("ConfigPreview", &this->configs, selectedCfg);
    }
    ImGui::cEndChild();

}