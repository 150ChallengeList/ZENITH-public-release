#include "AnimEngine.hpp"
#include <imgui.h>

void WidgetController::updateState(bool hovered, bool clicked) {
	double curTime = ImGui::GetTime();

	if (this->_hoverState != hovered)
		this->_hoverTime = curTime;
	this->_hoverState = hovered;

	if (this->_clickState != clicked)
		this->_clickTime = curTime;
	this->_clickState = clicked;
}

ImColor WidgetController::processColor(const ImColor& start, const ImColor& end, float tVal) {
	return ImColor(
		start.Value.x * (1.0f - tVal) + end.Value.x * tVal,
		start.Value.y * (1.0f - tVal) + end.Value.y * tVal,
		start.Value.z * (1.0f - tVal) + end.Value.z * tVal,
		start.Value.w * (1.0f - tVal) + end.Value.w * tVal
	);
}
ImColor WidgetController::processColor(const ImColor& start, const ImColor& middle, const ImColor& end, float t1, float t2) {
	return ImColor(
		(start.Value.x * (1.0f - t1) + middle.Value.x * t1) * (1.0f - t2) + end.Value.x * t2,
		(start.Value.y * (1.0f - t1) + middle.Value.y * t1) * (1.0f - t2) + end.Value.y * t2,
		(start.Value.z * (1.0f - t1) + middle.Value.z * t1) * (1.0f - t2) + end.Value.z * t2,
		(start.Value.w * (1.0f - t1) + middle.Value.w * t1) * (1.0f - t2) + end.Value.w * t2
	);
}
const float WidgetController::processFloat(const float start, const float end, float tVal) {
	return start * (1.0f - tVal) + end * tVal;
}

bool AnimEngine::getController(ImGuiID id, WidgetController** controller, bool forceFind) {
	
	if (*controller != nullptr && !forceFind)
		return true;
	
	const auto& foundController = this->controllers.find(id);
	if (foundController == this->controllers.end()) {
		WidgetController* newController = new WidgetController;
		this->controllers[id] = newController;
		*controller = newController;
		return true;
	}

	*controller = foundController->second;
	return true;
}

void AnimEngine::renderHud(void* state) {

	double curTime = ImGui::GetTime();
	double animMul = 1.0f / this->animLength;

	for (auto& [label, controller] : this->controllers) {

		float newHoverState = controller->_hoverState ?
			static_cast<float>(std::min((curTime - controller->_hoverTime) * animMul, 1.0)) : // 0.0 -> 1.0
			static_cast<float>(std::max((controller->_hoverTime + this->animLength - curTime) * animMul, 0.0));

		controller->tHover = newHoverState;

		float newClickState = controller->_clickState ?
			static_cast<float>(std::min((curTime - controller->_clickTime) * animMul, 1.0)) : // 0.0 -> 1.0
			static_cast<float>(std::max((controller->_clickTime + this->animLength - curTime) * animMul, 0.0)); // 1.0 -> 0.0
		
		controller->tClick = newClickState;
	}

}

void AnimEngine::deInit(void* state) {
	for (auto& [label, controller] : this->controllers)
		delete controller;
	this->controllers.clear();
}