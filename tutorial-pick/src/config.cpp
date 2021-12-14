//=================================================================================//
// Copyright (c) 2021 Yaochuang Ding 
//=================================================================================//


#include "config.h"

static RenderSetting tempSetting;
static DisplayOption tempDisplayOption;


RenderSetting& GetRenderSetting()
{
    return tempSetting;
};

DisplayOption& GetDisplayOption()
{
    return tempDisplayOption;
}
