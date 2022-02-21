//=================================================================================//
// Copyright (c) 2021 Yaochuang Ding 
//=================================================================================//

#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "objloader.h"
#include "simpleMesh.h"

#include "camera.h" 
#include "viewer.h" 


#include "event_handler.h" 
#include "gui.h"

float myRandom();

void drawOverlay(RenderSetting &setting);
void drawHelp(RenderSetting &setting);

template<typename MeshType>
std::unique_ptr<Camera> buildCamera(const MeshType & mesh)
{
    auto cameraTarget   = mesh.Center();
    auto cameraDistance = mesh.LogestDim() * 1.382f;
    return std::unique_ptr<Camera>{new Camera(cameraTarget, cameraDistance)};
}

void Viewer::Run()
{
    MeshBin meshes{ m_objPath };
    m_camera = buildCamera(meshes);

    m_counter.Initialize();
    m_counter.BindAsAtomicCounter(1);

    m_fragment_list.Initialize();
    m_fragment_list.BindBase(GL_SHADER_STORAGE_BUFFER, 2);

    glfwCallbackData cb{ m_camera.get(), m_animation_mode, GetRenderSetting(), GetDisplayOption()};
    glfwSetWindowUserPointer(m_window, &cb);

    GUI::Setup(m_window, "#version 130");
    do
    {
        animateCamera(*m_camera);
        render(meshes, SimpleMesh{}, *m_camera);
        glfwSwapBuffers(m_window);
        glfwPollEvents();
        glfwSwapInterval(m_enable_vsync); //< enable vsync
        m_frame_id++;
    } while(glfwGetKey(m_window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(m_window) == 0);

    GUI::CleanUp();
}

void Viewer::animateCamera(Camera &camera)
{
    switch (m_animation_mode)
    {
        case 0:
        {
            break;
        }
        case 1:
        {
            glm::vec2 delta{0.01, 0.0};
            camera.rotate(delta);
            break;
        }
        case 2:
        {
            float scale = -0.01;
            camera.zoom(scale); 
            break;
        }
        case 3:
        {
            float scale = 0.01;
            camera.zoom(scale); 
            break;
        }
        case 4:
        {
            glm::vec2 delta{ myRandom() * 0.05, myRandom() * 0.04 };
            camera.rotate(delta);
            break;
        }
        default:
        {
            break;
        }
    }

}

void Viewer::render(const MeshBin & meshBin, SimpleMesh &simplemesh, const Camera &camera)
{
    //glEnable(GL_MULTISAMPLE);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glFrontFace(GL_CCW);

    if (m_double_side_lighting)
    {
        /* This case, keep both front & back face triangle */
        //< clip a part to show internal
        glEnable(GL_CLIP_DISTANCE0);
        glDisable(GL_CULL_FACE);
    }
    else
    {
        glDisable(GL_CLIP_DISTANCE0);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }

    if(m_option.wireframe)
    {
        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    //renderMeshBin(meshBin, camera);
    voxelize(meshBin, camera);
    

    //< shall capture color buffer here
    if (m_capture_colorbuffer)
    {
        SaveScreen();
        m_capture_colorbuffer = false;
    }

    if (m_save_image_sequence)
    {
        SaveImageSequence();
    }

    //< draw ui after render
    drawUI(*this);
}

void Viewer::voxelize(const MeshBin& meshBin, const Camera& camera)
{
    glViewport(0, 0, 1024, 1024);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);

    m_counter.Reset();    
    m_fragment_list.BindBase(GL_SHADER_STORAGE_BUFFER, 2);
    auto& shaderParam = m_voxelShader.m_paramMap;
    SHADER_PARAM_SET_INT(shaderParam, "u_count_voxel_only", 1);

    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, m_voxelShader.Name().c_str());
        m_voxelShader.Apply();
        meshBin.DrawBins();
        int fragment_num = m_counter.SyncAndGetValue();
        m_fragment_list.MutableStorage(fragment_num * sizeof(GLuint) * 2, GL_DYNAMIC_DRAW);
        Log("Fragment count={}", fragment_num);
        Log("[VOXELIZER] : Allocate fragment list : {}voxels( {} MB)", fragment_num, m_fragment_list.GetByteCount() / float(1024 * 1024));

        SHADER_PARAM_SET_INT(shaderParam, "u_count_voxel_only", 0);
        m_voxelShader.Apply();
        meshBin.DrawBins();
    glPopDebugGroup();
}
void Viewer::renderMeshBin(const MeshBin& meshBin, const Camera& camera)
{
    glViewport(0, 0, 1024, 1024);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);

    glm::mat4 modelMatrix = glm::mat4(1.0);
    glm::mat4 viewMatrix = camera.viewMatrix();
    glm::mat4 projMatrix = camera.projMatrix();
    glm::vec2 windowSize(m_window_width, m_window_height);

    m_counter.Reset();
    auto& shaderParam = m_voxelShader.m_paramMap;
    SHADER_PARAM_SET_VEC2(shaderParam, "u_window_size", windowSize );
    SHADER_PARAM_SET_VEC3(shaderParam, "u_mesh_color", m_mesh_color);
    SHADER_PARAM_SET_VEC3(shaderParam, "u_eye_position", camera.eye());
    SHADER_PARAM_SET_INT(shaderParam, "u_show_wireframe", m_show_wireframe);
    //SHADER_PARAM_SET_MAT4(shaderParam, "M", modelMatrix);
    //SHADER_PARAM_SET_MAT4(shaderParam, "V", viewMatrix);
    //SHADER_PARAM_SET_MAT4(shaderParam, "P", projMatrix);

    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, m_voxelShader.Name().c_str());
    m_voxelShader.Apply();
        meshBin.DrawBins();
        int fragment_num = m_counter.SyncAndGetValue();
        Log("Fragment count={}", fragment_num);
        //m_fragment_list.Storage(fragment_num * sizeof(GLuint) * 2, 0);
        m_fragment_list.MutableStorage(fragment_num * sizeof(GLuint) * 2, GL_DYNAMIC_DRAW);
        Log("[VOXELIZER] : Build Fragment List : {}voxels( {} MB)", fragment_num, m_fragment_list.GetByteCount() / float(1024 * 1024));
    glPopDebugGroup();

    if (m_visualize_normal)
    {
        visualizeVertexNormal(meshBin, camera);
    }
}

