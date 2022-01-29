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
#include "config.h"

class MeshBin;

class Viewer;

void drawUI(Viewer &viewer);

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

    int m_window_width = 1280;
    int m_window_height = 720;
    GLFWwindow *m_window{nullptr};

    std::string m_window_title{ "Double side shading" };
    bool m_capture_colorbuffer{false};
    bool m_save_image_sequence{false};
    bool m_enable_vsync{false};
    bool m_enable_normal_map{true};
    bool m_enable_displace_map{false};
    bool m_simple_mesh_mode{true}; //< two modes : simple mesh mode, obj mesh mode
    bool m_double_side_lighting{true}; //< two modes : simple mesh mode, obj mesh mode

    int  m_sequence_count{ 0 }; //< animation
    int  m_animation_mode{ 0 }; //< animation
    int  m_frame_id{ 0 };

    float m_clip_plane_distance{0.0f};

    glm::vec3 m_mesh_color{0.6, 0.5, 0.4};

    RenderSetting &m_setting;
    DisplayOption &m_option;

    TextureID m_diffuse_TexID{ INVALIDE_TEXTURE_ID };
    TextureID m_normal_TexID{ INVALIDE_TEXTURE_ID };
    TextureID m_displace_TexID{ INVALIDE_TEXTURE_ID };

    friend void drawUI(Viewer &viewer);

private:

    Material m_brickWall{"brickwall"};
    Material m_light_material{"emissive"};

    std::unique_ptr<Quad> m_quad;
    std::unique_ptr<Cube> m_cube;
    SimpleMesh* m_active_simpleMesh{nullptr};
    std::string m_picked_mesh_name{"Cube"};
    std::vector<std::string> m_simpleMesh_types{"Quad", "Cube"};

    std::unique_ptr<PointLight> m_pointLight;
    std::unique_ptr<Camera>     m_camera;

public:
    Viewer() = delete;
    ~Viewer()
    {
        glfwTerminate();
    }

    Viewer(std::string title, int width, int height)
        : m_window_title(title)
        , m_window_width(width)
        , m_window_height(height)
        , m_setting(GetRenderSetting())
        , m_option(GetDisplayOption())
    {
        initWindow();
        initOpenGLShaders();
        initMaterials();
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
    void initMaterials(void);
    void render(const MeshBin &meshBin, SimpleMesh& simplemesh, const Camera &camera);
    void animateCamera(Camera &camera);
    void renderSimpleMesh(SimpleMesh& simplemesh, const Camera& camera);
    void renderLight(Light& light, const Camera& camera);
};
