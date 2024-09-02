#include <windows.h>
#include <tlhelp32.h>
#include <string>
#include <iostream>
#include <conio.h>

DWORD GetProcId(const wchar_t* procName) {
    DWORD procId = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32 procEntry;
        procEntry.dwSize = sizeof(procEntry);

        if (Process32First(hSnap, &procEntry)) {
            do {
                if (!_wcsicmp(procEntry.szExeFile, procName)) {
                    procId = procEntry.th32ProcessID;
                    break;
                }
            } while (Process32Next(hSnap, &procEntry));
        }
    }
    CloseHandle(hSnap);
    return procId;
}

void InjectDLL(DWORD processID, const std::wstring& dllPath) {
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
    void* location = VirtualAllocEx(hProcess, 0, (dllPath.size() + 1) * sizeof(wchar_t), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    WriteProcessMemory(hProcess, location, dllPath.c_str(), (dllPath.size() + 1) * sizeof(wchar_t), 0);
    HANDLE hThread = CreateRemoteThread(hProcess, 0, 0, (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(L"Kernel32.dll"), "LoadLibraryW"), location, 0, 0);
    WaitForSingleObject(hThread, INFINITE);
    VirtualFreeEx(hProcess, location, (dllPath.size() + 1) * sizeof(wchar_t), MEM_RELEASE);
    CloseHandle(hThread);
    CloseHandle(hProcess);
}
// ignore ctrl c
void signalHandler(int signum) {
}
//inject slinky dlls
void executeSlinky() {
    std::cout << L"injecting slinky" << std::endl;
    DWORD processID = GetProcId(L"javaw.exe");

    wchar_t exePath[MAX_PATH];
    GetModuleFileName(NULL, exePath, MAX_PATH);
    std::wstring basePath(exePath);
    basePath = basePath.substr(0, basePath.find_last_of(L"\\/"));

    const wchar_t* dllNames[] = { L"slinky_library.dll", L"slinkyhook.dll" };
    std::wstring dllPath = basePath + L"\\" + dllNames[0];
    InjectDLL(processID, dllPath);
    for (int i = 1; i < sizeof(dllNames) / sizeof(dllNames[0]); i++) {
        dllPath = basePath + L"\\" + dllNames[i];
        InjectDLL(processID, dllPath);
    }
    std::wcout << L"injected" << std::endl;
    Sleep(1000);
}
// inject raid0.dll
void executeRaid0() {
    std::cout << L"injecting raid0" << std::endl;
    DWORD processID = GetProcId(L"javaw.exe");

    wchar_t exePath[MAX_PATH];
    GetModuleFileName(NULL, exePath, MAX_PATH);
    std::wstring basePath(exePath);
    basePath = basePath.substr(0, basePath.find_last_of(L"\\/"));

    const wchar_t* dllNames[] = { L"internal.dll" };
    std::wstring dllPath = basePath + L"\\" + dllNames[0];
    InjectDLL(processID, dllPath);
    for (int i = 1; i < sizeof(dllNames) / sizeof(dllNames[0]); i++) {
        dllPath = basePath + L"\\" + dllNames[i];
        InjectDLL(processID, dllPath);
    }
    std::wcout << L"injected" << std::endl;
    Sleep(1000);
}

void executeCMDDll() {
    std::cout << L"injecting cmd.dll" << std::endl;
    DWORD processID = GetProcId(L"javaw.exe");

    wchar_t exePath[MAX_PATH];
    GetModuleFileName(NULL, exePath, MAX_PATH);
    std::wstring basePath(exePath);
    basePath = basePath.substr(0, basePath.find_last_of(L"\\/"));

    const wchar_t* dllNames[] = { L"cmd.dll" };
    std::wstring dllPath = basePath + L"\\" + dllNames[0];
    InjectDLL(processID, dllPath);
    for (int i = 1; i < sizeof(dllNames) / sizeof(dllNames[0]); i++) {
        dllPath = basePath + L"\\" + dllNames[i];
        InjectDLL(processID, dllPath);
    }
    std::wcout << L"injected" << std::endl;
    Sleep(1000);
}


int main() {
    SetConsoleTitle(TEXT("Command Prompt"));

    std::cout << "Microsoft Windows [Version 10.0.19045.2965]" << std::endl;
    std::cout << "(c) Microsoft Corporation. All rights reserved." << std::endl;
    std::cout << "" << std::endl;
    // getting username
    while (true) {
        std::string input;
        char* username = nullptr;
        size_t len = 0;

        if (_dupenv_s(&username, &len, "USERNAME") == 0 && username != nullptr) {
            std::cout << "C:\\Users\\" << username << ">";
        }
        else {
            std::cout << "C:\\Users\\Administrator>";
        }

        std::getline(std::cin, input);
        // inject commands
        if (input == "slinky") {
            executeSlinky();
        }
        else {
            if (input == "raid0") {
                executeRaid0();
            }
            else {
                if (input == "cmd.dll") {
                    executeCMDDll();
                }
                else {
                    system(input.c_str());
                }
            }
        }

        free(username);  
    }

    return 0;
}