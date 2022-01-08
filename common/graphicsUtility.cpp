//=================================================================================//
// Copyright (c) 2021 Yaochuang Ding 
//=================================================================================//

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>

#include "graphicsUtility.h"
#include "Log.h"

void PostProcessing(GLuint srcTex, GLuint dstTex, const Shader& shader, std::string label)
{
    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, label.c_str());
        GLuint m_empty_vao{ 0 }; //< in core profile, we need an explict empty vao, 0 means nothing
        glGenVertexArrays(1, &m_empty_vao);
        GLuint m_framebuffer{ 0 };
        glGenFramebuffers(1, &m_framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dstTex, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);  //< disable z depth

        shader.Active();
        shader.SetTex2D("u_tex_src_map", srcTex, 0);

        GL_API_CHECK( glBindVertexArray(m_empty_vao) );
        GL_API_CHECK( glDrawArrays(GL_TRIANGLES, 0, 3) );
        glBindVertexArray(0);
        shader.Deactive();
        glDeleteVertexArrays(1, &m_empty_vao);
        glDeleteFramebuffers(1, &m_framebuffer); //< suppose color attachment is valid after FB object deletion
    glPopDebugGroup();
}
void PresentTex(GLuint srcTex, const Shader& shader, std::string label)
{
    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, label.c_str());
        GLuint m_empty_vao{ 0 }; //< in core profile, we need an explict empty vao, 0 means nothing
        glGenVertexArrays(1, &m_empty_vao);
        glBindFramebuffer(GL_FRAMEBUFFER, 0); //< to display 
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);  //< disable z depth

        shader.Active();
        shader.SetTex2D("u_tex_color_map", srcTex, 0);
        glBindVertexArray(m_empty_vao);
        GL_API_CHECK(glDrawArrays(GL_TRIANGLES, 0, 3));
        glBindVertexArray(0);
        shader.Deactive();
        glDeleteVertexArrays(1, &m_empty_vao);
    glPopDebugGroup();
}

