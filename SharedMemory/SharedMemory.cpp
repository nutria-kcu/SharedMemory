#include "pch.h"
#include "SharedMemory.h"
#include <iostream>

SharedMemoryHandler::SharedMemoryHandler(const std::string& shmName, const std::string& eventName) {
    // Open the file mapping (shared memory)
    hMapFile = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, shmName.c_str());

    // Open the event (for synchronization)
    hEvent = OpenEventA(EVENT_ALL_ACCESS, FALSE, eventName.c_str());

    if (!hMapFile || !hEvent) {
        std::cerr << "Failed to open shared memory or event\n";
        exit(1);
    }

    // Map the shared memory to the process's address space
    shm = (SharedMemory*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SharedMemory));
}

SharedMemoryHandler::~SharedMemoryHandler() {
    // Unmap the shared memory view
    if (shm) {
        UnmapViewOfFile(shm);
    }
    // Close the file mapping handle (shared memory)
    if (hMapFile) {
        CloseHandle(hMapFile);
    }
    // Close the event handle
    if (hEvent) {
        CloseHandle(hEvent);
    }
}

void SharedMemoryHandler::setMessage(const std::string& message) {
    // Write message to shared memory
    strncpy_s(shm->message, message.c_str(), sizeof(shm->message) - 1);
}

const char* SharedMemoryHandler::getMessage() {
    // Read message from shared memory
    return shm->message;
}

SharedMemory* SharedMemoryHandler::getMemory() {
    return shm;
}

void SharedMemoryHandler::resetEvent() {
    ResetEvent(hEvent);  // Reset event to allow next trigger
}

