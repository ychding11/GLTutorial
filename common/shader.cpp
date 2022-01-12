//=================================================================================//
// Copyright (c) 2021 Yaochuang Ding 
//=================================================================================//

#include "shader.h"


void Shader::Apply() const 
{
    glUseProgram(m_programID);
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

