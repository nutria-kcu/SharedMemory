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
    HANDLE hEventProduced;
    HANDLE hEventConsumed;
    SharedMemory* shm;
    bool isProducer;

public:
    SharedMemoryHandler(const std::wstring& shmName, const int isProducer);

    ~SharedMemoryHandler();

    void setMessage(const std::string& message);

    const char* getMessage();

    SharedMemory* getMemory();

    void resetEvent();

    void cleanup();

    void initProducer(const std::wstring& shmName);

    void initConsumer(const std::wstring& shmName);
};

#endif // SHAREDMEMORY_H
