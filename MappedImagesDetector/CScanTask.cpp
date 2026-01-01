#include "StdInc.h"

CScanTask::CScanTask(HANDLE hProcess, int iProcessID) : m_hProcess(hProcess), m_bCompleted(false)
{
}

CScanTask::~CScanTask()
{
}

bool CScanTask::EnumerateRegions(std::vector<CScanTask::SMemoryRegion*>* vMemoryRegions)
{
    NTSTATUS                 status;
    MEMORY_BASIC_INFORMATION MemoryRegion{};
    SYSTEM_INFO              sysInfo;

    GetSystemInfo(&sysInfo);

    Logger::Instance().Log(LogLevel::Info, "Enumerating memory regions from 0x%llx to 0x%llx", reinterpret_cast<DWORD64>(sysInfo.lpMinimumApplicationAddress),
                           reinterpret_cast<DWORD64>(sysInfo.lpMaximumApplicationAddress));

    for (LPVOID lpCurrentAddress = sysInfo.lpMinimumApplicationAddress; lpCurrentAddress < sysInfo.lpMaximumApplicationAddress;
         lpCurrentAddress = static_cast<LPBYTE>(MemoryRegion.BaseAddress) + MemoryRegion.RegionSize)
    {
        size_t returnLength = 0;
        if (!NT_SUCCESS(status =
                            SysNtQueryVirtualMemory(m_hProcess, lpCurrentAddress, MemoryBasicInformation, &MemoryRegion, sizeof(MemoryRegion), &returnLength)))
        {
            Logger::Instance().Log(LogLevel::Warning, "Failed to query memory region at address 0x%llx, error: %d", reinterpret_cast<DWORD64>(lpCurrentAddress),
                                   status);
            continue;
        }

        if (MemoryRegion.Protect & (PAGE_NOACCESS | PAGE_GUARD | PAGE_WRITECOMBINE))
            continue;

        if (!(MemoryRegion.Protect & (PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_READ | PAGE_EXECUTE_WRITECOPY)))
            continue;

        vMemoryRegions->push_back(new SMemoryRegion{reinterpret_cast<DWORD64>(MemoryRegion.BaseAddress), MemoryRegion.RegionSize, MemoryRegion.Protect});
    }

    Logger::Instance().Log(LogLevel::Info, "%zu memory regions enumerated for scanning.", vMemoryRegions->size());

    return vMemoryRegions->size() > 0;
}

void CScanTask::ScanRegion(SMemoryRegion* pRegion)
{
    std::vector<BYTE> buffer(pRegion->Size);

    size_t bytesRead = 0;
    if (NT_SUCCESS(SysNtReadVirtualMemory(m_hProcess, reinterpret_cast<PVOID>(pRegion->dwBaseAddress), buffer.data(), pRegion->Size, &bytesRead)))
    {
        DWORD64 dwThunkAddress = NULL;
        if (FindIATThunk(buffer.data(), static_cast<DWORD>(bytesRead), pRegion->dwBaseAddress, &dwThunkAddress))
        {
            Logger::Instance().Log(LogLevel::Warning, "IAT thunk found at address: 0x%llx in region starting at 0x%llx", dwThunkAddress,
                                   pRegion->dwBaseAddress);
        }
    }
}

void CScanTask::Execute()
{
    Logger::Instance().Log(LogLevel::Debug, "Starting scan task...");
    std::vector<SMemoryRegion*> vMemoryRegions;
    int                         iTotalRegions = EnumerateRegions(&vMemoryRegions);
    if (iTotalRegions)
    {
        for (const auto& pRegion : vMemoryRegions)
        {
            ScanRegion(pRegion);
            delete pRegion;
        }
    }
    else
    {
        Logger::Instance().Log(LogLevel::Warning, "No memory regions found to scan.");
    }

    m_bCompleted = true;
    Logger::Instance().Log(LogLevel::Info, "Scan task completed %d.", iTotalRegions);

    if (m_lpOnFinishCallback)
    {
        auto callback = reinterpret_cast<void (*)(LPVOID)>(m_lpOnFinishCallback);
        callback(nullptr);
    }
}

bool CScanTask::FindIATThunk(BYTE* pBuffer, DWORD dwSize, DWORD64 dwBaseAddress, DWORD64* dwThunkAddress)
{
    DWORD   dwCompleteSequence = NULL;
    DWORD64 dwIATAddress = NULL;

    for (size_t z = 0; z < dwSize; ++z)
    {
        for (size_t x = 0; x < (8 * 6); x += 0x6)
        {
            size_t offset = z + x;

            if ((offset + 1) < dwSize)
            {
                if (pBuffer[offset] == 0xFF && pBuffer[offset + 1] == 0x25)
                {
                    *dwThunkAddress = dwBaseAddress + offset;
                    dwCompleteSequence++;
                }
                else
                {
                    dwCompleteSequence = NULL;
                }
            }
            else
            {
                dwCompleteSequence = NULL;
            }

            if (dwCompleteSequence >= 8)
            {
                return true;
            }
        }
    }

    return false;
}

void CScanTask::ExecuteThread(int iThreadIdx)
{
    Logger::Instance().Log(LogLevel::Info, "Launching scan task thread %d", iThreadIdx);

    m_Thread = std::thread(&CScanTask::Execute, this);
    m_Thread.join();
}