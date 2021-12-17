//=================================================================================//
// Copyright (c) 2021 Yaochuang Ding 
//=================================================================================//


#pragma once

#include <GL/glew.h>
//#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <Log.h>

class ColorBufferDsc
{
public:

    GLenum attachment{ GL_COLOR_ATTACHMENT0 };
    GLsizei  width{0};
    GLsizei  height{0};
    GLint internalformat{ GL_RGB }; //< number of color components in the texture
    GLint border{0};
    GLenum format{ GL_RGB }; //< format of the pixel data
    GLenum type{ GL_UNSIGNED_BYTE }; //< the data type of the pixel data

    GLint min_filter{ GL_LINEAR };
    GLint mag_filter{ GL_LINEAR };
    GLint warp_s{ GL_REPEAT };
    GLint warp_t{ GL_REPEAT };
};

class ZBufferDsc
{
public:

    //< always use render buffer
    GLenum attachment{ GL_DEPTH_STENCIL_ATTACHMENT };
    GLsizei  width{ 0 };
    GLsizei  height{ 0 };
    GLint internalformat{ GL_DEPTH_COMPONENT }; //< number of color components in the texture
};


class Framebuffer 
{
private:
    std::string m_name;


    GLuint m_framebuffer{0};
    GLuint m_tex_color{0};
    GLuint m_rb_depth_stencil{0};
    bool m_initialized{false};

    std::vector<GLuint> m_color_textuers;

public:

    ~Framebuffer()
    {
        if (m_tex_color)
            glDeleteTextures(1, &m_tex_color);
        for (int i = 0; i < m_color_textuers.size(); ++i)
        {
            if (m_color_textuers[i])
                glDeleteTextures(1, &m_color_textuers[i]);
        }

        //< https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glDeleteRenderbuffers.xhtml
        glDeleteRenderbuffers(1, &m_rb_depth_stencil);
        //< https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glDeleteFramebuffers.xhtml
        glDeleteFramebuffers(1, &m_framebuffer);
    }

    Framebuffer(std::string name = "framebuffer")
        : m_name(name)
        , m_color_textuers(8,0)
    {
    }

    void Init(int numColorBuffer, ColorBufferDsc* colorDscs, ZBufferDsc* zDscs);

    void Init(int width, int height)
    {
        if (m_initialized)
        {
            Err("Frame buffer object already init!");
            return;
        }
        glGenFramebuffers(1, &m_framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

        // create a color attachment texture
        glGenTextures(1, &m_tex_color);
        glBindTexture(GL_TEXTURE_2D, m_tex_color);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_tex_color, 0);

        //< create a renderbuffer object for depth and stencil attachment
        //< renderbuffer object stores raw data, so it is faster. But it can not be read directly.
        //< suppose depth buffer would not be sampled in shader in this case.
        glGenRenderbuffers(1, &m_rb_depth_stencil);
        glBindRenderbuffer(GL_RENDERBUFFER, m_rb_depth_stencil);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rb_depth_stencil);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            Err("Framebuffer is not complete. Init failed !");
        }
        else
        {
            m_initialized = true;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    GLuint ColorTexture() const
    {
        return m_tex_color;
    }

    GLuint ColorTexture(int index) const
    {
        //assert index here
        return m_color_textuers[index];
    }

    // activate the frame buffer 
    // ------------------------------------------------------------------------
    void Active()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    }

    void DeActive()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

};
