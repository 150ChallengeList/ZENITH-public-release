#ifndef BASECLASS_HPP
#define BASECLASS_HPP

#include <JSON/json.hpp>
#include <functional>
#include <windows.h>
#include <string>

class BaseClass {
public:

	std::unordered_map<std::string, std::function<bool(void*)>> luaFns;

	/// <summary>
	/// This gets called when then ClassManager initializes
	/// </summary>
	/// <param name="state"> -> data set by ClassManager::setState</param>
	/// /// <returns>Wether the module initialized successfully</returns>
	virtual bool init(void* state) { return true; }

	/// <summary>
	/// This gets called when then ClassManager deinitializes
	/// </summary>
	/// <param name="state"> -> data set by ClassManager::setState</param>
	virtual void deInit(void* state) {}

	/// <summary>
	/// This gets called every frame in the render loop
	/// </summary>
	/// <param name="state"> -> data set by ClassManager::setState</param>
	virtual void renderHud(void* state) {}
	
	/// <summary>
	/// This gets called every frame in the render loop if menu is visible
	/// </summary>
	/// <param name="state"> -> data set by ClassManager::setState</param>
	virtual void renderMenu(void* state) {}

	/// <summary>
	/// This gets called by the while loop
	/// </summary>
	/// <param name="state"> -> data set by ClassManager::setState</param>
	virtual void update(void* state) {};

	/// <summary>
	/// This gets called in the while loop at a lower interval
	/// </summary>
	/// <param name="state"> -> data set by ClassManager::setState</param>
	virtual void tick(void* state) {};

	/// <summary>
	/// This gets called in the window procedure to intercept events
	/// </summary>
	/// <param name="state"> -> data set by ClassManager::setState</param>
	/// <param name="windowHandle"></param>
	/// <param name="message"></param>
	/// <param name="wParam"></param>
	/// <param name="lParam"></param>
	/// <returns></returns>
	virtual LRESULT wndProc(void* state, const HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam) { return 0; }

	/// <summary>
	/// This gets called when the cheat is saving the config
	/// </summary>
	/// <param name="state"> -> data set by ClassManager::setState</param>
	/// <param name="cfg"> -> ptr to json obj</param>
	virtual void saveConfig(void* state, nlohmann::json* cfg) {};

	/// <summary>
	/// This gets called when the cheat is loading the config
	/// </summary>
	/// <param name="state"> -> data set by ClassManager::setState</param>
	/// <param name="cfg"> -> address to json obj</param>
	virtual void loadConfig(void* state, const nlohmann::json& cfg) {};
};

#endif BASECLASS_HPP