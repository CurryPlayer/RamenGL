#ifndef RGL_PLATFORM_H
#define RGL_PLATFORM_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct File
{
    File()
    {
        data   = nullptr;
        m_size = 0;
    }

    File(char* buffer, size_t size)
    {
        data   = buffer;
        m_size = size;
    }

    void Destroy()
    {
        if ( data )
        {
            free(data);
            m_size = 0;
        }
    }

    char*  data;
    size_t m_size;
};

bool FileExists(const char* file);
File ReadFile(const char* file);
File CreateFile(uint64_t numBytes);

#endif
