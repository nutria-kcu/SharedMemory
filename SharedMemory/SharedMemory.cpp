#include "pch.h"
#include "SharedMemory.h"
#include <iostream>
#include <tchar.h>

SharedMemoryHandler::SharedMemoryHandler(const TCHAR* shmName, const int isProducer) {
    SharedMemoryHandler::isProducer = isProducer;

    _tprintf(_T("SharedMemoryName: %s\n"), shmName);

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

bool SharedMemoryHandler::setMessage(int input = 0, int option = 0) {
    if (!isProducer) {
        std::cout << "getMessage accessed from consumer\n";
        return false;
    }

    if (shm->isMessageSet) {
        std::cout << "Message already set. Waiting for consumer to read.\n";
        std::cout << "Ignore the given msg\n";
    }

    shm->cmd = input;
    shm->option = option;
    shm->isMessageSet = true;
    return true;
}

SharedMemory* SharedMemoryHandler::getMessage() {
    if (isProducer) {
        std::cout << "getMessage accessed from producer\n";
        return nullptr;
    }

    if (!shm->isMessageSet) {
        std::cout << "No command set.\n";
        return nullptr;
    }
    shm->isMessageSet = false;
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

void SharedMemoryHandler::initProducer(const TCHAR* shmName) {
    hMapFile = CreateFileMapping(
        INVALID_HANDLE_VALUE,    // use paging file
        NULL,                    // default security
        PAGE_READWRITE,          // read/write access
        0,                       // maximum object size (high-order DWORD)
        sizeof(SharedMemory),                // maximum object size (low-order DWORD)
        shmName);


    //HANDLE CreateEvent(
    //    LPSECURITY_ATTRIBUTES lpEventAttributes, security stuff, set NULL
    //    BOOL                  bManualReset, False is Auto Reset
    //    BOOL                  bInitialState, TRUE if signaled 
    //    LPCTSTR               lpName
    //);
    hEventEmpty = CreateEvent(NULL, TRUE, TRUE, L"IPCEMPTY");
    hEventFull = CreateEvent(NULL, TRUE, FALSE, L"IPCFULL");
}

void SharedMemoryHandler::initConsumer(const TCHAR* shmName) {
    hMapFile =  OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, shmName);

    hEventEmpty = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"IPCEMPTY");
    hEventFull = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"IPCFULL");
}

HANDLE SharedMemoryHandler::getEmptyEvent() {
    return hEventEmpty;
}

HANDLE SharedMemoryHandler::getFullEvent() {
    return hEventFull;
}
