#ifndef CUSTOMWIDGETS_HPP
#define CUSTOMWIDGETS_HPP

#include "AnimEngine.hpp"
#include <imgui.h>
#include <imgui_internal.h>
#include <unordered_set>
#include <functional>

namespace std {
	template<>
	struct hash<ImVec2> {
		size_t operator()(const ImVec2& v) const noexcept {
			size_t hx = std::hash<float>{}(v.x);
			size_t hy = std::hash<float>{}(v.y);
			return hx ^ (hy << 1);
		}
	};
}

namespace ImGui {

	inline auto rgbaToImVec4 = [](uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		return ImVec4(
			static_cast<float>(r) / 255.0f,
			static_cast<float>(g) / 255.0f,
			static_cast<float>(b) / 255.0f,
			static_cast<float>(a) / 255.0f
		);
	};

	static ImColor accentBackground = ImGui::rgbaToImVec4(22, 22, 34, 255);

	static ImColor accentGrayDark = ImGui::rgbaToImVec4(42, 42, 58, 255);
	static ImColor accentGrayMedium1 = ImGui::rgbaToImVec4(30, 30, 45, 255);
	static ImColor accentGrayMedium2 = ImGui::rgbaToImVec4(36, 36, 52, 255);
	static ImColor accentGrayMedium3 = ImGui::rgbaToImVec4(60, 60, 86, 255);
	static ImColor accentGrayBright = ImGui::rgbaToImVec4(108, 114, 147, 255);

	static ImColor accentPurpleDark = ImGui::rgbaToImVec4(54, 41, 92, 255);
	static ImColor accentPurpleMedium0 = ImGui::rgbaToImVec4(25, 25, 39, 255);
	static ImColor accentPurpleMedium1 = ImGui::rgbaToImVec4(40, 36, 63, 255);
	static ImColor accentPurpleMedium2 = ImGui::rgbaToImVec4(45, 41, 68, 255);
	static ImColor accentPurpleMedium3 = ImGui::rgbaToImVec4(106, 78, 170, 255);
	static ImColor accentPurpleBright = ImGui::rgbaToImVec4(144, 106, 231, 255);
	
	static ImColor accentIconBackground = ImGui::rgbaToImVec4(108, 73, 190, 255);

	static ImColor textColorLight = ImGui::rgbaToImVec4(255, 255, 255, 255);
	static ImColor textColorMedium = ImGui::rgbaToImVec4(108, 114, 147, 255);
	static ImColor textColorDark = ImGui::rgbaToImVec4(79, 93, 109, 255);

	IMGUI_API void PushGlobalAlpha(float alpha);
	IMGUI_API void PopGlobalAlpha();

	IMGUI_API bool Switch(const char* label, bool* state, ImVec2 sizeArg = ImVec2(0.0f, 30.0f));

	IMGUI_API bool SliderInt(const char* label, int* state, int min, int max, const char* fmt = "", ImVec2 sizeArg = ImVec2(0.0f, 40.0f));
	IMGUI_API bool SliderFlt(const char* label, float* state, float min, float max, const char* fmt = "", ImVec2 sizeArg = ImVec2(0.0f, 40.0f));
	IMGUI_API bool SliderDouble(const char* label, double* state, double min, double max, const char* fmt = "", ImVec2 sizeArg = ImVec2(0.0f, 40.0f));
	IMGUI_API bool SliderWrapper(const char* label, void* state, ImGuiDataType dataType, const void* min, const void* max, ImVec2 sizeArg, const char* fmt = "");
	
	IMGUI_API bool ListCfgs(const char* label, std::vector<Config>* configs, Config** selected);
	IMGUI_API bool ShowCfgInfo(const char* label, std::vector<Config>* configs, Config* config);
	IMGUI_API bool ConfigButton(const Config& config, bool* state);

	//struct BezierCurve {
	//
	//	ImVec2* startPos;
	//	uint8_t startMoving : 1;
	//
	//	ImVec2* midPos;
	//	uint8_t midMoving : 1;
	//
	//	ImVec2* endPos;
	//	uint8_t endMoving : 1;
	//
	//	ImRect boundRect;
	//
	//	BezierCurve(ImVec2* startPos, ImVec2* centerPos, ImVec2* endPos) :
	//		startPos(startPos),
	//		startMoving(false),
	//		midPos(centerPos),
	//		midMoving(false),
	//		endPos(endPos),
	//		endMoving(false) {
	//	};
	//
	//	ImVec2 getRelPosAtT(float t);
	//};
	//IMGUI_API bool BezierEditor(const char* label, std::vector<ImGui::BezierCurve>* curves, std::unordered_set<ImVec2>* points);

	IMGUI_API void cBeginChild(const char* label, const ImVec2& sizeArg = ImVec2(0.0f, 0.0f), const char* icon = nullptr);
	IMGUI_API void cEndChild();
	
	IMGUI_API bool TabButton(const char* label, const char* icon = nullptr, bool* state = nullptr);
	IMGUI_API bool TopBarTabs(const std::vector<std::string>& tabNames, std::string* currentSelected, const std::unordered_map<std::string, const char*>& tabIcons = {});
	
	struct ComboOption {
		const char* name;
		const char* description;
		const char* icon;
	};
	IMGUI_API bool ComboBox(const char* label, const std::vector<ComboOption>& options, int* selectedIndex);
	IMGUI_API bool ComboButton(const char* label, const char* description, const char* icon);

	IMGUI_API bool ChildContentSlider(std::function<void()> currentContent, std::function<void()> nextContent, int direction);
}

#endif CUSTOMWIDGETS_HPP