#ifndef WINDOWMANAGER_TELEMETRY_HPP
#define WINDOWMANAGER_TELEMETRY_HPP

#include "BaseLib.hpp"
#include <Windows.h>
#include <TlHelp32.h>
#include <psapi.h>
#include <thread>

class WindowManager_Telemetry : public BaseLib {
private:

    time_t lastMeassureTime = time(nullptr);

    FILETIME prevSysKernel = {}, prevSysUser = {}, prevProcKernel = {}, prevProcUser = {};
    HANDLE processHandle = NULL;
    bool getCPUUsage() {
        FILETIME sysIdle, sysKernel, sysUser, procCreation, procExit, procKernel, procUser;

        if (!GetSystemTimes(&sysIdle, &sysKernel, &sysUser))
            return false;

        if (!GetProcessTimes(this->processHandle, &procCreation, &procExit, &procKernel, &procUser))
            return false;

        ULARGE_INTEGER sysKernelDiff, sysUserDiff, procKernelDiff, procUserDiff;

        sysKernelDiff.QuadPart = (reinterpret_cast<ULARGE_INTEGER&>(sysKernel).QuadPart - reinterpret_cast<ULARGE_INTEGER&>(this->prevSysKernel).QuadPart);
        sysUserDiff.QuadPart = (reinterpret_cast<ULARGE_INTEGER&>(sysUser).QuadPart - reinterpret_cast<ULARGE_INTEGER&>(this->prevSysUser).QuadPart);

        procKernelDiff.QuadPart = (reinterpret_cast<ULARGE_INTEGER&>(procKernel).QuadPart - reinterpret_cast<ULARGE_INTEGER&>(this->prevProcKernel).QuadPart);
        procUserDiff.QuadPart = (reinterpret_cast<ULARGE_INTEGER&>(procUser).QuadPart - reinterpret_cast<ULARGE_INTEGER&>(this->prevProcUser).QuadPart);

        this->prevSysKernel = sysKernel;
        this->prevSysUser = sysUser;
        this->prevProcKernel = procKernel;
        this->prevProcUser = procUser;

        ULONGLONG sysTotal = sysKernelDiff.QuadPart + sysUserDiff.QuadPart;
        ULONGLONG procTotal = procKernelDiff.QuadPart + procUserDiff.QuadPart;

        if (sysTotal == 0)
            return 0.0;

        this->cpuUsage = (100.0 * procTotal) / sysTotal;

        return true;
    }

    bool getRamUsage() {
        PROCESS_MEMORY_COUNTERS_EX pmc;

        if (!GetProcessMemoryInfo(this->processHandle, reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&pmc), sizeof(pmc)))
            return false;

        this->ramUsage = (static_cast<double>(pmc.PrivateUsage) / 1024.0) / 1024.0;
        return true;
    }

    ULONGLONG prevReadBytes = 0, prevWriteBytes = 0;
    bool getDiskUsage() {
        IO_COUNTERS ioCounters;

        if (!GetProcessIoCounters(this->processHandle, &ioCounters))
            return false;

        ULONGLONG readBytes = ioCounters.ReadTransferCount;
        ULONGLONG writeBytes = ioCounters.WriteTransferCount;

        ULONGLONG readDelta = readBytes - this->prevReadBytes;
        ULONGLONG writeDelta = writeBytes - this->prevWriteBytes;

        this->prevReadBytes = readBytes;
        this->prevWriteBytes = writeBytes;

        this->diskRead = (static_cast<double>(readDelta) / 1024.0) / 1024.0;
        this->diskWrite = (static_cast<double>(writeDelta) / 1024.0) / 1024.0;

        return true;
    }

    bool getThreadCount() {

        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
        if (snapshot == INVALID_HANDLE_VALUE)
            return false;

        THREADENTRY32 threadEntry;
        threadEntry.dwSize = sizeof(THREADENTRY32);
        if (!Thread32First(snapshot, &threadEntry)) {
            CloseHandle(snapshot);
            return false;
        }

        int threadCount = 0;
        DWORD currentPID = GetCurrentProcessId();

        this->threads = 0;

        do {
            if (threadEntry.th32OwnerProcessID != currentPID)
                continue;
            this->threads++;
        } while (Thread32Next(snapshot, &threadEntry));

        CloseHandle(snapshot);

        return true;
    }

    void update() {
        this->getCPUUsage();
        this->getRamUsage();
        this->getDiskUsage();
        this->getThreadCount();
    }

public:

    inline static double cpuUsage = 0.0;
    inline static double ramUsage = 0.0;
    inline static double diskRead = 0.0;
    inline static double diskWrite = 0.0;
    inline static int threads = 0;

    void onInit(GLFWwindow* window) override {
        this->processHandle = GetCurrentProcess();

        GetSystemTimes(nullptr, &this->prevSysKernel, &this->prevSysUser);

        FILETIME creationTime = {}, exitTime = {};
        GetProcessTimes(this->processHandle, &creationTime, &exitTime, &this->prevProcKernel, &this->prevProcUser);
    }

    void onBegin() override {

        time_t curTime = time(nullptr);
        if ((curTime - this->lastMeassureTime) <= 0)
            return;
        this->lastMeassureTime = curTime;

        // 1 second old
        printf("CPU %.02f RAM %.02f DISKR %.02f DISKW %.02f THREADS %d\n",
            WindowManager_Telemetry::cpuUsage,
            WindowManager_Telemetry::ramUsage,
            WindowManager_Telemetry::diskRead,
            WindowManager_Telemetry::diskWrite,
            WindowManager_Telemetry::threads);

        // Dispatch thread to update data
        std::thread thread(&WindowManager_Telemetry::update, this);
        thread.detach();
    }

};

#endif WINDOWMANAGER_TELEMETRY_HPP