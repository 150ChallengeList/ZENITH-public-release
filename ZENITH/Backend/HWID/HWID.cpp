#include "HWID.hpp"
#include <comdef.h>
#include <Wbemidl.h>
#pragma comment(lib, "wbemuuid.lib")
#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>

void PrintWMIClassProperties(const std::string& wmiClass) {
    HRESULT result = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (FAILED(result)) 
        return;

    result = CoInitializeSecurity(NULL, -1, NULL, NULL,
        RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE,
        NULL, EOAC_NONE, NULL);
    if (FAILED(result)) {
        CoUninitialize();
        return;
    }

    IWbemLocator* locator = nullptr;
    result = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER,
        IID_IWbemLocator, (LPVOID*)&locator);
    if (FAILED(result)) {
        CoUninitialize();
        return;
    }

    IWbemServices* services = nullptr;
    result = locator->ConnectServer(_bstr_t(L"ROOT\\CIMV2"), nullptr, nullptr, 0,
        NULL, 0, 0, &services);
    if (FAILED(result)) {
        locator->Release(); CoUninitialize(); return;
    }

    result = CoSetProxyBlanket(services, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
        RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE,
        NULL, EOAC_NONE);
    if (FAILED(result)) {
        services->Release(); locator->Release(); CoUninitialize(); return;
    }

    IEnumWbemClassObject* enumerator = nullptr;
    result = services->ExecQuery(
        bstr_t("WQL"),
        bstr_t(("SELECT * FROM " + wmiClass).c_str()),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        NULL, &enumerator);
    if (FAILED(result)) {
        services->Release(); locator->Release(); CoUninitialize(); return;
    }

    IWbemClassObject* classObject = nullptr;
    ULONG returnValue = 0;
    while (enumerator->Next(WBEM_INFINITE, 1, &classObject, &returnValue) == WBEM_S_NO_ERROR) {
        std::wcout << L"--- Object ---\n";
        result = classObject->BeginEnumeration(WBEM_FLAG_NONSYSTEM_ONLY);
        if (FAILED(result)) {
            classObject->Release(); break;
        }

        BSTR propName = nullptr;
        VARIANT var;
        while (classObject->Next(0, &propName, &var, nullptr, nullptr) == WBEM_S_NO_ERROR) {
            std::wcout << L"> " << propName << L" = ";

            if (var.vt == VT_BSTR && var.bstrVal) {
                std::wcout << var.bstrVal;
            }
            else if (var.vt == VT_UINT || var.vt == VT_UI4) {
                std::wcout << var.uintVal;
            }
            else if (var.vt == VT_I4) {
                std::wcout << var.intVal;
            }
            else if (var.vt == VT_BOOL) {
                std::wcout << (var.boolVal ? L"true" : L"false");
            }
            else {
                std::wcout << L"[Unsupported type: " << var.vt << L"]";
            }

            std::wcout << std::endl;
            VariantClear(&var);
            SysFreeString(propName);
        }

        classObject->Release();
    }

    enumerator->Release();
    services->Release();
    locator->Release();
    CoUninitialize();
}

