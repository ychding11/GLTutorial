//=================================================================================//
// Copyright (c) 2021 Yaochuang Ding 
//=================================================================================//
#pragma once

#include <GL/glew.h>
#include "shader.h"

#define QUERY_GL_Limit(name)                                        \
do {                                                                \
    int value = 0;                                                  \
    glGetIntegerv(name, &value);                                    \
    Log(#name"={}", value);                                         \
} while (0);

class AABB;
class Camera;

class BoundingBoxDrawer
{
public :
    BoundingBoxDrawer() = default;

    void Init();
    void DrawBoundingBox(const AABB& aabb, const Camera& camera);

private:
    GLuint m_vao_id;
    GLuint vbo_id;
    GLuint ibo_id;
    Shader m_shader{"BoxDrawer"};
};

//< suppose all shader parameter is updated
void PostProcessing(GLuint srcTex, GLuint dstTex, const Shader& shader, std::string label = "PostProcessing");

void PresentTex(const Shader& shader, std::string label = "PresentTexture");


