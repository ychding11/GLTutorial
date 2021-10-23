//=================================================================================//
// Copyright (c) 2021 Yaochuang Ding 
//=================================================================================//

#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <Log.h>

#include "shader.h" 
#include "texture.h" 

class Material
{
public:
    bool m_dirty{false};
    bool m_reevaluate{false};
    std::string m_diffuseTexName;
    std::string m_normalTexName;

private:
    std::string m_name;
    Shader m_shader;

    TextureID m_albedoMapTexId{INVALIDE_TEXTURE_ID};
    TextureID m_normalMapTexId{INVALIDE_TEXTURE_ID};

public:
    Material(std::string name)
        : m_name(name)
    {

    }

    void Init(const char* vs_file_path, const char* fs_file_path,
        const char* tcs_file_path = nullptr, const char* tes_file_path = nullptr,
        const char* gs_file_path = nullptr)
    {
        m_shader.Init(vs_file_path, fs_file_path, tcs_file_path, tes_file_path, gs_file_path);
    }

    Shader* GetShader()
    {
        return &m_shader;
    }

    void ActiveDiffuseMap()
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
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_albedoMapTexId);
    }

    void ActiveNormalMap()
    {
        if (m_normalMapTexId == INVALIDE_TEXTURE_ID)
        {
            Err("normal map texture ID is invalide.");
            return;
        }
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_normalMapTexId);
    }

    void SetBool(const std::string& name, bool value) const
    {
        m_shader.SetBool(name, value);
    }
    void SetInt(const std::string& name, int value) const
    {
        m_shader.SetInt(name, value);
    }
    void SetFloat(const std::string& name, float value) const
    {
        m_shader.SetFloat(name, value);
    }
    void SetVec2(const std::string& name, const glm::vec2& value) const
    {
        m_shader.SetVec2(name, value);
    }
    void SetVec2(const std::string& name, float x, float y) const
    {
        m_shader.SetVec2(name, x, y);
    }
    void SetVec3(const std::string& name, const glm::vec3& value) const
    {
        m_shader.SetVec3(name, value);
    }
    void SetVec3(const std::string& name, float x, float y, float z) const
    {
        m_shader.SetVec3(name, x, y, z);
    }
    void SetVec4(const std::string& name, const glm::vec4& value) const
    {
        m_shader.SetVec4(name, value);
    }
    void SetVec4(const std::string& name, float x, float y, float z, float w)
    {
        m_shader.SetVec4(name, x, y, z, w);
    }
    void SetMat2(const std::string& name, const glm::mat2& mat) const
    {
        m_shader.SetMat2(name, mat);
    }
    void SetMat3(const std::string& name, const glm::mat3& mat) const
    {
        m_shader.SetMat3(name, mat);
    }
    void SetMat4(const std::string& name, const glm::mat4& mat) const
    {
        m_shader.SetMat4(name, mat);
    }
};