void Viewer::visualizeVertexNormal(const MeshBin& meshBin, const Camera& camera)
{
    glm::mat4 modelMatrix = glm::mat4(1.0);
    glm::mat4 viewMatrix = camera.viewMatrix();
    glm::mat4 projMatrix = camera.projMatrix();

    auto& shaderParam = m_vertex_normal_visualize.m_paramMap;
    SHADER_PARAM_SET_MAT4(shaderParam, "M", modelMatrix);
    SHADER_PARAM_SET_MAT4(shaderParam, "V", viewMatrix);
    SHADER_PARAM_SET_MAT4(shaderParam, "P", projMatrix);
    SHADER_PARAM_SET_FLOAT(shaderParam, "u_normal_visualize_scale", m_normal_visualize_scale);
    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, m_vertex_normal_visualize.Name().c_str());
        m_vertex_normal_visualize.Apply();
        meshBin.DrawBins();
    glPopDebugGroup();
}

void Viewer::initOpenGLShaders()
{
    m_voxelShader.Init(
        "shaders/voxel.vs.glsl",
        "shaders/voxel.fs.glsl",
        nullptr,
        nullptr,
        "shaders/voxel.gs.glsl"
        );

    m_vertex_normal_visualize.Init(
        "shaders/VertexNormalVisualize.vs.glsl",
        "shaders/VertexNormalVisualize.fs.glsl",
        nullptr,
        nullptr,
        "shaders/VertexNormalVisualize.gs.glsl"
        );

}

