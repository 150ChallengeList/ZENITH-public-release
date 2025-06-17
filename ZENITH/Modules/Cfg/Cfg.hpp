#ifndef CFG_HPP
#define CFG_HPP

#include <string>
#include <unordered_map>
#include <ClassManager/BaseClass.hpp>

struct Config {
	std::string name;
	std::string description;
	time_t lastChange;
	std::string author;
};

class Cfg : public BaseClass {
private:

public:

	std::vector<Config> configs;

	bool writeToCfg(const std::string& data, const std::string& filename, char key);
	std::string readCfg(const  std::string& filename, char key);
	std::unordered_map<std::string, std::string> parseConfig(const std::string& rawConfig);

	void renderMenu(void* state) override;

};

#endif CFG_HPP