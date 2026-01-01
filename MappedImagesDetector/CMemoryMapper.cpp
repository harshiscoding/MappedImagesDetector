#include "StdInc.h"
#include <tlhelp32.h>

CMemoryMapper::CMemoryMapper()
{
}

CMemoryMapper::~CMemoryMapper()
{
}

void CMemoryMapper::Attach(std::vector<const char*> vProcesses)
{
    for (const auto& szProcessName : vProcesses)
    {
        Attach(szProcessName);
    }
}

void CMemoryMapper::Attach(const char* szProcessName)
{
    DWORD  iProcessID = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot != INVALID_HANDLE_VALUE)
    {
        PROCESSENTRY32 pe32;
        pe32.dwSize = sizeof(PROCESSENTRY32);
        if (Process32First(hSnapshot, &pe32))
        {
            do
            {
                if (_stricmp(pe32.szExeFile, szProcessName) == 0)
                {
                    iProcessID = pe32.th32ProcessID;
                    break;
                }
            } while (Process32Next(hSnapshot, &pe32));
        }
        CloseHandle(hSnapshot);
    }

    if (iProcessID == 0)
    {
        Logger::Instance().Log(LogLevel::Warning, "Process '%s' not found.", szProcessName);
        return;
    }

    KernelCalls_CLIENT_ID         clientId{};
    KernelCalls_OBJECT_ATTRIBUTES objAttr{};

    RtlSecureZeroMemory(&objAttr, sizeof(objAttr));
    RtlSecureZeroMemory(&clientId, sizeof(clientId));

    objAttr.Length = sizeof(objAttr);
    clientId.UniqueProcess = reinterpret_cast<HANDLE>(static_cast<ULONG_PTR>(iProcessID));

    Logger::Instance().Log(LogLevel::Info, "Attaching to process '%s' with ID %d", szProcessName, iProcessID);

    HANDLE   hProcess = nullptr;
    NTSTATUS status = SysNtOpenProcess(&hProcess, PROCESS_ALL_ACCESS, &objAttr, &clientId);

    if (!NT_SUCCESS(status) || !hProcess)
    {
        Logger::Instance().Log(LogLevel::Error, "Failed to open process with ID %d, error: %x", iProcessID, status);
    }
    else
    {
        m_pScanTasks.push(new CScanTask(hProcess, iProcessID));
    }
}

void CMemoryMapper::Start()
{
    int iThreadIdx = 1;

    // while (!m_pScanTasks.empty())
    {
        CScanTask* pTask = m_pScanTasks.front();
        pTask->ExecuteThread(iThreadIdx);
        iThreadIdx++;
    }
}