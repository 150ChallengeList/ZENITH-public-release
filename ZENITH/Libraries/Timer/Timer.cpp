#include "Timer.hpp"
#include <iostream>

Timer timer;

void Timer::start(const std::string& name) {
	this->timers[name] = std::chrono::high_resolution_clock::now();
}

void Timer::stop(const std::string& name) { 
	
	const auto& found = this->timers.find(name);
	if (found == this->timers.end())
		return;

	bool isMicro = false;
	long long duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - found->second).count();
	if (duration < 2) {
		duration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - found->second).count();
		isMicro = true;
	}

	if (isMicro)
		std::cout << "'" << name << "' took 0." << duration << " milliseconds\n";
	else
		std::cout << "'" << name << "' took " << duration << " milliseconds\n";

	this->timers.erase(found);
}