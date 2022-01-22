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

#define SHADER_PARAM_CHECK(paramMap, name) do {                          \
    if (paramMap.find(name)==paramMap.end())                             \
    {                                                                    \
        Err("Not fine shader param: {} !", name);                        \
    }                                                                    \
} while (0)

#define SHADER_PARAM_SET_MAT4(paramMap, name, value) do {               \
        SHADER_PARAM_CHECK(paramMap, name);                             \
        *(glm::mat4*)shaderParam[name].data = value;                    \
} while (0)

#define SHADER_PARAM_SET_MAT3(paramMap, name, value) do {               \
        SHADER_PARAM_CHECK(paramMap, name);                             \
        *(glm::mat3*)shaderParam[name].data = value;                    \
} while (0)

#define SHADER_PARAM_SET_VEC2(paramMap, name, value) do {               \
        SHADER_PARAM_CHECK(paramMap, name);                             \
        *(glm::vec2*)shaderParam[name].data = value;                    \
} while (0)

#define SHADER_PARAM_SET_VEC3(paramMap, name, value) do {               \
        SHADER_PARAM_CHECK(paramMap, name);                             \
        *(glm::vec3*)shaderParam[name].data = value;                    \
} while (0)

#define SHADER_PARAM_SET_VEC4(paramMap, name, value) do {               \
        SHADER_PARAM_CHECK(paramMap, name);                             \
        *(glm::vec4*)shaderParam[name].data = value;                    \
} while (0)

#define SHADER_PARAM_SET_SAMPLER2D(paramMap, name, value) do {         \
        SHADER_PARAM_CHECK(paramMap, name);                            \
        *(int*)shaderParam[name].data = value;                         \
} while (0)

#define SHADER_PARAM_SET_INT(paramMap, name, value) do {               \
        SHADER_PARAM_CHECK(paramMap, name);                            \
        *(int*)shaderParam[name].data = value;                         \
} while (0)

#define SHADER_PARAM_SET_FLOAT(paramMap, name, value) do {          \
        SHADER_PARAM_CHECK(paramMap, name);                         \
        *(float*)shaderParam[name].data = value;                    \
} while (0)

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
    std::string m_name{"Not Set"};
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

    std::string Name() const
    {
        return m_name;
    }

    void Init(const char* vs_file_path, const char* fs_file_path,
        const char* tcs_file_path = nullptr, const char* tes_file_path = nullptr,
        const char* gs_file_path = nullptr);

    // activate the shader
    // ------------------------------------------------------------------------
    void Active() const 
    {
        GL_API_CHECK( glUseProgram(m_programID) );
    }
    void Deactive() const 
    {
        glUseProgram(0);
    }

    void Apply() const;

    // utility uniform functions
    // ------------------------------------------------------------------------
    void SetTex2D(const std::string& name, GLuint texID, int slot) const
    {
        glActiveTexture(GL_TEXTURE0 + slot); //< always slot 0 ?
        glBindTexture(GL_TEXTURE_2D, texID);
        GL_API_CHECK( glUniform1i(glGetUniformLocation(m_programID, name.c_str()), slot) );
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
        GL_API_CHECK( glUniform3fv(glGetUniformLocation(m_programID, name.c_str()), 1, &value[0]) );
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
       GL_API_CHECK( glUniformMatrix3fv(glGetUniformLocation(m_programID, name.c_str()), 1, GL_FALSE, &mat[0][0]) );
    }
    void SetMat4(const std::string& name, const glm::mat4& mat) const
    {
        GL_API_CHECK( glUniformMatrix4fv(glGetUniformLocation(m_programID, name.c_str()), 1, GL_FALSE, &mat[0][0]) );
    }
};
