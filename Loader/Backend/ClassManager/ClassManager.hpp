#ifndef CLASSMANAGER_HPP
#define CLASSMANAGER_HPP

#include <unordered_map>
#include "BaseClass.hpp"
#include <memory>

class ClassManager {
private:

	std::vector<BaseClass*> libraryPtrs;
	std::unordered_map<std::string, std::vector<BaseClass*>> tabs;
	std::vector<std::string> tabNames;
	std::unordered_map<std::string, std::unique_ptr<BaseClass>> libraryInstances;
	void* state = nullptr;

public:

	uint32_t onTickUps = 120;

	/// <summary>
	/// Sets the userdata for every class
	/// </summary>
	/// <param name="newState"> -> userdata ptr</param>
	void setState(void* newState);

	/// <summary>
	/// Adds a new backend module to the ClassManager (no menu render calls)
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="libName"> -> internal library name</param>
	/// <returns>Library instance ptr</returns>
	template <typename T>
	T* addClass(const std::string& libName) {
		BaseClass* lib = new T;
		this->libraryPtrs.push_back(lib);
		this->libraryInstances[libName] = std::unique_ptr<BaseClass>(lib);
		return reinterpret_cast<T*>(lib);
	}

	/// <summary>
	/// Adds a new module to the ClassManager
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="libName"> -> internal library name</param>
	/// <param name="tabName"> -> internal tab name</param>
	template <typename T>
	void addClass(const std::string& libName, const std::string& tabName) {
		BaseClass* lib = new T;

		this->libraryPtrs.push_back(lib);
		this->libraryInstances[libName] = std::unique_ptr<BaseClass>(lib);
		
		const auto& foundTab = this->tabs.find(tabName);
		if (foundTab != this->tabs.end()) {
			foundTab->second.push_back(lib);
			return;
		}
		
		this->tabs[tabName].push_back(lib);
		this->tabNames.emplace_back(tabName);
	}

	/// <summary>
	/// Upon providing existing module name, gives back the modulePtr
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="libName"> -> internal library name</param>
	/// <param name="lib"> -> outPtr for the library</param>
	/// <param name="forceFind"> -> if it should be retrieved everytime</param>
	/// <returns>Wheter the operation was successful</returns>
	template <typename T>
	bool getClass(const std::string& libName, T** lib, bool forceFind = false) {

		if (*lib != nullptr && !forceFind)
			return true;

		const auto& foundLib = this->libraryInstances.find(libName);
		if (foundLib == this->libraryInstances.end())
			return false;

		*lib = reinterpret_cast<T*>(foundLib->second.get());

		return true;
	}
	
	/// <summary>
	/// Returns all added tab names in order
	/// </summary>
	/// <returns></returns>
	const std::vector<std::string>& getTabNames();

	/// <summary>
	/// Calls the "init" function on all modules
	/// </summary>
	/// <returns>Whether every module successfully initialized</returns>
	bool init();

	/// <summary>
	/// Calls the "deInit" function on all modules
	/// </summary>
	void deInit();

	/// <summary>
	/// Calls the "update" function on all modules
	/// </summary>
	void update();

	/// <summary>
	/// Calls the "render" function on all modules
	/// </summary>
	void render();

	/// <summary>
	/// Calls the "renderTab" function on all modules in the tab
	/// </summary>
	/// <param name="tabName"> -> internal tab name</param>
	void renderTab(const std::string& tabName);

	/// <summary>
	/// Calls the "wndProc" function on all modules
	/// </summary>
	void wndProc(const HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);

} extern classManager;

#endif CLASSMANAGER_HPP