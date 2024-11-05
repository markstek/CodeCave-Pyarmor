#pragma once
#include <windows.h>
#include <string>
#include <vector>
#include <set>

// Pyarmors constants
#define MEM_PAGE_READONLY           0x02
#define MEM_PAGE_READWRITE         0x04
#define MEM_PAGE_WRITECOPY         0x08
#define MEM_PAGE_EXECUTE_READ      0x20
#define MEM_PAGE_EXECUTE_READWRITE 0x40

class CodecaveDumper {
private:
    static const DWORD READABLE_PROTECTIONS[];
    HANDLE getHandle(DWORD pid);
    std::vector<char> readMemory(HANDLE handle, LPVOID address, SIZE_T size);
    bool isReadable(DWORD protect);
    std::vector<std::string> extractData(const std::vector<char>& data, size_t minLength = 4);

public:
    void dump(DWORD pid);
};