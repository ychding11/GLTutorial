//=================================================================================//
// Copyright (c) 2021 Yaochuang Ding 
//=================================================================================//

#include "shader.h"

void outputVertexAttributes(GLuint prog)
{
    GLint active_attribs, max_length;
    glGetProgramiv(prog, GL_ACTIVE_ATTRIBUTES, &active_attribs);
    glGetProgramiv(prog, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &max_length);

    Log("Vertex attribute table:");
    std::vector<GLchar> name(max_length, 0);
    for (unsigned i = 0; i < active_attribs; i++)
    {
        GLint size;
        GLenum type;
        glGetActiveAttrib(prog, i, max_length + 1, NULL, &size, &type, name.data());
        Log("\t {} is at location {}", name.data(), glGetAttribLocation(prog, name.data()));
    }
}

void Shader::Init(const char* vs_file_path, const char* fs_file_path,
    const char* tcs_file_path, const char* tes_file_path,
    const char* gs_file_path)
{  
    m_programID = LoadShaders(vs_file_path, fs_file_path, tcs_file_path, tes_file_path, gs_file_path);
    m_initialized = true;
    Log("{} Shader Link done.", m_name);
    outputVertexAttributes(m_programID);

    GLint count = 0;
    GLint maxNameLength = 0;
    glGetProgramiv(m_programID, GL_ACTIVE_UNIFORMS, &count);
    glGetProgramiv(m_programID, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxNameLength);
    std::vector<GLchar> name(maxNameLength, 0);
    for (int i = 0; i < count; i++)
    {
        GLsizei length = 0; //< actual name length (exclude null terminator)
        GLint size = 0; //< size of the uniform variable
        GLenum type = 0; //<data type of the uniform variable
        glGetActiveUniform(m_programID, i, maxNameLength, &length, &size, &type, name.data());
        //std::string paramName(name.begin(), name.end()-1);
        std::string paramName(name.data());
        if (type == GL_FLOAT_MAT4)
        {
            m_paramMap[paramName].name = paramName;
            m_paramMap[paramName].type = ShaderParamType::Matrix;
            m_paramMap[paramName].demension = 4;
            m_paramMap[paramName].data = new glm::mat4();
        }
        else if (type == GL_FLOAT_VEC2)
        {
            m_paramMap[paramName].name = paramName;
            m_paramMap[paramName].type = ShaderParamType::Vector;
            m_paramMap[paramName].demension = 2;
            m_paramMap[paramName].data = new glm::vec2();
        }
        else if (type == GL_FLOAT_VEC3)
        {
            m_paramMap[paramName].name = paramName;
            m_paramMap[paramName].type = ShaderParamType::Vector;
            m_paramMap[paramName].demension = 3;
            m_paramMap[paramName].data = new glm::vec3();
        }
        else if (type == GL_FLOAT_VEC4)
        {
            m_paramMap[paramName].name = paramName;
            m_paramMap[paramName].type = ShaderParamType::Vector;
            m_paramMap[paramName].demension = 4;
            m_paramMap[paramName].data = new glm::vec4();
        }
        else if (type == GL_FLOAT)
        {
            m_paramMap[paramName].name = paramName;
            m_paramMap[paramName].type = ShaderParamType::Vector;
            m_paramMap[paramName].demension = 1;
            m_paramMap[paramName].data = new float(0);
        }
        else if (type == GL_INT)
        {
            m_paramMap[paramName].name = paramName;
            m_paramMap[paramName].type = ShaderParamType::Scalar;
            m_paramMap[paramName].data = new int(0);
        }
        else if (type == GL_SAMPLER_2D)
        {
            GLint index = glGetUniformLocation(m_programID, paramName.c_str());

            m_paramMap[paramName].name = paramName;
            m_paramMap[paramName].type = ShaderParamType::Tex2D;
            m_paramMap[paramName].data = new GLuint(0);
            m_paramMap[paramName].slot = index; //< can we treat it as a texture slot ?
        }
        else
        {
            Warn("Not supported data type");
        }
    }
}

void Shader::Apply() const 
{
    GL_API_CHECK( glUseProgram(m_programID) );
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

