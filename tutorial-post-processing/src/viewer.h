//=================================================================================//
// Copyright (c) 2021 Yaochuang Ding 
//=================================================================================//

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "camera.h"
#include "shader.h"
#include "material.h"
#include "simpleMesh.h"
#include "framebuffer.h"
#include "config.h"

#include "enum_str.h"

class MeshBin;

class Viewer;

void drawUI(Viewer &viewer);

MyEnum(PPFilter, None, Sharpen, Blur);

//<
//< It serve as a container for modules such as
//< 1. window management
//< 2. keyboard & mouse event handling 
//< 3. shader program management
//< 4. lighting & camera arrangement
//< 5. rendering & updating
//< 6. interact with user
//<
class Viewer
{
private:

    std::string m_window_title{ "Post-Processing" };
    int m_window_width = 1280;
    int m_window_height = 720;
    GLFWwindow *m_window{nullptr};

    bool m_capture_colorbuffer{false};
    bool m_save_image_sequence{false};
    bool m_enable_vsync{false};
    bool m_double_side_lighting{false}; //
    bool m_visualize_normal{false}; 

    int  m_sequence_count{ 0 }; //< animation
    int  m_animation_mode{ 0 }; //< animation
    int  m_frame_id{ 0 };

    float m_clip_plane_distance{0.0f};
    float m_normal_visualize_scale{0.01f};

    glm::vec3 m_mesh_color{1.0 };

    std::string m_picked_pp{ "Blur" };
    PPFilter m_filter_type{Blur};

    RenderSetting &m_setting;
    DisplayOption &m_option;

    GLuint m_empty_vao{0}; //< in core profile, we need an explict empty vao, 0 means nothing

    friend void drawUI(Viewer &viewer);

private:

    Framebuffer m_framebuffer;
    Shader m_solidWireframeShader{"wireframe"};
    Shader m_full_screen_Shader{"full_screen"};
    Shader m_vertex_normal_visualize_shader{"vertex_normal_visualize"};

    std::unique_ptr<Camera>     m_camera;

public:
    Viewer() = delete;
    ~Viewer()
    {
        glDeleteVertexArrays(1, &m_empty_vao);
        glfwTerminate();
    }

    Viewer(std::string title, int width, int height)
        : m_window_width(width)
        , m_window_height(height)
        , m_setting(GetRenderSetting())
        , m_option(GetDisplayOption())
    {
        if (m_window_title.empty())
            m_window_title = title;
        initWindow();
        initOpenGLShaders();
    }

    void SetModel(std::string modelPath)
    {
        m_objPath = modelPath;
    }

    void Run();

    void SaveScreen(const std::string filename = "pic.tga");
    void SavePng(const std::string filename = "test.png");
    void SaveImageSequence(const std::string dir = "imageseq");

private:
    std::string m_objPath;
    int initWindow(void);
    void initOpenGLShaders(void);
    void render(const MeshBin &meshBin, SimpleMesh& simplemesh, const Camera &camera);
    void animateCamera(Camera &camera);
    void renderMeshBin(const MeshBin& meshBin, const Camera& camera);
    void visualizeVertexNormal(const MeshBin& meshBin, const Camera& camera);
    void renderFullScreen();
};
