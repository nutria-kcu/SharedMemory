#include "pch.h"
#include "SharedMemory.h"
#include <iostream>

SharedMemoryHandler::SharedMemoryHandler(const std::wstring& shmName, const int isProducer) {
    SharedMemoryHandler::isProducer = isProducer;

    if (isProducer) {
        std::cout << "Shared Mem init - producer\n";
        initProducer(shmName);
    }
    else {
        std::cout << "Shared Mem init - consumer\n";
        initConsumer(shmName);
    }

    if (!hMapFile) {
        std::cerr << "Failed to open shared file\n";
        cleanup();
        exit(1);
    }

    if (!hEventEmpty || !hEventFull) {
        std::cerr << "Failed to open shared event\n";
        cleanup();
        exit(1);
    }

    shm = (SharedMemory*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SharedMemory));
}

SharedMemoryHandler::~SharedMemoryHandler() {
    cleanup();
}

bool SharedMemoryHandler::setMessage(const std::string& message) {
    if (!isProducer) {
        std::cout << "getMessage accessed from consumer\n";
        return false;
    }

    if (shm->isMessageSet) {
        std::cout << "Message already set. Waiting for consumer to read.\n";
        std::cout << "Ignore the given msg\n";
    }
    strncpy_s(shm->message, message.c_str(), sizeof(shm->message) - 1);
    shm->isMessageSet = true;
    SetEvent(hEventFull);

    return true;
}

const char* SharedMemoryHandler::getMessage() {
    if (isProducer) {
        std::cout << "getMessage accessed from producer\n";
        return nullptr;
    }

    if (!shm->isMessageSet) {
        std::cout << "No message set.\n";
        return nullptr;
    }
    shm->isMessageSet = false;
    SetEvent(hEventEmpty);
    return shm->message;
}

SharedMemory* SharedMemoryHandler::getMemory() {
    return shm;
}

void SharedMemoryHandler::resetEvent() {
    ResetEvent(hEventEmpty);
    ResetEvent(hEventFull);
}

void SharedMemoryHandler::cleanup() {
    if (shm) {
        UnmapViewOfFile(shm);
    }
    if (hMapFile) {
        CloseHandle(hMapFile);
    }
    if (hEventFull) {
        CloseHandle(hEventFull);
    }

    if (hEventEmpty) {
        CloseHandle(hEventEmpty);
    }
}

void SharedMemoryHandler::initProducer(const std::wstring& shmName) {
    hMapFile = CreateFileMapping(
        INVALID_HANDLE_VALUE,    // use paging file
        NULL,                    // default security
        PAGE_READWRITE,          // read/write access
        0,                       // maximum object size (high-order DWORD)
        sizeof(SharedMemory),                // maximum object size (low-order DWORD)
        shmName.c_str());


    //HANDLE CreateEvent(
    //    LPSECURITY_ATTRIBUTES lpEventAttributes, security stuff, set NULL
    //    BOOL                  bManualReset, False is Auto Reset
    //    BOOL                  bInitialState, TRUE if signaled 
    //    LPCTSTR               lpName
    //);
    hEventEmpty = CreateEvent(NULL, TRUE, TRUE, L"IPCEMPTY");
    hEventFull = CreateEvent(NULL, TRUE, FALSE, L"IPCFULL");
}

void SharedMemoryHandler::initConsumer(const std::wstring& shmName) {
    hMapFile =  OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, shmName.c_str());

    hEventEmpty = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"IPCEMPTY");
    hEventFull = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"IPCFULL");
}

HANDLE SharedMemoryHandler::getEmptyEvent() {
    return hEventEmpty;
}

HANDLE SharedMemoryHandler::getFullEvent() {
    return hEventFull;
}
