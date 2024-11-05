#include "codecave_dumper_cli.h"
#include <iostream>
#include <tlhelp32.h>

void CodecaveDumperCLI::listProcesses() {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) return;

    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(pe32);

    std::cout << "\nAvailable Python processes:" << std::endl;
    if (Process32FirstW(snapshot, &pe32)) {
        do {
            std::wstring processName = pe32.szExeFile;
            if (processName.find(L"python") != std::wstring::npos) {
                std::wcout << pe32.th32ProcessID << L"\t" << processName << std::endl;
            }
        } while (Process32NextW(snapshot, &pe32));
    }

    CloseHandle(snapshot);
}

void CodecaveDumperCLI::run() {
    while (true) {
        std::cout << "Codecave Hook By Libalpm Program Dumper" << std::endl;
        std::cout << "\nMenu Options:" << std::endl;
        std::cout << "1. List Python Processes" << std::endl;
        std::cout << "2. Dump Process Memory" << std::endl;
        std::cout << "3. Exit" << std::endl;

        std::string choice;
        std::cout << "\nEnter your choice (1-3): ";
        std::getline(std::cin, choice);

        if (choice == "1") {
            listProcesses();
        }
        else if (choice == "2") {
            std::cout << "Enter PID to dump: ";
            std::string pidStr;
            std::getline(std::cin, pidStr);
            try {
                DWORD pid = std::stoul(pidStr);
                dumper.dump(pid);
            }
            catch (const std::exception&) {
                std::cerr << "Error: Please enter a valid PID" << std::endl;
            }
        }
        else if (choice == "3") {
            std::cout << "\nGoodbye!" << std::endl;
            break;
        }
        else {
            std::cout << "Invalid choice. Please try again." << std::endl;
        }
    }
}