#include "ClassManager.hpp"

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

		// Find the 
		auto it = std::find_if(this->libraryInstances.begin(), this->libraryInstances.end(), [lib](const auto& pair) {
			return pair.second.get() == lib;
		});

		if (it == this->libraryInstances.end())
			printf("[ClassManager] Failed to find failing library\n");
		else
			printf("[ClassManager] Library '%s' failed to initialize\n", it->first.c_str());

		return false;
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
	// We also expect each child to be 1/2 of the entire width#
	ImGuiStyle* style = &ImGui::GetStyle();
	const float bottomContentWidth = ImGui::GetWindowWidth();
	const float halfSize = bottomContentWidth / 2.0f + style->FramePadding.x * 2.0f - 15.0f;

	foundTab->second.front()->renderMenu(this->state);
}

void ClassManager::wndProc(const HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam) {
	for (auto& lib : this->libraryPtrs)
		lib->wndProc(this->state, windowHandle, message, wParam, lParam);
}