//=================================================================================//
// Copyright (c) 2021 Yaochuang Ding 
//=================================================================================//

#include "framebuffer.h"


void Framebuffer::Init(int numColorBuffer, ColorBufferDsc* colorDscs, ZBufferDsc* zDscs)
{
    if (m_initialized)
    {
        Err("Frame buffer object already init!");
        return;
    }
    glGenFramebuffers(1, &m_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

    for (int i = 0; i < numColorBuffer; ++i)
    {
        const auto& desc = colorDscs[i];
        glGenTextures(1, &m_color_textuers[i]);
        glBindTexture(GL_TEXTURE_2D, m_color_textuers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, desc.internalformat, desc.width, desc.height, desc.border, desc.format, desc.type, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, desc.min_filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, desc.mag_filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, desc.warp_s);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, desc.warp_t);
        glFramebufferTexture2D(GL_FRAMEBUFFER, desc.attachment, GL_TEXTURE_2D, m_color_textuers[i], 0);
    }

    glGenRenderbuffers(1, &m_rb_depth_stencil);
    glBindRenderbuffer(GL_RENDERBUFFER, m_rb_depth_stencil);
    glRenderbufferStorage(GL_RENDERBUFFER, zDscs->internalformat, zDscs->width, zDscs->height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, zDscs->attachment, GL_RENDERBUFFER, m_rb_depth_stencil);
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
