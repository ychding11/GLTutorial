//=================================================================================//
// Copyright (c) 2021 Yaochuang Ding 
//=================================================================================//

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GL/glew.h>

#include <vector>

#include "aabb.h" 
#include "Material.h" 
#include "Log.h" 

//< https://en.cppreference.com/w/cpp/types/offsetof
//< offsetof is conditionally supported from c++ 17
//<
#define MY_OFFSET(x, y)  &((x *)0)->y

struct VertexAttribute
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 tangent;
    //glm::vec3 bitangent;
    glm::vec2 texcoord;
};

class SimpleMesh
{
protected:
    std::vector<VertexAttribute> m_vertices;
    AABB m_aabb;

    GLuint  m_vao_id{0};
    GLuint  m_vbo_id{0};
    size_t  m_vb_size_in_bytes{0};

    Material* m_pMaterial{nullptr};
    glm::vec3 m_position{ 0.f };

public:

    Material* GetMaterial() const
    {
        return m_pMaterial;
    }

    void SetMaterial(Material *mat) 
    {
        m_pMaterial = mat;
    }

    void SetPosition(glm::vec3 position)
    {
        m_position = position;
    }
    glm::vec3 GetPosition() const
    {
        return m_position;
    }

    virtual glm::mat4 GetWorldMatrix() const
    {
        glm::mat4 world = glm::mat4(1.0f);
        world = glm::translate(world, m_position);
        return world;
    }

    size_t vertex_num() const
    {
        return m_vertices.size();
    }

    glm::vec3 Center() const
    {
        return m_aabb.Center();
    }

    float LogestDim() const
    {
        return m_aabb.LongestEdge();
    }

    size_t size_in_bytes() const
    {
        return m_vb_size_in_bytes;
    }

    GLuint vao() const
    {
        return m_vao_id;
    }
};


class Quad : public SimpleMesh 
{
private:

public:
    Quad()
    {
        glm::vec3 pos1(-1.0f, 1.0f, 0.0f);
        glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
        glm::vec3 pos3(1.0f, -1.0f, 0.0f);
        glm::vec3 pos4(1.0f, 1.0f, 0.0f);
        // texture coordinates
        glm::vec2 uv1(0.0f, 1.0f);
        glm::vec2 uv2(0.0f, 0.0f);
        glm::vec2 uv3(1.0f, 0.0f);
        glm::vec2 uv4(1.0f, 1.0f);
        // normal vector
        glm::vec3 nm(0.0f, 0.0f, 1.0f);

        // calculate tangent/bitangent vectors of both triangles
        glm::vec3 tangent1, bitangent1;
        glm::vec3 tangent2, bitangent2;
        // triangle 1
        // ----------
        glm::vec3 edge1 = pos2 - pos1;
        glm::vec3 edge2 = pos3 - pos1;
        glm::vec2 deltaUV1 = uv2 - uv1;
        glm::vec2 deltaUV2 = uv3 - uv1;

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);


        // triangle 2
        // ----------
        edge1 = pos3 - pos1;
        edge2 = pos4 - pos1;
        deltaUV1 = uv3 - uv1;
        deltaUV2 = uv4 - uv1;

        f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

        m_vertices.push_back({pos1, nm, tangent1, uv1});
        m_vertices.push_back({pos2, nm, tangent1, uv2});
        m_vertices.push_back({pos3, nm, tangent1, uv3});

        m_vertices.push_back({pos1, nm, tangent2, uv1});
        m_vertices.push_back({pos3, nm, tangent2, uv3});
        m_vertices.push_back({pos4, nm, tangent2, uv4});

        m_aabb.Extend(pos1);
        m_aabb.Extend(pos2);
        m_aabb.Extend(pos3);
        m_aabb.Extend(pos4);
        m_vb_size_in_bytes = m_vertices.size() * sizeof(VertexAttribute);

        create_vao();
    }

    ~Quad()
    {
        {
            glDeleteBuffers(1, &m_vbo_id);
            glDeleteVertexArrays(1, &m_vao_id);
        }
    }

