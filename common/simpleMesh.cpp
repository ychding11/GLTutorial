//=================================================================================//
// Copyright (c) 2021 Yaochuang Ding 
//=================================================================================//

#include "simpleMesh.h"

//<  counter-clockwise winding order

static float s_cubeVertices[] =
    {
        //< Position, texture coordinate
        // Back face
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // Bottom-left
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // bottom-right         
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
        // Front face
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // top-left
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
        // Left face
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-left
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
        // Right face
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right         
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left     
        // Bottom face
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // top-left
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
        // Top face
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right     
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f  // bottom-left        
    };


    void Cube::convertCubeFace(glm::vec3 &nm, int faceid)
    {
        float *face = s_cubeVertices + faceid * int(5 * 6);
        glm::vec3 positions[6];
        glm::vec2 uvs[6];
        int offset = 5;
        for (int i = 0; i < 6; ++i)
        {
            positions[i] = glm::vec3{
                face[0+i*offset],
                face[1+i*offset],
                face[2+i*offset],
            };
            uvs[i] = glm::vec2{
                face[3+i*offset],
                face[4+i*offset],
            };
        }

        auto converter = [positions, uvs, nm, this](int triangleid)
        {

            glm::vec3 tangent1;

            glm::vec3 pos1 = positions[0 + triangleid * 3];
            glm::vec3 pos2 = positions[1 + triangleid * 3];
            glm::vec3 pos3 = positions[2 + triangleid * 3];

            glm::vec2 uv1 = uvs[0 + triangleid * 3];
            glm::vec2 uv2 = uvs[1 + triangleid * 3];
            glm::vec2 uv3 = uvs[2 + triangleid * 3];

            // triangle 
            glm::vec3 edge1 = pos2 - pos1;
            glm::vec3 edge2 = pos3 - pos1;
            glm::vec2 deltaUV1 = uv2 - uv1;
            glm::vec2 deltaUV2 = uv3 - uv1;

            float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
            tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
            tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
            tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

            m_vertices.push_back({ pos1, nm, tangent1, uv1 });
            m_vertices.push_back({ pos2, nm, tangent1, uv2 });
            m_vertices.push_back({ pos3, nm, tangent1, uv3 });

            m_aabb.Extend(pos1);
            m_aabb.Extend(pos2);
            m_aabb.Extend(pos3);
        };
        converter(0); //< first triangle in quad
        converter(1); //< second triangle in quad
    }

    Cube::Cube ()
    {
        convertCubeFace(glm::vec3{0,0,-1}, 0); //back face
        convertCubeFace(glm::vec3{0,0, 1}, 1); //front face
        convertCubeFace(glm::vec3{-1,0, 0}, 2); //left face
        convertCubeFace(glm::vec3{ 1,0, 0}, 3); //right face
        convertCubeFace(glm::vec3{0,-1, 0}, 4); //bottom face
        convertCubeFace(glm::vec3{0, 1, 0}, 5); //top face

        m_vb_size_in_bytes = m_vertices.size() * sizeof(VertexAttribute);
        create_vao();
    }
