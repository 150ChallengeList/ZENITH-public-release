#ifndef ANIMENGINE_HPP
#define ANIMENGINE_HPP

#include "../ClassManager/ClassManager.hpp"
#include "../../Libraries/ImGui/imgui.h"
#include "../../Globals/Globals.hpp"
#include <unordered_map>
#include <string>

class WidgetController {
public:
	float tClick = 0.0f;
	float tHover = 0.0f;
	
	bool _hoverState = false;
	bool _clickState = false;

	double _hoverTime = 0.0;
	double _clickTime = 0.0;

	void updateState(bool hovered, bool clicked);

	ImColor processColor(const ImColor& start, const ImColor& end, float tVal);
	ImColor processColor(const ImColor& start, const ImColor& middle, const ImColor& end, float t1, float t2);
	const float processFloat(const float start, const float end, float tVal);
};

class AnimEngine : public BaseClass {
private:

	std::unordered_map<ImGuiID, WidgetController*> controllers;

public:

	double animLength = 0.1;

	bool getController(ImGuiID id, WidgetController** controller, bool forceFind = false);

	void renderHud(void* state) override;
	void deInit(void* state) override;

};

#endif ANIMENGINE_HPP