#ifndef SHAREDMEMORY_H
#define SHAREDMEMORY_H

#include <windows.h>
#include <string>

#ifdef SHAREDMEMORY_EXPORTS
#define SHAREDMEMORY_API __declspec(dllexport)
#else
#define SHAREDMEMORY_API __declspec(dllimport)
#endif

struct SharedMemory {
    char message[20];
    bool isMessageSet = false;
};

class SHAREDMEMORY_API SharedMemoryHandler {
private:
    HANDLE hMapFile;
    HANDLE hEventFull;
    HANDLE hEventEmpty;
    SharedMemory* shm;

    bool isProducer;
    void cleanup();
    void initProducer(const std::wstring& shmName);
    void initConsumer(const std::wstring& shmName);

public:
    SharedMemoryHandler(const std::wstring& shmName, const int isProducer);

    ~SharedMemoryHandler();

    bool setMessage(const std::string& message);

    const char* getMessage();

    SharedMemory* getMemory();

    void resetEvent();

    HANDLE getFullEvent();

    HANDLE getEmptyEvent();
};

#endif // SHAREDMEMORY_H
