#ifndef SCRIPTS_HPP
#define SCRIPTS_HPP

#include <ClassManager/ClassManager.hpp>
#include <sol/sol.hpp>
#include "thread"
#include "atomic"

class Scripts : public BaseClass {
public:

	sol::state lua;

	std::thread scriptThread;
	std::atomic<bool> stopThread;

	bool init(void* state) override;

	//void update(void* state) override;

	void renderMenu(void* state) override;
	void cleanup();
};

#endif SCRIPTS_HPP