int Viewer::initWindow()
{
    if(!glfwInit())
    {
        throw std::runtime_error("Failed to initialize GLFW !");
        return -1;
    }

    //glfwWindowHint(GLFW_SAMPLES, 4); //< MSAA
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    m_window = glfwCreateWindow(m_window_width, m_window_height, m_window_title.c_str(), NULL, NULL);
    if(m_window == NULL)
    {
        throw std::runtime_error("Failed to open GLFW window!");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(m_window); //< pick context of window into current thread

    glfwSwapInterval(1); //< enable vsync

    glfwSetCursorPos(m_window, m_window_width / 2.f, m_window_height / 2.f);
    glfwSetKeyCallback(m_window, glfwindow_key_cb);
    glfwSetMouseButtonCallback(m_window, glfwindow_mouseButton_cb);
    glfwSetCursorPosCallback(m_window, glfwindow_mouseMotion_cb);

    glewExperimental = true; //< defined in glew
    if(glewInit() != GLEW_OK)
    {
        throw std::runtime_error("Failed to initialize GLEW !");
        return -1;
    }

    //< There are bugs in glewInit(), it would set opengl error code 
    //< The following is an workaround
    ClearOpenGLError(__FILE__, __LINE__); 

    return 0;
}

#define STB_IMAGE_WRITE_IMPLEMENTATION 
#include "stb_image_write.h" 

void Viewer::SavePng(const std::string filename)
{
    /**
     * https://www.khronos.org/opengl/wiki/Framebuffer#Read_color_buffer
     *
     **/

    const int kSize = m_window_height * m_window_width;
    std::vector<GLfloat> pixels((size_t)kSize * 3);
    GL_API_CHECK(glReadPixels(0, 0, m_window_width, m_window_height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data()));
    stbi_flip_vertically_on_write(1);
    stbi_write_png(filename.c_str(), m_window_width, m_window_height, 3, pixels.data(), m_window_width * 3); //< only 3 channels
    Log("save color buffer into : {}", filename.c_str());
}

void Viewer::SaveScreen(const std::string filename)
{
    /**
     * https://www.khronos.org/opengl/wiki/Framebuffer#Read_color_buffer
     *
     **/
    const int kSize = m_window_height * m_window_width;
    std::vector<GLfloat> pixels((size_t)kSize * 3);
    glReadPixels(0, 0, m_window_width, m_window_height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
    GLenum errorCheckValue = glGetError();
    if (errorCheckValue != GL_NO_ERROR)
    {
        //fprintf(stderr, "Error: Could not read color buffer: %s\n", gluErrorString(errorCheckValue));
        Err("Could not read color buffer: {}", gluErrorString(errorCheckValue));
    }

    stbi_write_tga(filename.c_str(), m_window_width, m_window_height, 3, pixels.data()); //< only 3 channels
    Log("save color buffer into : {}", filename);
}

void Viewer::SaveImageSequence(const std::string dir)
{
    std::ostringstream  iss;
    iss << std::setw(4) << std::setfill('0') << m_sequence_count;
    SaveScreen(iss.str() + ".tga");
    m_sequence_count++;
}

///////////////////////////////////////////////////////////////////////////////////////
/////
///// UI
/////
///////////////////////////////////////////////////////////////////////////////////////

#include "IconsFontAwesome4.h"

static bool drawTextureDropList(const std::string & label,
    const std::vector<std::string> &textureNames,
    std::string &pickedTextureName
    )
{
    ImGui::Text(label.c_str());
    ImGui::SameLine();
    if (ImGui::BeginCombo(("##combo_" + label).c_str(), pickedTextureName.c_str())) // The second parameter is the label previewed before opening the combo.
    {
        bool picked = false;
        for (auto i = 0; i < textureNames.size(); i++)
        {
            bool is_selected = (pickedTextureName == textureNames[i]); // You can store your selection however you want, outside or inside your objects
            if (ImGui::Selectable(textureNames[i].c_str(), is_selected))
            {
                pickedTextureName = textureNames[i];
                picked = true;
            }
            if (is_selected)
                    ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
        }
        ImGui::EndCombo();
        if (picked)
        {
            Log("Pick Texture : {}", pickedTextureName);
            return true; //< pick something
        }
    }
    return false; //< pick nothing
}

static void drawUI(Viewer &viewer)
{
    RenderSetting &setting = viewer.m_setting;
    DisplayOption &displayOption = viewer.m_option;

    if (!displayOption.showUI)
    {
        return;
    }
    GUI::BeginFrame();

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 5.0f));
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu(ICON_FA_CUBE " File"))
        {
            if (ImGui::MenuItem(ICON_FA_FOLDER_OPEN " Open..."))
            {
            }
            if (ImGui::MenuItem(ICON_FA_FILM " Save..."))
            {
                viewer.m_capture_colorbuffer = true;
            }
            
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu(ICON_FA_CAMERA " Camera"))
        {
            const char *play = ICON_FA_PLAY " Start Image Sequence";
            const char *stop = ICON_FA_STOP " Stop  Image Sequence";
            if (ImGui::MenuItem(viewer.m_save_image_sequence ? stop : play))
            {
                viewer.m_save_image_sequence = !viewer.m_save_image_sequence;
                viewer.m_sequence_count = 0;
                if (viewer.m_save_image_sequence)
                {
                    Log("start Image sequence.");
                }
                else
                {
                    Log("stop Image sequence.");
                }
            }
            ImGui::Separator();
            if (ImGui::MenuItem(ICON_FA_FILM "Capture"))
            {
                Log("Capture screen.");
                viewer.SavePng(SCREEN_CAPTURE_IMAGE);
            }
            ImGui::Separator();
            ImGui::SliderFloat("Clip plane distance", &viewer.m_clip_plane_distance, -0.5f, 0.5f);
            ImGui::Separator();
            ImGui::Checkbox("Double side lighting", &viewer.m_double_side_lighting);
            ImGui::Separator();
            ImGui::Checkbox("Visualize normal", &viewer.m_visualize_normal);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu(ICON_FA_EYE " View"))
        {
            ImGui::Checkbox("ShowUI", &displayOption.showUI);
            ImGui::Checkbox("ShowHelp", &setting.showHelpTip);
            ImGui::Separator();
            ImGui::Checkbox("Wireframe", &displayOption.wireframe);
            ImGui::Separator();
            ImGui::SliderFloat("Normal Visualize Scale", &viewer.m_normal_visualize_scale, 0.f, 0.1f);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu(ICON_FA_WINDOWS " Settings"))
        {
            ImGui::Checkbox("VSync", &viewer.m_enable_vsync);
            ImGui::Separator();
            ImGui::ColorEdit3("Mesh Color", &viewer.m_mesh_color.x);
            ImGui::Separator();
            ImGui::Checkbox("Wireframe on Shaded", &viewer.m_show_wireframe);
            ImGui::Separator();
            
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
    ImGui::PopStyleVar();

    drawHelp(setting);
    drawOverlay(setting);

    GUI::EndFrame();
}

