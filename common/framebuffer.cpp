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
    GL_API_CHECK( glGenFramebuffers(1, &m_framebuffer) );
    GL_API_CHECK( glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer) );

    std::vector<GLenum> attachments(numColorBuffer, 0);

    for (int i = 0; i < numColorBuffer; ++i)
    {
        const auto& desc = colorDscs[i];
        GL_API_CHECK( glGenTextures(1, &m_color_textuers[i]) );
        GL_API_CHECK( glBindTexture(GL_TEXTURE_2D, m_color_textuers[i]) );
        GL_API_CHECK( glTexImage2D(GL_TEXTURE_2D, 0, desc.internalformat, desc.width, desc.height, desc.border, desc.format, desc.type, NULL) );
        GL_API_CHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, desc.min_filter) );
        GL_API_CHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, desc.mag_filter) );
        GL_API_CHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, desc.warp_s) );
        GL_API_CHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, desc.warp_t) );
        GL_API_CHECK( glFramebufferTexture2D(GL_FRAMEBUFFER, desc.attachment, GL_TEXTURE_2D, m_color_textuers[i], 0) );
        attachments[i] = desc.attachment;
    }
    GL_API_CHECK( glDrawBuffers(numColorBuffer, attachments.data()) );

    GL_API_CHECK( glGenRenderbuffers(1, &m_rb_depth_stencil) );
    GL_API_CHECK( glBindRenderbuffer(GL_RENDERBUFFER, m_rb_depth_stencil) );
    GL_API_CHECK( glRenderbufferStorage(GL_RENDERBUFFER, zDscs->internalformat, zDscs->width, zDscs->height) );
    GL_API_CHECK( glFramebufferRenderbuffer(GL_FRAMEBUFFER, zDscs->attachment, GL_RENDERBUFFER, m_rb_depth_stencil) );
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
