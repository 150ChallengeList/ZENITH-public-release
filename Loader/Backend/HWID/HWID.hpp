#ifndef HWID_HPP
#define HWID_HPP

#include <map>
#include <string>

class HWID {
private:

	std::string hashVal(const std::string& inVal);

public:

	std::map<std::string, std::string> hwids;

	bool retrieveHWIDs();

};

#endif HWID_HPP