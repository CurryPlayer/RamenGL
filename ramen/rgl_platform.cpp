#include "rgl_platform.h"

#include <stdlib.h>
#include <string.h>

bool FileExists(const char* file)
{
    FILE* pFileDesc = nullptr;
    pFileDesc       = fopen(file, "r");
    if ( pFileDesc == nullptr )
    {
        return false;
    }
    fclose(pFileDesc);

    return true;
}

File ReadFile(const char* file)
{
    FILE* pFileDesc = 0;
    pFileDesc       = fopen(file, "rb");
    if ( file == 0 )
    {
        return File{};
    }
    fseek(pFileDesc, 0L, SEEK_END);
    size_t size = ftell(pFileDesc);
    fseek(pFileDesc, 0L, SEEK_SET);
    char* data = (char*)malloc(size + 1);
    fread(data, sizeof(char), size, pFileDesc);
    data[ size ] = '\0';
    fclose(pFileDesc);

    return File{ data, size };
}

File CreateFile(uint64_t numBytes)
{
    File result{};
    result.data = (char*)malloc(numBytes + 1);
    memset(result.data, 0, numBytes + 1);
    result.m_size = numBytes + 1;

    return result;
}
