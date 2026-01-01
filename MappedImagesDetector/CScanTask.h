#pragma once
#include "StdInc.h"

class CScanTask
{
public:
    struct SMemoryRegion
    {
        DWORD64 dwBaseAddress;
        size_t  Size;
        DWORD   dwProtect;
    };

    CScanTask(HANDLE hProcess, int iProcessID);
    ~CScanTask();

    void SetOnFinishCallback(void*(callback)(LPVOID lpParam)) { m_lpOnFinishCallback = (LPVOID)callback; }

    bool EnumerateRegions(std::vector<CScanTask::SMemoryRegion*>* vMemoryRegions);
    void ScanRegion(SMemoryRegion* pRegion);
    void Execute();

    void ExecuteThread(int iThreadIdx);

    bool IsCompleted() { return m_bCompleted; }

    std::thread GetThread() { return std::move(m_Thread); }

private:
    bool FindIATThunk(BYTE* pBuffer, DWORD dwSize, DWORD64 dwBaseAddress, DWORD64* dwThunkAddress);

    std::thread m_Thread;
    HANDLE      m_hProcess;
    bool        m_bCompleted;

    LPVOID m_lpOnFinishCallback;
};