private:

    void create_vao()
    {
        const size_t vertexStride = sizeof(VertexAttribute);
        GLvoid* normalOffset  = MY_OFFSET(struct VertexAttribute, normal);
        GLvoid* tangentOffset = MY_OFFSET(struct VertexAttribute, tangent);
        GLvoid* texcoordOffset  = MY_OFFSET(struct VertexAttribute, texcoord);

        glGenVertexArrays(1, &m_vao_id);
        glBindVertexArray(m_vao_id);

        glGenBuffers(1, &m_vbo_id);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo_id);
        glBufferData(GL_ARRAY_BUFFER, m_vb_size_in_bytes, m_vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexStride, 0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertexStride, (GLvoid*)normalOffset);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, vertexStride, (GLvoid*)tangentOffset);
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, vertexStride, (GLvoid*)texcoordOffset);

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glEnableVertexAttribArray(3);

        glBindVertexArray(0);

        GLenum errorCheckValue = glGetError();
        if (errorCheckValue != GL_NO_ERROR)
        {
            Err("Quad: Could not create a VBO: {}", gluErrorString(errorCheckValue));
        }
    }
};

class Cube : public SimpleMesh 
{
private:

    void convertCubeFace(glm::vec3& nm, int faceid);
public:
    Cube();

    ~Cube()
    {
        {
            glDeleteBuffers(1, &m_vbo_id);
            glDeleteVertexArrays(1, &m_vao_id);
        }
    }

private:

    void create_vao()
    {
        const size_t vertexStride = sizeof(VertexAttribute);
        GLvoid* normalOffset  = MY_OFFSET(struct VertexAttribute, normal);
        GLvoid* tangentOffset = MY_OFFSET(struct VertexAttribute, tangent);
        GLvoid* texcoordOffset  = MY_OFFSET(struct VertexAttribute, texcoord);

        glGenVertexArrays(1, &m_vao_id);
        glBindVertexArray(m_vao_id);

        glGenBuffers(1, &m_vbo_id);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo_id);
        glBufferData(GL_ARRAY_BUFFER, m_vb_size_in_bytes, m_vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexStride, 0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertexStride, (GLvoid*)normalOffset);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, vertexStride, (GLvoid*)tangentOffset);
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, vertexStride, (GLvoid*)texcoordOffset);

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glEnableVertexAttribArray(3);

        glBindVertexArray(0);

        GLenum errorCheckValue = glGetError();
        if (errorCheckValue != GL_NO_ERROR)
        {
            Err("Quad: Could not create a VBO: {}", gluErrorString(errorCheckValue));
        }
    }
};

/*
  - evaluate light property change
  - self draw into frame buffer
    - mesh
    - material
*/
class Light
{
public:
    Light() = delete;
    Light(glm::vec3 position, glm::vec3 color)
        : m_position(position)
        , m_color(color)
    {
        glm::mat4 world = glm::mat4(1.0f);
        world = glm::translate(world, m_position);
        world = glm::scale(world, glm::vec3(0.1f));
        m_world_matrix = world;
    }

    virtual const SimpleMesh* GetMesh() = 0;
    virtual Material* GetMaterial() = 0;
    virtual glm::vec3 GetPosition()
    {
        return m_position;
    }
    virtual glm::mat4 GetWorldMatrix()
    {
        if (false) //dirty check in future
        {
            glm::mat4 world = glm::mat4(1.0f);
            world = glm::translate(world, m_position);
            world = glm::scale(world, glm::vec3(0.06f));
            m_world_matrix = world;
        }
        return m_world_matrix;
    }

    virtual glm::vec3 GetColor()
    {
        return m_color;
    }
protected:

    glm::vec3 m_position;
    glm::vec3 m_color;
    glm::mat4 m_world_matrix;
};

class PointLight : public Light
{

public:
    PointLight() = delete;
    PointLight(glm::vec3 position, glm::vec3 color)
        : Light(position, color)
        , m_mesh(new Cube())
        , m_material(new Material("Light"))
    {
        m_material->Init("shaders/light.vs.glsl", "shaders/light.fs.glsl");
    }

    virtual const SimpleMesh* GetMesh() override
    {
        return m_mesh.get();
    }
    virtual Material* GetMaterial() override
    {
        return m_material.get();
    }

private:
    std::unique_ptr<SimpleMesh> m_mesh;
    std::unique_ptr<Material>   m_material;
};