std::string HWID::hashVal(const std::string& inVal) {
    uint32_t digest[8] = {
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xbe4ff53a,
        0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x50accd19
    };

    const uint32_t K[64] = {
        0x21370680, // Bee movie1
        0x116ceec0, // Shrek movie2
        0x11ab5a00, // Cars3
        0x1f246300, // Website of fightclub movie4
        0x1ea34703, // Cisko thml in hex5
        0x81a7e0c5, // deine hose ist nass 6
        0xce733073, // chatgpt website von text zu hexcode 7
        0xb00bf00d, // spooky ghost hacker vibes 8
        0xdeadbeef, // classic debug marker 9
        0x0badc0de, // when code goes rogue 10
        0xcafebabe, // Java class files start like this 11
        0xfaceb00c, // Facebook with a twist 12
        0x1337c0de, // elite code detected 13
        0x600d1dea, // good idea, maybe? 14
        0xf005ba11, // fools ball! (foosball) 15
        0x12345678, // yes 16
        0x8badf00d, // bad food 17
        0xd15ea5e0, // disease-o 18
        0xc0ffee00, // coffee overflow 19
        0xbaaaaaad, // baaahd RAM 20
        0xfeedf00d, // dinner's served 21
        0x0d15ea5e, // "disease" but cooler 22
        0xfadebabe, // fading babe 23
        0xa11dead0, // all dead-o 24
        0xfee1dead, // feel dead 25
        0x0ff1ce00, // office space 26
        0xba5eba11, // baseball fan 27
        0xdeadc0de, // zombie code 28
        0xabad1dea, // a bad idea™ 29
        0xfacade00, // mask on 30
        0xdeafbeef, // can’t hear your steak 31
        0xbeefcace, // meat in your face 32
        0xabcddcba, // mirrored alphabet 33
        0x600dc0de, // good code 34
        0x10aded00, // totally rad 35
        0xc01df00d, // cold food :( 36
        0x900dbeef, // good beef 37
        0xbeadedfa, // jewelry nightmare 38
        0xbed0babe, // bedtime baby 39
        0xdeadfa11, // deadfall 40
        0xc01a1ab5, // cola labs? 41
        0xf1ee1eaf, // flee leaf 42
        0xd0d0caca, // bathroom emergency 43
        0x404b00b5, // not found but funny 44
        0xabcafeed, // alphabet snack 45
        0xdead2bad, // bad death 46
        0x777b00b5, // jackpot! 47
        0x5ca1ab1e, // scalable! 48
        0xdefec8ed, // oh no 49
        0xfac3c0de, // facecode 50
        0xd15ea5ed, // diseased 51
        0xbaaaaaaa, // sheep attack 52
        0xf0e1f0e1, // echo of itself 53
        0xfadefade, // double fade 54
        0xc0deba5e, // code base 55
        0x50fababe, // sofa babe 56
        0xbabeface, // beauty mode on 57
        0xa5a5a5a5, // zebra pattern 58
        0xbeefdead, // well-cooked demise 59
        0x7e57e57e, // testy 60
        0xf007ba11, // football again 61
        0xca11ab1e, // callable 62
        0xd0ff1ce0, // office 63
        0xbadc0ffe, // bad coffee day 64
    };
    auto rightRotate = [](uint32_t value, uint32_t bits) {
        return (value >> bits) | (value << (32 - bits));
    };

    size_t bitLength = inVal.size() * 8;
    std::vector<unsigned char> paddedData = std::vector<unsigned char>(inVal.begin(), inVal.end());
    paddedData.push_back(0x80);

    while ((paddedData.size() * 8) % 512 != 448)
        paddedData.push_back(0x00);

    for (int i = 7; i >= 0; --i)
        paddedData.push_back((bitLength >> (i * 8)) & 0xFF);

    for (size_t i = 0; i < paddedData.size(); i += 64) {
        uint32_t W[64];

        for (int j = 0; j < 16; ++j) {
            W[j] = (paddedData[i + j * 4] << 24) |
                (paddedData[i + j * 4 + 1] << 16) |
                (paddedData[i + j * 4 + 2] << 8) |
                paddedData[i + j * 4 + 3];
        }

        for (int j = 16; j < 64; ++j) {
            uint32_t s0 = rightRotate(W[j - 15], 7) ^ rightRotate(W[j - 15], 18) ^ (W[j - 15] >> 3);
            uint32_t s1 = rightRotate(W[j - 2], 17) ^ rightRotate(W[j - 2], 19) ^ (W[j - 2] >> 10);
            W[j] = W[j - 16] + s0 + W[j - 7] + s1;
        }

        uint32_t a = digest[0];
        uint32_t b = digest[1];
        uint32_t c = digest[2];
        uint32_t d = digest[3];
        uint32_t e = digest[4];
        uint32_t f = digest[5];
        uint32_t g = digest[6];
        uint32_t h = digest[7];

        for (int j = 0; j < 64; ++j) {
            uint32_t S1 = rightRotate(e, 6) ^ rightRotate(e, 11) ^ rightRotate(e, 25);
            uint32_t ch = (e & f) ^ (~e & g);
            uint32_t temp1 = h + S1 + ch + K[j] + W[j];
            uint32_t S0 = rightRotate(a, 2) ^ rightRotate(a, 13) ^ rightRotate(a, 22);
            uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
            uint32_t temp2 = S0 + maj;

            h = g;
            g = f;
            f = e;
            e = d + temp1;
            d = c;
            c = b;
            b = a;
            a = temp1 + temp2;
        }

        digest[0] += a;
        digest[1] += b;
        digest[2] += c;
        digest[3] += d;
        digest[4] += e;
        digest[5] += f;
        digest[6] += g;
        digest[7] += h;
    }

    std::stringstream ss;
    for (int i = 0; i < 8; ++i)
        ss << std::setw(8) << std::setfill('0') << std::hex << digest[i];

    return ss.str();
}

