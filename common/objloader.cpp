//=================================================================================//
// Copyright (c) 2021 Yaochuang Ding 
//=================================================================================//

#include <vector>
#include <stdio.h>
#include <string>
#include <sstream>
#include <cstring>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "objloader.h"
#include "Log.h"

AABB MeshBin::loadObjModel(const std::string &filename, const std::string &base_dir, std::vector<Mesh> &meshes)
{
    std::stringstream ss;
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::material_t> materials;
    std::vector<tinyobj::shape_t> shapes;

    AABB aabb;

    std::string err;
    if ( !tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename.c_str(), base_dir.c_str()) )
    {
        Err("Fail to load obj model : {}", filename);
        return aabb;
    }

    if (materials.size() == 0)
    {
        Err("No material found in obj model : {}", filename);
        return aabb;
    }

    //< There is a bug here: err may contain multiple '\n' terminated string
    //< Yaochuang's Plan: Research how to output multiple line log by spdlog
    if (!err.empty())
        printf("%s", err.c_str());

    meshes.resize(materials.size());

    //< is this macro: FLT_MAX OS dependent ?
    //< should always prefer os independent ones
    glm::vec3 pmin(FLT_MAX), pmax(-FLT_MAX);

    // Loop over shapes
    for (const auto &shape : shapes)
    {
        size_t index_offset = 0, face = 0;

        //< Loop over all faces(polygon) in a mesh. vertex number of face maybe variable
        //< for triangle num_face_vertices = 3
        for (const auto &num_face_vertex : shape.mesh.num_face_vertices)
        {
            int mat = shape.mesh.material_ids[face];
            // Loop over triangles in the face.
            for (size_t v = 0; v < num_face_vertex; ++v)
            {
                tinyobj::index_t index = shape.mesh.indices[index_offset + v];

                meshes.at(mat).vertices.emplace_back(); //< use "material id" to index mesh : mesh <--> material id is one-one map
                auto &vert = meshes.at(mat).vertices.back();
                vert.position =
                {
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2]
                };
                pmin = glm::min(vert.position, pmin);
                pmax = glm::max(vert.position, pmax);
                if (~index.normal_index) //< -1 == 0xFFFFFFFF, it is equal to if (index.normal_index != -1)
                {

                    vert.normal =
                    {
                        attrib.normals[3 * index.normal_index + 0],
                        attrib.normals[3 * index.normal_index + 1],
                        attrib.normals[3 * index.normal_index + 2]
                    };
                }
                else
                {
                    throw std::runtime_error("No normal channel found in vertex");
                    return aabb;
                }

            }
            index_offset += num_face_vertex;
            face++;
        }
    }

    aabb.Extend(pmin);
    aabb.Extend(pmax);

    ss << "After binning," << meshes.size() << " meshes constructed(each mesh contains only one material)" << "\n"
       << "Obj Model file : " << filename << "\n"
       << aabb.str()
    ;

    Log("{}", ss.str());

#if 0
    //< normalize all the vertex to [-1, 1]
    glm::vec3 extent3 = pmax - pmin;
    float extent = glm::max(extent3.x, glm::max(extent3.y, extent3.z)) * 0.5f;
    float inv_extent = 1.0f / extent;

    glm::vec3 center = (pmax + pmin) * 0.5f;

    for (auto &m : meshes)
        for (auto &v : m.vertices)
            v.position = (v.position - center) * inv_extent;

    Log("[SCENE]: load {} meshes from file {}\t vertex position normalized to [-1, 1]", meshes.size(), filename);

#endif

    return aabb;
}

///////////////////////////// Class MeshBin //////////////////////////////////////////////

    MeshBin::MeshBin(const std::string &filename)
        : m_max_mesh_num(256)
    {
        m_aabb = loadObjModel(filename, "Model/", m_meshes);
        if (m_aabb.Empty())
        {
            Err("AABB is empty, exit!");
            exit(1);
        }
        else
        {
            Log("mesh AABB Center is {}", glm::to_string(m_aabb.Center()));
        }
        init_instace_buffer();
        create_vaos();
    }

    void MeshBin::create_vaos()
    {
        for (int i = 0; i < m_meshes.size(); ++i)
        {
            GLenum errorCheckValue = glGetError();

            m_vb_size[m_mesh_num] = m_meshes[i].vertices.size() * sizeof(SimpleVertex);
            m_vertex_num[m_mesh_num] = m_meshes[i].vertices.size();

            const size_t vertexStride = sizeof(SimpleVertex);
            const size_t normalOffset = sizeof(m_meshes[i].vertices[0].position);

            glGenVertexArrays(1, &m_vao_id[m_mesh_num]);
            glBindVertexArray(m_vao_id[m_mesh_num]);

            glGenBuffers(1, &m_vbo_id[m_mesh_num]);
            glBindBuffer(GL_ARRAY_BUFFER, m_vbo_id[m_mesh_num]);
            glBufferData(GL_ARRAY_BUFFER, m_vb_size[m_mesh_num], m_meshes[i].vertices.data(), GL_STATIC_DRAW);

            //< 
            //< It is a composed API :
            //<   - specify generic vertex attribute format
            //<   - specify generic vertex attribute stride 
            //<   - specify vertex buffer binding 
            //<   - specify mapping between {vertex buffer binding, vertex attribute}
            //<     - vertex attribute index <--> buffer currently bound to GL_ARRAY_BUFFER
            //< See details on page 344 of glspec45.core.pdf
            glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, vertexStride, 0);
            glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, vertexStride, (GLvoid*)normalOffset);

            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);

            glBindBuffer(GL_ARRAY_BUFFER, m_instanceVBO);
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, m_instance_stride, (void*)0);
            glEnableVertexAttribArray(2);
            glVertexAttribDivisor(2, 1); //< vertex attribute 2 advances once per instance
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindVertexArray(0);

            m_mesh_num++;
        }
    }

    void MeshBin::init_instace_buffer()
    {
        glm::vec3 translations[100];
        m_instance_stride = sizeof(glm::vec3);
        m_instance_count = 0;
        float offset = 0.1f;
        for (int z = -10; z < 10; z += 2)
        {
            for (int x = -10; x < 10; x += 2)
            {
                glm::vec3 translation;
                translation.x = (float)x / 10.0f + offset;
                translation.z = (float)z / 10.0f + offset;
                translation.y = 0.f;
                translations[m_instance_count++] = translation;
            }
        }
        m_instance_buffer_byte_size = m_instance_stride * m_instance_count;

        glGenBuffers(1, &m_instanceVBO);
        glBindBuffer(GL_ARRAY_BUFFER, m_instanceVBO);
        glBufferData(GL_ARRAY_BUFFER, m_instance_buffer_byte_size, &translations[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void MeshBin::DrawBins() const
    {
        for (int i = 0; i < this->size(); ++i)
        {
            GL_API_CHECK( glBindVertexArray( this->vao(i) ) );
            GL_API_CHECK( glDrawArrays( GL_TRIANGLES, 0, this->vertex_num(i) ) );
        }
    }
