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
#include "shaderUtility.h"

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
    m_quad = std::make_unique<Quad>();
    m_cube = std::make_unique<Cube>();

    m_cube->SetMaterial(&m_brickWall);
    m_quad->SetMaterial(&m_brickWall);
    m_active_simpleMesh = m_quad.get();

    //< second stage init
    MeshBin meshes{ m_objPath };


    glm::vec3 cameraTarget;
    float cameraDistance;

    if (m_simple_mesh_mode)
    {
        m_camera = buildCamera(*m_active_simpleMesh);
    }
    else
    {
        m_camera = buildCamera(meshes);
    }

    //< it should lighting here
    m_pointLight = std::make_unique<PointLight>(
        glm::vec3{0.0, 0.1f , 0.3 * m_active_simpleMesh->LogestDim()}, glm::vec3{3.0f});

    glfwCallbackData cb{ m_camera.get(), m_animation_mode, GetRenderSetting(), GetDisplayOption()};
    glfwSetWindowUserPointer(m_window, &cb);

    GUI::Setup(m_window, "#version 130");
    do
    {
        evaluateDirtyValue();
        animateExplode();
        animateCamera(*m_camera);
        render(meshes, *m_active_simpleMesh, *m_camera);
    } while(glfwGetKey(m_window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(m_window) == 0);

    GUI::CleanUp();
}

void Viewer::evaluateDirtyValue()
{
    if (m_picked_mesh_name == "Quad")
    {
        m_active_simpleMesh = m_quad.get();
    }
    else if (m_picked_mesh_name == "Cube")
    {
        m_active_simpleMesh = m_cube.get();
    }
    else
    {

    }
}

void Viewer::animateExplode()
{
    
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

    if (m_simple_mesh_mode)
        renderSimpleMesh(simplemesh, camera);
    else
        renderMeshBin(meshBin, camera);
    
    glBindVertexArray(0);
    glUseProgram(0);

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

    glfwSwapBuffers(m_window);
    glfwPollEvents();
    glfwSwapInterval(m_enable_vsync); //< enable vsync
    m_frame_id++;
}

void Viewer::renderMeshBin(const MeshBin& m_meshBin, const Camera& m_camera)
{
    glm::mat4 modelMatrix = glm::mat4(1.0);
    glm::mat4 viewMatrix = m_camera.viewMatrix();
    glm::mat4 projMatrix = m_camera.projMatrix();

    m_standardShader.Active();
    m_standardShader.SetMat4("M", modelMatrix);
    m_standardShader.SetMat4("V", viewMatrix);
    m_standardShader.SetMat4("P", projMatrix);
    m_standardShader.SetVec2("u_window_size", {m_window_width, m_window_height});
    m_standardShader.SetVec3("u_mesh_color", m_mesh_color);
    m_standardShader.SetVec3("u_eye_position", m_camera.eye());
    for (int i = 0; i < m_meshBin.size(); ++i)
    {
        glBindVertexArray( m_meshBin.vao(i) );
        glDrawArrays( GL_TRIANGLES, 0, m_meshBin.vertex_num(i) );
        {
            GLenum errorCheckValue = glGetError();
            if (errorCheckValue != GL_NO_ERROR)
            {
                Err("Draw: {}", gluErrorString(errorCheckValue));
            }
        }
    }
}

void Viewer::renderLight(Light& light, const Camera& camera)
{
    glm::mat4 modelMatrix = light.GetWorldMatrix();
    glm::mat4 viewMatrix = camera.viewMatrix();
    glm::mat4 projMatrix = camera.projMatrix();

    auto* shader = light.GetMaterial()->GetShader();
    shader->Active();
    shader->SetMat4("M", modelMatrix);
    shader->SetMat4("V", viewMatrix);
    shader->SetMat4("P", projMatrix);
    shader->SetVec3("u_viewer_pos", camera.eye());

    shader->SetVec3("u_light_pos", light.GetPosition());
    shader->SetVec3("u_light_color", light.GetColor());

    glBindVertexArray( light.GetMesh()->vao() );
    glDrawArrays( GL_TRIANGLES, 0, light.GetMesh()->vertex_num() );
}

