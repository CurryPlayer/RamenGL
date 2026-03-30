#include "rgl_filesystem.h"

#include <assert.h>
#include <string.h>

static Filesystem* pTheOneAndOnly;

Filesystem* Filesystem::Init(int argc, char** argv, const char* baseDir)
{
    if ( !pTheOneAndOnly )
    {
        printf("Init filesystem...\n");

        pTheOneAndOnly = new Filesystem();
        PHYSFS_init(argv[ 0 ]);
        char sysBaseDir[ 512 ];
        bool pathSet = false;

        if ( argc > 1 )
        {
            strcpy(sysBaseDir, argv[ 1 ]);
            pathSet = true;
        }
        if ( !pathSet && baseDir )
        {
            strcpy(sysBaseDir, baseDir);
            pathSet = true;
        }
        if ( !pathSet )
        {
            const char* baseDir = PHYSFS_getBaseDir();
            strcpy(sysBaseDir, baseDir);
        }

        if ( !PHYSFS_mount(sysBaseDir, "", 1) )
        {
            printf("Failed to mount: %s\n", baseDir);
        }
        else
        {
            printf("Mounted system directory: %s\n", sysBaseDir);
            printf("Files available:\n");
            char** files = PHYSFS_enumerateFiles("");
            for ( char** i = files; *i != NULL; i++ )
            {
                printf("  %s\n", *i);
            }
            PHYSFS_freeList(files);
        }
    }

    return pTheOneAndOnly;
}

Filesystem* Filesystem::Instance()
{
    assert(pTheOneAndOnly && "Filesystem has not been initialized! Did you forget to call Filesystem::Init()?");
    return pTheOneAndOnly;
}
