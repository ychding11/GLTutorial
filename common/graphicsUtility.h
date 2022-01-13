//=================================================================================//
// Copyright (c) 2021 Yaochuang Ding 
//=================================================================================//

#pragma once

#include <GL/glew.h>
#include "shader.h"

//< suppose all shader parameter is updated
void PostProcessing(GLuint srcTex, GLuint dstTex, const Shader& shader, std::string label = "PostProcessing");

void PresentTex(const Shader& shader, std::string label = "PresentTexture");