void Viewer::renderSimpleMesh(SimpleMesh& simplemesh, const Camera& camera)
{
    Material *mat = simplemesh.GetMaterial();
    Shader* shader = mat->GetShader();
    shader->Active();
    shader->SetMat4("M", simplemesh.GetWorldMatrix());
    shader->SetMat4("V", camera.viewMatrix());
    shader->SetMat4("P", camera.projMatrix());
    shader->SetVec3("u_viewer_pos", camera.eye());

    shader->SetVec3("u_light_pos",  m_pointLight->GetPosition());
    shader->SetVec3("u_light_color",m_pointLight->GetColor());

    shader->SetFloat("u_clip_plane_dist", m_clip_plane_distance);

    shader->SetInt("diffuseMap", 0);
    shader->SetInt("normalMap", 1);
    if (m_enable_normal_map) //< control from view
    shader->SetInt("use_normal_map", 1);
    else
    shader->SetInt("use_normal_map", 0);
    mat->ActiveDiffuseMap();
    mat->ActiveNormalMap();
    glBindVertexArray( simplemesh.vao() );
    glDrawArrays( GL_TRIANGLES, 0, simplemesh.vertex_num() );
    GLenum errorCheckValue = glGetError();
    if (errorCheckValue != GL_NO_ERROR)
    {
        //Err("Draw: {}", gluErrorString(errorCheckValue));
    }

    renderLight(*m_pointLight, camera);
}

void Viewer::initOpenGLShaders()
{
    m_standardShader.Init("shaders/SolidWireframe.vs.glsl",
        "shaders/SolidWireframe.fs.glsl",
        nullptr,
        nullptr,
        "shaders/SolidWireframe.gs.glsl"
        );
}

void Viewer::initMaterials(void)
{
    //< shader cache someday
    m_brickWall.Init("shaders/brickWall.vs.glsl", "shaders/brickWall.fs.glsl");
    m_light_material.Init("shaders/light.vs.glsl", "shaders/light.fs.glsl");

    GetTextureCache().Add("textures/brickwall_diffuse.jpg");
    GetTextureCache().Add("textures/brickwall_normal.jpg");

    GetTextureCache().Add("textures/bricks2_diffuse.jpg");
    GetTextureCache().Add("textures/bricks2_normal.jpg");
    GetTextureCache().Add("textures/bricks2_disp.jpg");

    m_brickWall.m_diffuseTexName = "bricks2_diffuse.jpg";
    m_brickWall.m_normalTexName = "bricks2_normal.jpg";
    m_brickWall.m_dirty = true;
    m_brickWall.m_reevaluate = true;
}

int Viewer::initWindow()
{
    if(!glfwInit())
    {
        throw std::runtime_error("Failed to initialize GLFW !");
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4); //< MSAA
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
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

    glewExperimental = true;
    if(glewInit() != GLEW_OK)
    {
        throw std::runtime_error("Failed to initialize GLEW !");
        return -1;
    }
    return 0;
}

#define STB_IMAGE_WRITE_IMPLEMENTATION 
#include "stb_image_write.h" 

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
                    Log("start Image sequence.\n");
                }
                else
                {
                    Log("stop Image sequence.\n");
                }
            }
            ImGui::SliderFloat("Clip plane distance", &viewer.m_clip_plane_distance, -0.5f, 0.5f);

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu(ICON_FA_EYE " View"))
        {
            ImGui::Checkbox("ShowUI", &displayOption.showUI);
            ImGui::Checkbox("ShowHelp", &setting.showHelpTip);
            ImGui::Checkbox("Wireframe", &displayOption.wireframe);
            ImGui::Separator();
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu(ICON_FA_WINDOWS " Settings"))
        {
            ImGui::Checkbox("VSync", &viewer.m_enable_vsync);
            ImGui::Separator();
            ImGui::Checkbox("Double side lighting", &viewer.m_double_side_lighting);
            ImGui::Separator();
            ImGui::ColorEdit3("Mesh Color", &viewer.m_mesh_color.x);
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
        ImGui::Text("         'T' : toggle tesselation ");
        ImGui::Text("         ']' : increase tessellate level when enable tesselation ");
        ImGui::Text("         '[' : decrease tessellate level when enable tesselation ");
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