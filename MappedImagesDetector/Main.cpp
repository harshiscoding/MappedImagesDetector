#include "StdInc.h"

int main()
{
    CMemoryMapper* pMapper = new CMemoryMapper();

    Logger::Instance().SetLogFile("MappedImages.logs");

    pMapper->Attach("notepad.exe");
    pMapper->Start();
}
