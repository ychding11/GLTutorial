//=================================================================================//
// Copyright (c) 2021 Yaochuang Ding 
//=================================================================================//


#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>

#include "viewer.h"

int main(int argc, char **argv)
{
    std::string modelPath;
    char **filename = nullptr; 
    for (int i = 0; i < argc; ++i)
    {
        if (i + 1 < argc && strcmp(argv[i], "-obj") == 0)
            filename = argv + i + 1, ++i;
        else
        {
        }
    }

    if ( filename != nullptr )
    {
        modelPath = *filename;
    }
    else
    {
        modelPath = "Model/bunny.obj";
    }

    Viewer viewer{"Normal Map Viewer", 1280, 720};
    viewer.SetModel(modelPath);
    viewer.Run();

    return 0;
}

