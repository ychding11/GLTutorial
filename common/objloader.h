//=================================================================================//
// Copyright (c) 2021 Yaochuang Ding 
//=================================================================================//

#ifndef OBJLOADER_H
#define OBJLOADER_H

#include <glm/glm.hpp>

#include <GL/glew.h>

#include <vector>

#include "aabb.h" 

class MeshBin
{
    struct SimpleVertex
    {
        glm::vec3 position, normal;
    };

    struct Mesh
    {
        std::vector<SimpleVertex> vertices;

    };

private:
    //const GLuint m_max_mesh_num = 256;
    const GLuint m_max_mesh_num;
    size_t m_mesh_num{ 0 };
    std::vector<GLuint> m_vao_id{m_max_mesh_num, 0};
    std::vector<GLuint> m_vbo_id{m_max_mesh_num, 0};
    std::vector<size_t> m_vb_size{m_max_mesh_num, 0};
    std::vector<size_t> m_vertex_num{m_max_mesh_num, 0};

    std::vector<Mesh> m_meshes; //< binned meshes

    AABB m_aabb;

    GLuint m_instanceVBO{ 0 };
    int m_instance_stride{ 0 };
    int m_instance_count{ 0 };
    int m_instance_buffer_byte_size{ 0 };

public:
    MeshBin() = delete;

    MeshBin(const std::string &filename);

    ~MeshBin()
    {
        for (int i = 0; i < m_mesh_num; i++)
        {
            glDeleteBuffers(1, &m_vbo_id[i]);
            glDeleteVertexArrays(1, &m_vao_id[i]);
        }
    }

    const AABB& Aabb() const
    {
        return m_aabb;
    }
    glm::vec3 Center() const
    {
        return m_aabb.Center();
    }

    float LogestDim() const
    {
        return m_aabb.LongestEdge();
    }

    size_t size() const
    {
        return m_mesh_num;
    }

    size_t vertex_num(int index) const
    {
        return m_vertex_num[index];
    }

    GLuint vao(int index) const
    {
        return m_vao_id[index];
    }

    GLuint instance_count() const
    {
        return m_instance_count;
    }

    void DrawBins() const;

private:

    void create_vaos();
    AABB loadObjModel(const std::string& filename, const std::string& base_dir, std::vector<Mesh>& meshes);
    void MeshBin::init_instace_buffer();
};


#endif