static void drawOverlay(RenderSetting &setting)
{
    ImGui::SetNextWindowPos(ImVec2(10.0f, ImGui::GetIO().DisplaySize.y - 10.0f), ImGuiCond_Always, ImVec2(0, 1));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.4f)); // Transparent background
    if (ImGui::Begin("StatusOverlay", nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus))
    {
        ImGui::Text("Toggle UI display with key [ x | X ]");
        ImGui::Text("Renderer : %s", glGetString(GL_RENDERER));
        ImGui::Text("OpenGL version : %s", glGetString(GL_VERSION));
        ImGui::Text("outer level : %2f, %.2f, %.2f", setting.outerTessLevel.x, setting.outerTessLevel.y, setting.outerTessLevel.z);
        ImGui::Text("inner level : %.2f", setting.innerTessLevel.x);
        ImGui::Text("fps : %.2f fps", ImGui::GetIO().Framerate);
        ImGui::End();
    }
    ImGui::PopStyleColor();
}

static void drawHelp(RenderSetting &setting)
{
    if (!setting.showHelpTip) return;
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x / 2.f, ImGui::GetIO().DisplaySize.y / 2.f), ImGuiCond_Always, ImVec2(0, 1));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.4f)); // Transparent background
    if (ImGui::Begin("HelpOverlay", nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus))
    {
        ImGui::Text("Help    : ");
        ImGui::Text("Key stroke  : ");
        ImGui::Text("         ' ' : loop animation mode ");
        ImGui::End();
    }
    ImGui::PopStyleColor();
}

///////////////////////////////////////////////////////////////////////////////////////
/////
///// 
/////
///////////////////////////////////////////////////////////////////////////////////////
#include <random>

static std::default_random_engine generator;
static std::uniform_real_distribution<float> distribution(-1.f, 1.f); //from -1 to 1

static float myRandom()
{
    return distribution(generator);
}
