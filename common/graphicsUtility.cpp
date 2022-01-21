//=================================================================================//
// Copyright (c) 2021 Yaochuang Ding 
//=================================================================================//

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "graphicsUtility.h" 
#include "camera.h"
#include "aabb.h"

    void BoundingBoxDrawer::Init()
    {
        GLfloat vertices[] =
        {
            -0.5, -0.5, -0.5, 1.0,
             0.5, -0.5, -0.5, 1.0,
             0.5,  0.5, -0.5, 1.0,
            -0.5,  0.5, -0.5, 1.0,
            -0.5, -0.5,  0.5, 1.0,
             0.5, -0.5,  0.5, 1.0,
             0.5,  0.5,  0.5, 1.0,
            -0.5,  0.5,  0.5, 1.0,
        };

        GLushort elements[] =
        {
          0, 1, 2, 3,
          4, 5, 6, 7,
          0, 4, 1, 5, 2, 6, 3, 7
        };

        glGenVertexArrays(1, &m_vao_id);
        glBindVertexArray(m_vao_id);

        glGenBuffers(1, &vbo_id);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4*sizeof(GLfloat), 0);
        glEnableVertexAttribArray(0);

        glGenBuffers(1, &ibo_id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        //< delete IBO, VBO in destructor
        //< does delete order concern for IBO, VBO, VAO ?

        m_shader.Init("shaders/aabbDrawer.vs.glsl", "shaders/aabbDrawer.fs.glsl");
        {
            auto& shaderParam = m_shader.m_paramMap;
            //shaderParam["u_mat_PVM"].name = "u_mat_PVM";
            //shaderParam["u_mat_PVM"].type = ShaderParamType::Matrix;
            //shaderParam["u_mat_PVM"].data = new glm::mat4();
            //shaderParam["u_mat_PVM"].demension = 4;
        }
    }

    void BoundingBoxDrawer::DrawBoundingBox(const AABB &aabb, const Camera & camera)
    {
        //< generate transform matrix from AABB
        auto center = aabb.Center();
        auto scale = aabb.Diagonal();
        auto world = glm::translate(glm::mat4(1), center) * glm::scale(glm::mat4(1), scale);
        auto PVM = camera.projMatrix() * camera.viewMatrix() * world;
        auto& shaderParam = m_shader.m_paramMap;
        //SHADER_PARAM_CHECK(shaderParam, "u_mat_PVM");
        SHADER_PARAM_SET_MAT4(shaderParam, "u_mat_PVM", PVM);

        m_shader.Apply();

        //< bind VAO & IBO & Draw
        //< https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glDrawElements.xhtml
        GL_API_CHECK( glBindVertexArray(m_vao_id) );
        GL_API_CHECK( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_id) );
        GL_API_CHECK( glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, 0) );
        GL_API_CHECK( glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, (GLvoid*)(4 * sizeof(GLushort))) );
        GL_API_CHECK( glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, (GLvoid*)(8 * sizeof(GLushort))) );

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }


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

        GL_API_CHECK(glBindVertexArray(m_empty_vao));
        GL_API_CHECK(glDrawArrays(GL_TRIANGLES, 0, 3));
        glDeleteVertexArrays(1, &m_empty_vao);
        glDeleteFramebuffers(1, &m_framebuffer); //< suppose color attachment is valid after FB object deletion
        glPopDebugGroup();
    }

    void PresentTex(const Shader& shader, std::string label)
    {
        glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, label.c_str());
        GLuint m_empty_vao{ 0 }; //< in core profile, we need an explict empty vao, 0 means nothing
        glGenVertexArrays(1, &m_empty_vao);
        glBindFramebuffer(GL_FRAMEBUFFER, 0); //< to display 
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);  //< disable z depth

        shader.Apply();
        GL_API_CHECK(glBindVertexArray(m_empty_vao));
        GL_API_CHECK(glDrawArrays(GL_TRIANGLES, 0, 3));
        GL_API_CHECK(glDeleteVertexArrays(1, &m_empty_vao));
        glPopDebugGroup();
    }

