#ifndef TIMER_HPP
#define TIMER_HPP

#include <string>
#include <unordered_map>
#include <chrono>

class Timer {
public:

	void start(const std::string& name);
	void stop(const std::string& name);

private:

	std::unordered_map<std::string, std::chrono::steady_clock::time_point> timers;

} extern timer;


#endif TIMER_HPP