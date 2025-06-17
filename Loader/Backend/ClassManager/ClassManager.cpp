#include "../../Libraries/ImGui/imgui.h"
#include "ClassManager.hpp"
#include <chrono>

ClassManager classManager = ClassManager();

void ClassManager::setState(void* newState) {
	this->state = newState;
}

const std::vector<std::string>& ClassManager::getTabNames() {
	return this->tabNames;
}

bool ClassManager::init() {
	for (auto& lib : this->libraryPtrs) {
		if (lib->init(this->state))
			continue;
		//return false;
	}
	return true;
}

void ClassManager::deInit() {
	for (auto& lib : this->libraryPtrs)
		lib->deInit(this->state);
}

void ClassManager::update() {

	static std::chrono::steady_clock::time_point lastTime = std::chrono::high_resolution_clock::now();
	std::chrono::steady_clock::time_point curTime = std::chrono::high_resolution_clock::now();
	long long dt = std::chrono::duration_cast<std::chrono::nanoseconds>(curTime - lastTime).count();
	lastTime = curTime;
	static long long accumulate = 0;

	for (auto& lib : this->libraryPtrs)
		lib->update(this->state);

	accumulate += dt;
	if (accumulate < 1000000000 / this->onTickUps)
		return;

	for (auto& lib : this->libraryPtrs)
		lib->tick(this->state);

	accumulate = 0;

}

void ClassManager::render() {
	for (auto& lib : this->libraryPtrs)
		lib->renderHud(this->state);
}

void ClassManager::renderTab(const std::string& tabName) {

	const auto& foundTab = this->tabs.find(tabName);
	if (foundTab == this->tabs.end())
		return;

	// We expect to be inside a window
	// We also expect each child to be 1/2 of the entire width
	float windowWidth = ImGui::GetWindowWidth(); // This is windowsize - 2x windowpadding.x (30 total)
	ImGuiStyle* style = &ImGui::GetStyle();

	ImVec2 leftColumn = ImGui::GetCursorPos();
	ImVec2 rightColumn = { windowWidth / 2.0f + style->WindowPadding.x / 2.0f, 0 };
	
	for (const auto& lib : foundTab->second) {

		bool useFirstColumn = leftColumn.y <= rightColumn.y;
		if (useFirstColumn)
			ImGui::SetCursorPos(leftColumn);
		else
			ImGui::SetCursorPos(rightColumn);

		lib->renderMenu(this->state);

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 15); // Newline is too much again...

		if (useFirstColumn)
			leftColumn.y = ImGui::GetCursorPos().y;
		else
			rightColumn.y = ImGui::GetCursorPos().y;
	}
}

void ClassManager::wndProc(const HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam) {
	for (auto& lib : this->libraryPtrs)
		lib->wndProc(this->state, windowHandle, message, wParam, lParam);
}