bool HWID::retrieveHWIDs() {

    auto QueryWMIAll = [this](const std::string& wmiClass, const std::string& property, const std::string& name, std::map<std::string, std::string>* outList) -> bool {
        HRESULT result = CoInitializeEx(0, COINIT_MULTITHREADED);
        if (FAILED(result)) 
            return false;

        result = CoInitializeSecurity(NULL, -1, NULL, NULL,
            RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE,
            NULL, EOAC_NONE, NULL);
        if (FAILED(result)) {
            CoUninitialize();
            return false;
        }

        IWbemLocator* locator = nullptr;
        result = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER,
            IID_IWbemLocator, (LPVOID*)&locator);
        if (FAILED(result)) {
            CoUninitialize();
            return false;
        }

        IWbemServices* services = nullptr;
        result = locator->ConnectServer(_bstr_t(L"ROOT\\CIMV2"), nullptr, nullptr, 0,
            NULL, 0, 0, &services);
        if (FAILED(result)) {
            locator->Release(); 
            CoUninitialize(); 
            return false;
        }

        result = CoSetProxyBlanket(services, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
            RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE,
            NULL, EOAC_NONE);
        if (FAILED(result)) {
            services->Release(); 
            locator->Release(); 
            CoUninitialize();
            return false;
        }

        IEnumWbemClassObject* enumerator = nullptr;
        std::string query = "SELECT " + property + " FROM " + wmiClass;
        result = services->ExecQuery(
            bstr_t("WQL"), bstr_t(query.c_str()),
            WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
            NULL, &enumerator);
        if (FAILED(result)) {
            services->Release(); 
            locator->Release(); 
            CoUninitialize();
            return false;
        }

        IWbemClassObject* classObject = nullptr;
        ULONG returnValue = 0;
        std::wstring wProp(property.begin(), property.end());

        int index = -1;
        while (enumerator) {
            index++;
            result = enumerator->Next(WBEM_INFINITE, 1, &classObject, &returnValue);
            if (FAILED(result) || returnValue == 0) 
                break;

            VARIANT vtProp;
            VariantInit(&vtProp);
            result = classObject->Get(wProp.c_str(), 0, &vtProp, 0, 0);

            if (SUCCEEDED(result) && vtProp.vt == VT_BSTR && vtProp.bstrVal) {
                int len = WideCharToMultiByte(CP_UTF8, 0, vtProp.bstrVal, -1, nullptr, 0, nullptr, nullptr);
                if (len > 0) {
                    std::string val(len - 1, 0);
                    WideCharToMultiByte(CP_UTF8, 0, vtProp.bstrVal, -1, &val[0], len, nullptr, nullptr);
                    outList->insert({ name + property + std::to_string(index), this->hashVal(val)});
                }
            }

            VariantClear(&vtProp);
            classObject->Release();
        }

        enumerator->Release();
        services->Release();
        locator->Release();
        CoUninitialize();
        return true;
    };

    if (!QueryWMIAll("Win32_Processor", "Name", "cpu", &this->hwids)) return false;
    if (!QueryWMIAll("Win32_Processor", "Caption", "cpu", &this->hwids)) return false;
    if (!QueryWMIAll("Win32_Processor", "Description", "cpu", &this->hwids)) return false;
    if (!QueryWMIAll("Win32_Processor", "Manufacturer", "cpu", &this->hwids)) return false;
    if (!QueryWMIAll("Win32_Processor", "ProcessorId", "cpu", &this->hwids)) return false;
    if (!QueryWMIAll("Win32_Processor", "Revision", "cpu", &this->hwids)) return false;

    if (!QueryWMIAll("Win32_VideoController", "Caption", "gpu", &this->hwids)) return false;
    if (!QueryWMIAll("Win32_VideoController", "Description", "gpu", &this->hwids)) return false;
    if (!QueryWMIAll("Win32_VideoController", "InfFilename", "gpu", &this->hwids)) return false;
    if (!QueryWMIAll("Win32_VideoController", "InfSection", "gpu", &this->hwids)) return false;
    if (!QueryWMIAll("Win32_VideoController", "PNPDeviceID", "gpu", &this->hwids)) return false;

    if (!QueryWMIAll("Win32_DiskDrive", "Caption", "drive", &this->hwids)) return false;
    if (!QueryWMIAll("Win32_DiskDrive", "DeviceID", "drive", &this->hwids)) return false;
    if (!QueryWMIAll("Win32_DiskDrive", "FirmwareRevision", "drive", &this->hwids)) return false;
    if (!QueryWMIAll("Win32_DiskDrive", "PNPDeviceID", "drive", &this->hwids)) return false;
    if (!QueryWMIAll("Win32_DiskDrive", "SerialNumber", "drive", &this->hwids)) return false;

    if (!QueryWMIAll("Win32_BaseBoard", "Product", "motherboard", &this->hwids)) return false;
    if (!QueryWMIAll("Win32_BaseBoard", "SerialNumber", "motherboard", &this->hwids)) return false;
    if (!QueryWMIAll("Win32_BaseBoard", "Version", "motherboard", &this->hwids)) return false;

    if (!QueryWMIAll("Win32_PhysicalMemory", "PartNumber", "memory", &this->hwids)) return false;
    if (!QueryWMIAll("Win32_PhysicalMemory", "SerialNumber", "memory", &this->hwids)) return false;
    if (!QueryWMIAll("Win32_PhysicalMemory", "DeviceLocator", "memory", &this->hwids)) return false;

    return true;
}

// HWID
// Backend -> link provider, one time link press, heartbeat