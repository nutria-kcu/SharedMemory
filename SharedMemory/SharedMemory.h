#ifndef SHAREDMEMORY_H
#define SHAREDMEMORY_H

#include <windows.h>
#include <string>

#ifdef SHAREDMEMORY_EXPORTS
#define SHAREDMEMORY_API __declspec(dllexport)
#else
#define SHAREDMEMORY_API __declspec(dllimport)
#endif

// Shared memory structure
struct SharedMemory {
    char message[256];
};

// SharedMemoryHandler class for managing shared memory
class SHAREDMEMORY_API SharedMemoryHandler {
private:
    HANDLE hMapFile;
    HANDLE hEvent;
    SharedMemory* shm;

public:
    // Constructor to initialize the shared memory
    SharedMemoryHandler(const std::string& shmName, const std::string& eventName);

    // Destructor to clean up resources
    ~SharedMemoryHandler();

    // Set a message in shared memory
    void setMessage(const std::string& message);

    // Get a message from shared memory
    const char* getMessage();

    // Function to map shared memory
    SharedMemory* getMemory();

    // Reset event to allow next trigger
    void resetEvent();
};

#endif // SHAREDMEMORY_H
