//=================================================================================//
// Copyright (c) 2021 Yaochuang Ding 
//=================================================================================//

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <glm/glm.hpp>

enum RenderMode : int 
{
    Standard = 0,
    Explode,
    Particle,

    Count,
};

struct RenderSetting
{
    // the input of current setting
    int  width = 1280;
    int  height = 720;
    bool enableTess{false};
    bool showHelpTip{false};
    RenderMode renderMode{RenderMode::Standard };
    glm::vec4 outerTessLevel{8.f};
    glm::vec3 innerTessLevel{3.f};
};

struct DisplayOption
{
    bool wireframe;
    bool showUI;

    DisplayOption()
        : wireframe(false)
        , showUI(true)
    { }
};

RenderSetting& GetRenderSetting() ;

DisplayOption& GetDisplayOption() ;

