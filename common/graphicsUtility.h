//=================================================================================//
// Copyright (c) 2021 Yaochuang Ding 
//=================================================================================//
#pragma once

#include <GL/glew.h>
#include "shader.h"

#define kInvalidGLId 0xffffffffu

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


//
inline bool IsValidGLId(GLuint id)
{
    return id != kInvalidGLId;
}

// busy wait for GPU job done
inline static void SyncGPU()
{
    GLsync sync_fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    GLenum wait_return = GL_UNSIGNALED;
    while (wait_return != GL_ALREADY_SIGNALED && wait_return != GL_CONDITION_SATISFIED)
        wait_return = glClientWaitSync(sync_fence, GL_SYNC_FLUSH_COMMANDS_BIT, 1);
    glDeleteSync(sync_fence);
}
