//=================================================================================//
// Copyright (c) 2021 Yaochuang Ding 
//=================================================================================//

#include "material.h" 


void Material::ActiveDiffuseMap()
{
    if (m_dirty && m_reevaluate)
    {
        m_albedoMapTexId = GetTextureCache().Acquire(m_diffuseTexName);
        m_normalMapTexId = GetTextureCache().Acquire(m_normalTexName);
        m_dirty = false;
        m_reevaluate = false;
    }

    if (m_albedoMapTexId == INVALIDE_TEXTURE_ID)
    {
        Err("diffuse map texture ID is invalide.");
        return;
    }
    m_shader.SetTex2D("diffuseMap",m_albedoMapTexId, 0);
}

void Material::DeactiveNormalMap()
{
    m_shader.SetInt("use_normal_map", 0);
}

void Material::ActiveNormalMap()
{
    if (m_dirty && m_reevaluate)
    {
        m_albedoMapTexId = GetTextureCache().Acquire(m_diffuseTexName);
        m_normalMapTexId = GetTextureCache().Acquire(m_normalTexName);
        m_dirty = false;
        m_reevaluate = false;
    }

    if (m_normalMapTexId == INVALIDE_TEXTURE_ID)
    {
        Err("normal map texture ID is invalid.");
        return;
    }

    m_shader.SetInt("use_normal_map", 1);
    m_shader.SetTex2D("normalMap", m_normalMapTexId, 1);
}



