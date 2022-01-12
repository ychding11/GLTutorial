//=================================================================================//
// Copyright (c) 2021 Yaochuang Ding 
//=================================================================================//


#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>
#include <Log.h>

#include "shaderUtility.h" 

enum class ShaderParamType
{
    Scalar,
    Vector,
    Matrix,
    Tex2D,
};

//< 1. dynamic memory would incur management overhead
//< 2. union style design would make the code ugly
struct ShaderParam
{
    std::string name;
    ShaderParamType type{ ShaderParamType::Scalar};
    int demension{1};
    int slot{-1};
    int arraysize{ 1 };
    void* data{nullptr};

    ~ShaderParam()
    {
        delete data; //< delete nullptr is OK
        Log("{} shader param destruction !", name);
    }
};
 
typedef std::unordered_map<std::string, ShaderParam> ShaderParamMap;

class Shader
{
public:
    ShaderParamMap m_paramMap;

private:
    std::string m_name;
    GLuint m_programID{0};
    bool m_initialized{false};

    GLint uniformLocation(const std::string& name) const
    {
        GLint index = glGetUniformLocation(m_programID, name.c_str());
        if (index == -1)
        {
            Err("Get uniform location failed, {}", name);
        }
        return index;
    }


public:

    ~Shader()
    {
        glDeleteProgram(m_programID);
    }

    Shader() = default;

    Shader(std::string name)
        : m_name(name)
    {

    }

    void Init(const char* vs_file_path, const char* fs_file_path, const char* tcs_file_path = nullptr, const char* tes_file_path = nullptr,
        const char* gs_file_path = nullptr)
    {
        m_programID = LoadShaders(vs_file_path, fs_file_path, tcs_file_path, tes_file_path, gs_file_path);
        Log("Link shader done.");
        m_initialized = true;
    }

    // activate the shader
    // ------------------------------------------------------------------------
    void Active() const 
    {
        glUseProgram(m_programID);
    }
    void Deactive() const 
    {
        glUseProgram(0);
    }

    void Apply() const
    {
        for (auto it = m_paramMap.begin(); it != m_paramMap.end(); ++it)
        {
            auto &param = it->second;
            auto &type = param.type;
            switch (type)
            {
            case ShaderParamType::Scalar:
            {
                SetInt(param.name, *(int*)(param.data));
                break;
            }
            case ShaderParamType::Vector:
            {
                if (param.demension == 1)
                {
                    SetFloat(param.name, *(float*)(param.data));
                }
                else if (param.demension == 2)
                {
                    SetVec2(param.name, *(glm::vec2*)(param.data));
                }
                else if (param.demension == 3)
                {
                    SetVec3(param.name, *(glm::vec3*)(param.data));
                }
                else if (param.demension == 4)
                {
                    SetVec4(param.name, *(glm::vec4*)(param.data));
                }

                break;
            }
            case ShaderParamType::Matrix:
            {
                if (param.demension == 2)
                {
                    SetMat2(param.name, *(glm::mat2*)(param.data));
                }
                else if (param.demension == 3)
                {
                    SetMat3(param.name, *(glm::mat3*)(param.data));
                }
                else if (param.demension == 4)
                {
                    SetMat4(param.name, *(glm::mat4*)(param.data));
                }

                break;
            }
            case ShaderParamType::Tex2D:
            {
                SetTex2D(param.name, *(GLuint*)(param.data), param.slot);
                break;
            }
            default :
                break;
            }
        }
    }

    // utility uniform functions
    // ------------------------------------------------------------------------
    void SetTex2D(const std::string& name, GLuint texID, int slot) const
    {
        glActiveTexture(GL_TEXTURE0 + slot); //< always slot 0 ?
        glBindTexture(GL_TEXTURE_2D, texID);
        glUniform1i(glGetUniformLocation(m_programID, name.c_str()), slot);
    }
    void SetBool(const std::string& name, bool value) const
    {
        glUniform1i(glGetUniformLocation(m_programID, name.c_str()), (int)value);
    }
    void SetInt(const std::string& name, int value) const
    {
        glUniform1i(glGetUniformLocation(m_programID, name.c_str()), value);
    }
    void SetFloat(const std::string& name, float value) const
    {
        glUniform1f(glGetUniformLocation(m_programID, name.c_str()), value);
    }
    void SetVec2(const std::string& name, const glm::vec2& value) const
    {
        glUniform2fv(glGetUniformLocation(m_programID, name.c_str()), 1, &value[0]);
    }
    void SetVec2(const std::string& name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(m_programID, name.c_str()), x, y);
    }
    void SetVec3(const std::string& name, const glm::vec3& value) const
    {
        glUniform3fv(glGetUniformLocation(m_programID, name.c_str()), 1, &value[0]);
    }
    void SetVec3(const std::string& name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(m_programID, name.c_str()), x, y, z);
    }
    void SetVec4(const std::string& name, const glm::vec4& value) const
    {
        glUniform4fv(glGetUniformLocation(m_programID, name.c_str()), 1, &value[0]);
    }
    void SetVec4(const std::string& name, float x, float y, float z, float w)
    {
        glUniform4f(glGetUniformLocation(m_programID, name.c_str()), x, y, z, w);
    }
    void SetMat2(const std::string& name, const glm::mat2& mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(m_programID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    void SetMat3(const std::string& name, const glm::mat3& mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(m_programID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    void SetMat4(const std::string& name, const glm::mat4& mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(m_programID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
};
