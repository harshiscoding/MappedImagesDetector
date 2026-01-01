#pragma once
#include "StdInc.h"

class CMemoryMapper
{
public:
    CMemoryMapper();
    ~CMemoryMapper();

    void Attach(std::vector<const char*> vProcesses);
    void Attach(const char* szProcessName);

    void Start();

private:
    std::queue<CScanTask*> m_pScanTasks;
};
