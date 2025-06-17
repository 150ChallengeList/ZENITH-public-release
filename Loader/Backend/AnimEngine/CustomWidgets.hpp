#ifndef CUSTOMWIDGETS_HPP
#define CUSTOMWIDGETS_HPP

#include "AnimEngine.hpp"
#include <imgui.h>
#include <functional>

namespace ImGui {

	inline auto rgbaToImVec4 = [](uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		return ImVec4(
			static_cast<float>(r) / 255.0f,
			static_cast<float>(g) / 255.0f,
			static_cast<float>(b) / 255.0f,
			static_cast<float>(a) / 255.0f
		);
	};


	inline static ImColor accentBackground = ImGui::rgbaToImVec4(22, 22, 34, 255);

	inline static ImColor accentGrayDark = ImGui::rgbaToImVec4(42, 42, 58, 255);
	inline static ImColor accentGrayMedium1 = ImGui::rgbaToImVec4(30, 30, 45, 255);
	inline static ImColor accentGrayMedium2 = ImGui::rgbaToImVec4(36, 36, 52, 255);
	inline static ImColor accentGrayBright = ImGui::rgbaToImVec4(108, 114, 147, 255);

	inline static ImColor accentPurpleDark = ImGui::rgbaToImVec4(54, 41, 92, 255);
	inline static ImColor accentPurpleMedium1 = ImGui::rgbaToImVec4(40, 36, 63, 255);
	inline static ImColor accentPurpleMedium2 = ImGui::rgbaToImVec4(45, 41, 68, 255);
	inline static ImColor accentPurpleBright = ImGui::rgbaToImVec4(144, 106, 231, 255);
	
	inline static ImColor accentIconBackground = ImGui::rgbaToImVec4(108, 73, 190, 255);

	inline static ImColor textColorLight = ImGui::rgbaToImVec4(255, 255, 255, 255);
	inline static ImColor textColorMedium = ImGui::rgbaToImVec4(108, 114, 147, 255);
	inline static ImColor textColorDark = ImGui::rgbaToImVec4(79, 93, 109, 255);

	IMGUI_API bool Switch(const char* label, bool* state);
	
	IMGUI_API bool SliderInt(const char* label, int* state, int min, int max, const char* fmt = "");
	IMGUI_API bool SliderFlt(const char* label, float* state, float min, float max, const char* fmt = "");
	IMGUI_API bool SliderDouble(const char* label, double* state, double min, double max, const char* fmt = "");
	IMGUI_API bool SliderWrapper(const char* label, void* state, ImGuiDataType dataType, const void* min, const void* max, const char* fmt = "");
	
	IMGUI_API bool BezierEditor(const char* label, ImVec2* pos1, ImVec2* pos2);

	IMGUI_API void cBeginChild(const char* label, const ImVec2& sizeArg = ImVec2(0.0f, 0.0f), const char* icon = nullptr);
	IMGUI_API void cEndChild();
	
	IMGUI_API bool TabButton(const char* label, const char* icon = nullptr, bool* state = nullptr);
	IMGUI_API bool TopBarTabs(const std::vector<std::string>& tabNames, std::string* currentSelected, const std::unordered_map<std::string, const char*> tabIcons = {});
	
	struct ComboOption {
		const char* name;
		const char* description;
		const char* icon;
	};
	IMGUI_API bool ComboBox(const char* label, const std::vector<ComboOption>& options, int* selectedIndex);
	IMGUI_API bool CustomButton(const char* label, const char* description, const char* icon);

	//IMGUI_API bool ChildContentSlider(std::function<void()> currentContent, std::function<void()> nextContent, int direction);
}

#endif CUSTOMWIDGETS_HPP