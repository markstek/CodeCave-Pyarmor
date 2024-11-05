#include "codecave_dumper.h"
#include <iostream>
#include <fstream>
#include <regex>
#include <algorithm>

const DWORD CodecaveDumper::READABLE_PROTECTIONS[] = {
    MEM_PAGE_READONLY,
    MEM_PAGE_READWRITE, 
    MEM_PAGE_WRITECOPY,
    MEM_PAGE_EXECUTE_READ,
    MEM_PAGE_EXECUTE_READWRITE
};

HANDLE CodecaveDumper::getHandle(DWORD pid) {
    return OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
}

std::vector<char> CodecaveDumper::readMemory(HANDLE handle, LPVOID address, SIZE_T size) {
    std::vector<char> buffer(size);
    SIZE_T bytesRead;
    
    if (!ReadProcessMemory(handle, address, buffer.data(), size, &bytesRead)) {
        throw std::runtime_error("Failed to read process memory");
    }
    
    buffer.resize(bytesRead);
    return buffer;
}

bool CodecaveDumper::isReadable(DWORD protect) {
    const size_t size = sizeof(READABLE_PROTECTIONS) / sizeof(READABLE_PROTECTIONS[0]);
    return std::find(READABLE_PROTECTIONS, 
                    READABLE_PROTECTIONS + size,
                    protect) != READABLE_PROTECTIONS + size;
}

std::vector<std::string> CodecaveDumper::extractData(const std::vector<char>& data, size_t minLength) {
    static const auto junkPattern = std::regex(R"(^[0-9A-F]+$|^[A-Z]{1,3}\d+$|^\W+$|^.{1,3}$|(.)\1{3,})", 
                                             std::regex::optimize);
    
    std::vector<std::string> strings;
    strings.reserve(1000);
    
    constexpr size_t CHUNK_SIZE = 4096;
    std::string current;
    current.reserve(256);

    for (size_t i = 0; i < data.size(); i += CHUNK_SIZE) {
        const size_t chunk_end = (i + CHUNK_SIZE < data.size()) ? i + CHUNK_SIZE : data.size();
        for (char byte : data) {
            if (isprint(byte)) {
                current += byte;
            } else if (!current.empty()) {
                if (current.length() >= minLength && 
                    !std::regex_match(current, junkPattern) &&
                    std::any_of(current.begin(), current.end(), ::isalnum)) {
                    strings.push_back(std::move(current));
                }
                current.clear();
                current.reserve(256);
            }
        }
    }

    if (current.length() >= minLength && 
        !std::regex_match(current, junkPattern) &&
        std::any_of(current.begin(), current.end(), ::isalnum)) {
        strings.push_back(std::move(current));
    }

    return strings;
}

void CodecaveDumper::dump(DWORD pid) {
    try {
        HANDLE handle = getHandle(pid);
        if (handle == NULL) {
            throw std::runtime_error("Failed to open process");
        }

        std::cout << "\nStarting codecave dump for PID: " << pid << std::endl;

        std::string outputFile = std::to_string(pid) + "_dump.txt";
        LPVOID address = nullptr;
        MEMORY_BASIC_INFORMATION memInfo;
        std::set<std::string> uniqueStrings;
        size_t regionsAnalyzed = 0;
        size_t totalRegions = 0;

        while (VirtualQueryEx(handle, address, &memInfo, sizeof(memInfo))) {
            totalRegions++;
            if (isReadable(memInfo.Protect)) {
                try {
                    auto chunk = readMemory(handle, memInfo.BaseAddress, memInfo.RegionSize);
                    auto strings = extractData(chunk);
                    uniqueStrings.insert(std::make_move_iterator(strings.begin()), 
                                       std::make_move_iterator(strings.end()));
                    regionsAnalyzed++;
                }
                catch (const std::exception&) {
                    // Skip failed regions
                }
            }
            address = (LPVOID)((DWORD_PTR)memInfo.BaseAddress + memInfo.RegionSize);
        }

        std::ofstream outFile(outputFile, std::ios::binary);
        for (const auto& str : uniqueStrings) {
            outFile << str << '\n';
        }

        std::cout << "\nDump complete!" << std::endl;
        std::cout << "Analyzed " << regionsAnalyzed << "/" << totalRegions << " regions" << std::endl;
        std::cout << "Found " << uniqueStrings.size() << " unique strings" << std::endl;
        std::cout << "Results saved to: " << outputFile << std::endl;

        CloseHandle(handle);
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}