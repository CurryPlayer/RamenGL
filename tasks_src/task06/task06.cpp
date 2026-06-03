#include <glad/glad.h>

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <algorithm>
#include <string>
#include <vector>

#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <physfs.h>

#include "../ramen/ramen.h"
#include "../ramen/rgl_camera.h"
#include "../ramen/rgl_defines.h"
#include "../ramen/rgl_filesystem.h"
#include "../ramen/rgl_image.h"
#include "../ramen/rgl_math.h"
#include "../ramen/rgl_model.h"
#include "../ramen/rgl_shader.h"

std::vector<Vertex> CreateCube(const Vec3f& color);
std::vector<Vertex> CreatePlane(float size, const Vec3f& color);
std::vector<Vertex> CreateNormalLines(const std::vector<Vertex>& vertices, float lineLength = 0.1f, bool drawNormals = true);

std::vector<Vertex> CreateCube(const Vec3f& color) {
    std::vector<Vertex> vertices;
    const Vec3f positions[8] = {
        {-0.5f, -0.5f, -0.5f},
        {0.5f, -0.5f, -0.5f},
        {0.5f, 0.5f, -0.5f},
        {-0.5f, 0.5f, -0.5f},
        {-0.5f, -0.5f, 0.5f},
        {0.5f, -0.5f, 0.5f},
        {0.5f, 0.5f, 0.5f},
        {-0.5f, 0.5f, 0.5f}
    };
    const Vec3f normals[6] = {
        {0, 0, -1}, // front
        {0, 0, 1},  // back
        {-1, 0, 0}, // left
        {1, 0, 0},  // right
        {0, -1, 0}, // bottom
        {0, 1, 0}   // top
    };
    const int indices[36] = {
        // front
        0, 1, 2, 0, 2, 3,
        // back
        5, 4, 7, 5, 7, 6,
        // left
        4, 0, 3, 4, 3, 7,
        // right
        1, 5, 6, 1, 6, 2,
        // bottom
        4, 5, 1, 4, 1, 0,
        // top
        3, 2, 6, 3, 6, 7
    };
    for (int i = 0; i < 36; i++) {
        Vertex v;
        v.position = positions[indices[i]];
        const int face = i / 6;
        v.normal = normals[face];
        v.color = color;
        vertices.push_back(v);
    }
    return vertices;
}

std::vector<Vertex> CreatePlane(const float size, const Vec3f& color) {
    std::vector<Vertex> vertices;
    const float h = size / 2.0f;
    vertices.push_back({{-h, 0.0f,  h}, {0, 1, 0}, color});
    vertices.push_back({{ h, 0.0f,  h}, {0, 1, 0}, color});
    vertices.push_back({{-h, 0.0f, -h}, {0, 1, 0}, color});
    vertices.push_back({{ h, 0.0f,  h}, {0, 1, 0}, color});
    vertices.push_back({{ h, 0.0f, -h}, {0, 1, 0}, color});
    vertices.push_back({{-h, 0.0f, -h}, {0, 1, 0}, color});
    return vertices;
}

std::vector<Vertex> CreateNormalLines(const std::vector<Vertex>& vertices, float lineLength, bool drawNormals) {
    std::vector<Vertex> lines;
    if (!drawNormals) return lines;

    for (const auto& v : vertices) {
        const Vec3f start = v.position;
        const Vec3f end = v.position + v.normal * lineLength;
        lines.push_back({start, {0, 0, 0}, {1, 1, 1}});
        lines.push_back({end, {0, 0, 0}, {1, 1, 1}});
    }
    return lines;
}

int main(int argc, char** argv)
{
    Filesystem* pFS = Filesystem::Init(argc, argv);

    Ramen* pRamen = Ramen::Instance();
    pRamen->Init("Task 06 - Shadow Mapping", 1280, 720);

    /* Load shaders. */
    Shader shader{};
    if ( !shader.Load("shaders/task05.vert", "shaders/task05.frag") )
    {
        fprintf(stderr, "Could not load shader.\n");
    }

    Shader groundShader{};
    if ( !groundShader.Load("shaders/task06_ground.vert", "shaders/task06_ground.frag") )
    {
        fprintf(stderr, "Could not load ground shader.\n");
    }

    Shader envShader{};
    if ( !envShader.Load("shaders/task06_model.vert", "shaders/task06_model.frag") )
    {
        fprintf(stderr, "Could not load environment mapping shader.\n");
    }
    Shader debugShader{};
    if ( !debugShader.Load("shaders/task06_debug.vert", "shaders/task06_debug.frag") )
    {
        fprintf(stderr, "Could not load debug shader.\n");
    }
    Shader shadowShader{};
    if ( !shadowShader.Load("shaders/task06_shadow_map.vert", "shaders/task06_shadow_map.frag") )
    {
        fprintf(stderr, "Could not load shadow shader.\n");
    }

    /* Load images */
    Image groundImage{};
    if (!groundImage.Load("textures/linux-quake-512x512.png")) {
        fprintf(stderr, "Could not load ground texture.\n");
    }
    GLuint groundTextureHandle;
    glCreateTextures(GL_TEXTURE_2D, 1, &groundTextureHandle);
    glTextureStorage2D(groundTextureHandle, 1, GL_RGBA8, groundImage.GetWidth(), groundImage.GetHeight());
    glTextureSubImage2D(groundTextureHandle, 0, 0, 0, groundImage.GetWidth(), groundImage.GetHeight(), GL_RGBA, GL_UNSIGNED_BYTE, groundImage.Data());
    glTextureParameteri(groundTextureHandle, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTextureParameteri(groundTextureHandle, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateTextureMipmap(groundTextureHandle);

    Image posxImage{};
    if (!posxImage.Load("textures/cubemaps/yokohama_park/posx.jpg")) {
        fprintf(stderr, "Could not load posx image.\n");
    }
    Image negxImage{};
    if (!negxImage.Load("textures/cubemaps/yokohama_park/negx.jpg")) {
        fprintf(stderr, "Could not load negx image.\n");
    }
    Image posyImage{};
    if (!posyImage.Load("textures/cubemaps/yokohama_park/posy.jpg")) {
        fprintf(stderr, "Could not load posy image.\n");
    }
    Image negyImage{};
    if (!negyImage.Load("textures/cubemaps/yokohama_park/negy.jpg")) {
        fprintf(stderr, "Could not load negy image.\n");
    }
    Image poszImage{};
    if (!poszImage.Load("textures/cubemaps/yokohama_park/posz.jpg")) {
        fprintf(stderr, "Could not load posz image.\n");
    }
    Image negzImage{};
    if (!negzImage.Load("textures/cubemaps/yokohama_park/negz.jpg")) {
        fprintf(stderr, "Could not load negz image.\n");
    }

    /* List models */
    std::vector<std::string> modelList;
    if (char** files = PHYSFS_enumerateFiles("models")) {
        for (char** i = files; *i != nullptr; i++) {
            if (std::string filename = *i; filename.find(".obj") != std::string::npos) {
                modelList.push_back(filename);
            }
        }
        PHYSFS_freeList(files);
    }
    std::ranges::sort(modelList);
    int selectedModelIndex = 0;
    for (size_t i = 0; i < modelList.size(); ++i) {
        if (modelList[i] == "Skull.obj") {
            selectedModelIndex = static_cast<int>(i);
            break;
        }
    }

    /* Load initial model */
    Model model{};
    if ( !modelList.empty() )
    {
        std::string path = "models/" + modelList[selectedModelIndex];
        if ( !model.Load(path.c_str()) )
        {
            fprintf(stderr, "Could not load model: %s\n", path.c_str());
        }
    }

    // ########################################
    // ### Task 5.1.1 (Create 3D texture) ###
    // ########################################
    /* create the cubemap texture */
    GLuint cubemapHandle;
    glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &cubemapHandle);
    glTextureStorage2D(cubemapHandle, 1, GL_RGBA8, posxImage.GetWidth(), posxImage.GetHeight());
    /* upload the six images to the cubemap */
    glTextureSubImage3D(cubemapHandle, 0, 0, 0, 0, posxImage.GetWidth(), posxImage.GetHeight(), 1, GL_RGBA, GL_UNSIGNED_BYTE, posxImage.Data());
    glTextureSubImage3D(cubemapHandle, 0, 0, 0, 1, negxImage.GetWidth(), negxImage.GetHeight(), 1, GL_RGBA, GL_UNSIGNED_BYTE, negxImage.Data());
    glTextureSubImage3D(cubemapHandle, 0, 0, 0, 2, posyImage.GetWidth(), posyImage.GetHeight(), 1, GL_RGBA, GL_UNSIGNED_BYTE, posyImage.Data());
    glTextureSubImage3D(cubemapHandle, 0, 0, 0, 3, negyImage.GetWidth(), negyImage.GetHeight(), 1, GL_RGBA, GL_UNSIGNED_BYTE, negyImage.Data());
    glTextureSubImage3D(cubemapHandle, 0, 0, 0, 4, poszImage.GetWidth(), poszImage.GetHeight(), 1, GL_RGBA, GL_UNSIGNED_BYTE, poszImage.Data());
    glTextureSubImage3D(cubemapHandle, 0, 0, 0, 5, negzImage.GetWidth(), negzImage.GetHeight(), 1, GL_RGBA, GL_UNSIGNED_BYTE, negzImage.Data());

    /* --- Shadow Mapping Configuration --- */
    constexpr unsigned int SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 4096;
    
    // 1. Create a Framebuffer Object (FBO)
    // The FBO is a container for textures that we can render into instead of the screen.
    GLuint shadowMapFBO;
    glCreateFramebuffers(1, &shadowMapFBO);

    // 2. Create the Depth Texture (The Shadow Map)
    // This texture will store the distance from the light source to the nearest geometry.
    GLuint shadowMapTexture;
    glCreateTextures(GL_TEXTURE_2D, 1, &shadowMapTexture);
    // Use GL_DEPTH_COMPONENT24 for high precision depth storage.
    glTextureStorage2D(shadowMapTexture, 1, GL_DEPTH_COMPONENT24, SHADOW_WIDTH, SHADOW_HEIGHT);

    // Set texture parameters:
    // NEAREST filtering is usually preferred for raw depth values to avoid interpolation artifacts during the depth test.
    glTextureParameteri(shadowMapTexture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(shadowMapTexture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // CLAMP_TO_BORDER ensures that areas outside the light's frustum are treated as "not in shadow" (by setting a high depth value).
    glTextureParameteri(shadowMapTexture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTextureParameteri(shadowMapTexture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTextureParameterfv(shadowMapTexture, GL_TEXTURE_BORDER_COLOR, borderColor);

    // 3. Attach the texture to the FBO
    // We only need the depth attachment because shadow mapping doesn't require color information.
    glNamedFramebufferTexture(shadowMapFBO, GL_DEPTH_ATTACHMENT, shadowMapTexture, 0);
    
    // Tell OpenGL that we don't want to draw any color data to this FBO.
    glNamedFramebufferDrawBuffer(shadowMapFBO, GL_NONE);
    glNamedFramebufferReadBuffer(shadowMapFBO, GL_NONE);

    // Check if the framebuffer is complete and ready for use.
    if (glCheckNamedFramebufferStatus(shadowMapFBO, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        fprintf(stderr, "Error: Shadow Map Framebuffer is not complete!\n");
    }

    /* Light Source State */
    Vec3f lightPos = { 2.0f, 4.0f, 1.0f };
    bool useShadows = true;
    bool showGroundTexture = false;

    /* Create camera */
    Camera camera(Vec3f{ 0.0f, 2.0f, 5.0f });
    camera.Pitch(-20.0f);

    /* Model mat*/
    Mat4f modelMat = Mat4f::Identity();

    /* show normal vectors */
    bool showNormals = false;
    bool showTexture = true;
    bool fpsCamera = false;

    /* Helper function to create VBO and VAO from vertices */
    auto CreateGeometryBuffers = [](const std::vector<Vertex>& vertices) {
        GLuint VBO;
        glCreateBuffers(1, &VBO);
        glNamedBufferData(VBO, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

        /* VAO. */
        GLuint VAO;
        glCreateVertexArrays(1, &VAO);
        glVertexArrayVertexBuffer(VAO, 0, VBO, 0, sizeof(Vertex));
        /* Position */
        glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
        glEnableVertexArrayAttrib(VAO, 0);
        glVertexArrayAttribBinding(VAO, 0, 0);
        /* Normal */
        glVertexArrayAttribFormat(VAO, 1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float));
        glEnableVertexArrayAttrib(VAO, 1);
        glVertexArrayAttribBinding(VAO, 1, 0);
        /* Color */
        glVertexArrayAttribFormat(VAO, 2, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float));
        glEnableVertexArrayAttrib(VAO, 2);
        glVertexArrayAttribBinding(VAO, 2, 0);

        return std::make_pair(VAO, VBO);
    };

    /* Create geometries */
    const std::vector<Vertex> cubeVertices = CreateCube({1.0f, 0.0f, 0.0f}); // Red
    auto [VAO_Cube, VBO_Cube] = CreateGeometryBuffers(cubeVertices);
    
    // Create a visual marker for the light source (Yellow Cube)
    const std::vector<Vertex> lightMarkerVertices = CreateCube({1.0f, 1.0f, 0.0f}); // Yellow
    auto [VAO_LightMarker, VBO_LightMarker] = CreateGeometryBuffers(lightMarkerVertices);

    const std::vector<Vertex> planeVertices = CreatePlane(5.0f, {0.5f, 0.5f, 0.5f}); // gray
    auto [VAO_Plane, VBO_Plane] = CreateGeometryBuffers(planeVertices);


    auto [VAO_Model, VBO_Model] = CreateGeometryBuffers(model.GetVertices());

    /* Create normal lines */
    const std::vector<Vertex> cubeNormals = CreateNormalLines(cubeVertices);
    auto [VAO_CubeNormals, VBO_CubeNormals] = CreateGeometryBuffers(cubeNormals);
    const std::vector<Vertex> planeNormals = CreateNormalLines(planeVertices);
    auto [VAO_PlaneNormals, VBO_PlaneNormals] = CreateGeometryBuffers(planeNormals);
    std::vector<Vertex> modelNormals = CreateNormalLines(model.GetVertices());
    auto [VAO_ModelNormals, VBO_ModelNormals] = CreateGeometryBuffers(modelNormals);

    /* Some global GL states */
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    // glDisable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    /* Main loop */
    bool isRunning = true;
    SDL_GL_SetSwapInterval(1); /* 1 = VSync enabled; 0 = VSync disabled */
    Uint64 ticksPerSecond = SDL_GetPerformanceFrequency();
    Uint64 startCounter   = SDL_GetPerformanceCounter();
    Uint64 endCounter     = SDL_GetPerformanceCounter();
    while ( isRunning )
    {
        double ticksPerFrame = (double)endCounter - (double)startCounter;
        double msPerFrame    = (ticksPerFrame / (double)ticksPerSecond) * 1000.0;
        startCounter         = SDL_GetPerformanceCounter();

        SDL_Event e;
        while ( SDL_PollEvent(&e) )
        {
            ImGui_ImplSDL3_ProcessEvent(&e);
            pRamen->ProcessInputEvent(e);

            if ( e.type == SDL_EVENT_QUIT )
            {
                isRunning = false;
            }

            if ( e.type == SDL_EVENT_KEY_DOWN )
            {
                switch ( e.key.key )
                {
                case SDLK_ESCAPE:
                {
                    isRunning = false;
                }
                break;
                    /* active / deactivate normals with 'N' */
                case SDLK_N:
                    {
                        showNormals = !showNormals;
                    }
                break;
            /* ADDITIONAL: Camera movement */
            /* Camera movement (operate on active camera) */
            case SDLK_W: { camera.Translate(camera.GetForward() * 0.05f); }
                break;
            case SDLK_S: { camera.Translate(-camera.GetForward() * 0.05f); }
                break;
            case SDLK_A: { camera.Translate(-camera.GetRight() * 0.05f); }
                break;
            case SDLK_D: { camera.Translate(camera.GetRight() * 0.05f); }
                break;
            case SDLK_Q: { camera.Translate(camera.GetUp() * 0.05f); }
                break;
            case SDLK_E: { camera.Translate(-camera.GetUp() * 0.05f); }
                break;

            case SDLK_UP: { camera.Pitch(1.0f); }
                break;
            case SDLK_DOWN: { camera.Pitch(-1.0f); }
                break;
            case SDLK_LEFT: { camera.Yaw(1.0f); }
                break;
            case SDLK_RIGHT: { camera.Yaw(-1.0f); }
                break;
            case SDLK_PAGEUP: { camera.Roll(1.0f); }
                break;
            case SDLK_PAGEDOWN: { camera.Roll(-1.0f); }
                break;
            case SDLK_C:
                {
                    showTexture = !showTexture;
                }
                break;
                case SDLK_F:
                    {
                        fpsCamera = !fpsCamera;
                    }
                    break;
                default:
                {
                }
                }
            }
        }

        /* Query new frame dimensions */
        int windowWidth, windowHeight;
        SDL_GetWindowSize(pRamen->GetWindow(), &windowWidth, &windowHeight);

        /* Adjust viewport and perspective projection accordingly. */
        glViewport(0, 0, windowWidth, windowHeight);

        /* View mat (depending on correct camera) */
        Mat4f viewMat = LookAt(
            camera.GetPosition(), camera.GetPosition() + camera.GetForward(), camera.GetUp());

        /* Projection mat */
        float aspect  = (float)windowWidth / (float)windowHeight;
        Mat4f projMat = PerspectiveProjection(TO_RAD(60.0f), aspect, 0.01f, 500.0f);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Shadow Mapping Settings");
        ImGui::Text("Controls: WASD move, Arrow keys rotate, F toggle FPS-mode");
        ImGui::Separator();
        ImGui::Checkbox("Use Shadows", &useShadows);
        ImGui::Checkbox("Show Ground Texture", &showGroundTexture);
        ImGui::DragFloat3("Light Position", lightPos.Data(), 0.1f);
        ImGui::Separator();
        ImGui::Checkbox("FPS Camera (centered cubemap)", &fpsCamera);
        ImGui::Text("Current camera: %s", fpsCamera ? "FPS" : "Free");

        ImGui::Separator();
        ImGui::Text("Model Selection");
        if (!modelList.empty()) {
            if (ImGui::BeginCombo("Model", modelList[selectedModelIndex].c_str())) {
                for (int i = 0; i < static_cast<int>(modelList.size()); i++) {
                    const bool is_selected = (selectedModelIndex == i);
                    if (ImGui::Selectable(modelList[i].c_str(), is_selected)) {
                        selectedModelIndex = i;
                        std::string path = "models/" + modelList[selectedModelIndex];
                        if (model.Load(path.c_str())) {
                            glDeleteBuffers(1, &VBO_Model);
                            glDeleteVertexArrays(1, &VAO_Model);
                            auto [newVAO, newVBO] = CreateGeometryBuffers(model.GetVertices());
                            VAO_Model = newVAO;
                            VBO_Model = newVBO;

                            glDeleteBuffers(1, &VBO_ModelNormals);
                            glDeleteVertexArrays(1, &VAO_ModelNormals);
                            modelNormals = CreateNormalLines(model.GetVertices());
                            auto [newNormVAO, newNormVBO] = CreateGeometryBuffers(modelNormals);
                            VAO_ModelNormals = newNormVAO;
                            VBO_ModelNormals = newNormVBO;
                        }
                    }
                    if (is_selected) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
        } else {
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "No models found in models/");
        }

        ImGui::End();

        /* ImGUI Rendering */
        ImGui::Render();

        /* --- Shadow Mapping: Pass 1 (Render to Texture) --- */
        // In the first pass, we render the scene from the light's perspective
        // to populate the depth texture (the shadow map).

        // 1. Calculate Light Matrices
        // The light needs a view matrix (LookAt) and a projection matrix (usually Orthographic for directional lights)
        Mat4f lightView = LookAt(lightPos, Vec3f{ 0.0f, 0.0f, 0.0f }, RAMEN_WORLD_UP);
        // Orthographic projection is used for directional lights (like the sun)
        // Adjust the frustum size (-10 to 10) to cover your entire scene.
        // For a spotlight, you would use PerspectiveProjection instead.
        float orthoSize = 10.0f;
        Mat4f lightProj = OrthographicProjection(-orthoSize, orthoSize, -orthoSize, orthoSize, 0.1f, 20.0f);
        
        // Let's assume the light space matrix is what we need in the shader.
        Mat4f lightSpaceMatrix = lightProj * lightView;

        // 2. Setup Rendering State for Shadow Pass
        shadowShader.Use();
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT); // Clear ONLY the depth buffer
        
        // We use a bit of "peter panning" prevention here by culling front faces
        // instead of back faces during the shadow pass.
        glCullFace(GL_FRONT);

        // 3. Render Scene into Shadow Map
        glUniformMatrix4fv(0, 1, GL_FALSE, lightSpaceMatrix.Data());

        // Render Plane
        glUniformMatrix4fv(1, 1, GL_FALSE, modelMat.Data());
        glBindVertexArray(VAO_Plane);
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)planeVertices.size());

        // Render Model
        // We translate the skull up by 0.5 units so it sits ON the plane instead of being clipped.
        Mat4f scullModelMat = modelMat * Translate(Vec3f{0.0f, 0.15f, 0.0f}) * Scale(Vec3f{ 0.15f, 0.15f, 0.15f });
        glUniformMatrix4fv(1, 1, GL_FALSE, scullModelMat.Data());
        glBindVertexArray(VAO_Model);
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)model.NumVertices());

        // 4. Reset Rendering State
        glCullFace(GL_BACK); // Restore normal culling
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // Back to default framebuffer
        glViewport(0, 0, windowWidth, windowHeight); // Restore screen viewport

        /* Rendering */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);

        shader.Use();

        glUniform1i(99, showTexture ? 1 : 0);

        Mat4f modelMatSky = Mat4f::Identity();

        /* ##############################
         * TASK 5.3 Choose correct camera
         * ##############################
         */
        // Choose view matrix based on camera mode
        // FPS Mode: Use normal viewMat (with translation) - skybox moves with camera
        // Free Mode: Use rotation-only viewMatSky - skybox stays in place
        Mat4f viewMatSkybox;
        if (fpsCamera)
        {
            // FPS Camera: skybox moves with camera
            viewMatSkybox = viewMat;
        }
        else
        {
            // Free Camera: skybox stays in place
            viewMatSkybox = LookAt(
                Vec3f{0.0f, 0.0f, 0.0f},    // eye at origin
                camera.GetForward(),         // look direction
                camera.GetUp()               // up vector
            );
        }

        // Draw skybox
        glDepthMask(GL_FALSE);
        glBindTextureUnit(0, cubemapHandle);
        glBindVertexArray(VAO_Cube);
        glUniformMatrix4fv(0, 1, GL_FALSE, modelMatSky.Data());
        glUniformMatrix4fv(1, 1, GL_FALSE, viewMatSkybox.Data());
        glUniformMatrix4fv(2, 1, GL_FALSE, projMat.Data());
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)cubeVertices.size());
        // Re-enable depth
        glDepthMask(GL_TRUE);

        if (showNormals)
        {
            debugShader.Use();
            glBindVertexArray(VAO_CubeNormals);
            glUniformMatrix4fv(0, 1, GL_FALSE, modelMatSky.Data());
            glUniformMatrix4fv(1, 1, GL_FALSE, viewMatSkybox.Data());
            glUniformMatrix4fv(2, 1, GL_FALSE, projMat.Data());
            glDrawArrays(GL_LINES, 0, (GLsizei)cubeNormals.size());
        }

        groundShader.Use();
        glBindVertexArray(VAO_Plane);
        // Pass standard transformation matrices
        glUniformMatrix4fv(0, 1, GL_FALSE, modelMat.Data());
        glUniformMatrix4fv(1, 1, GL_FALSE, viewMat.Data());
        glUniformMatrix4fv(2, 1, GL_FALSE, projMat.Data());
        
        // Pass shadow mapping specific uniforms
        // We use the same LightSpaceMatrix calculated in Pass 1
        glUniformMatrix4fv(10, 1, GL_FALSE, lightSpaceMatrix.Data());
        glUniform3fv(3, 1, lightPos.Data());
        glUniform3fv(4, 1, camera.GetPosition().Data());
        
        // Pass ground texture toggle, shadow toggle and bind textures
        glUniform1i(99, showGroundTexture ? 1 : 0);
        glUniform1i(100, useShadows ? 1 : 0);
        
        // Bind the shadow map texture to binding unit 1
        glBindTextureUnit(1, shadowMapTexture);
        // Bind the ground texture to binding unit 2
        glBindTextureUnit(2, groundTextureHandle);
        
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)planeVertices.size());

        if (showNormals)
        {
            debugShader.Use();
            glBindVertexArray(VAO_PlaneNormals);
            glUniformMatrix4fv(0, 1, GL_FALSE, modelMat.Data());
            glUniformMatrix4fv(1, 1, GL_FALSE, viewMat.Data());
            glUniformMatrix4fv(2, 1, GL_FALSE, projMat.Data());
            glDrawArrays(GL_LINES, 0, (GLsizei)planeNormals.size());
        }

        /* Render reflecting model */
        envShader.Use();
        glBindVertexArray(VAO_Model);
        // Same transformation as in Pass 1
        scullModelMat = modelMat * Translate(Vec3f{0.0f, 0.15f, 0.0f}) * Scale(Vec3f{0.15f, 0.15f, 0.15f});
        glUniformMatrix4fv(0, 1, GL_FALSE, scullModelMat.Data());
        glUniformMatrix4fv(1, 1, GL_FALSE, viewMat.Data());
        glUniformMatrix4fv(2, 1, GL_FALSE, projMat.Data());
        glUniform3fv(3, 1, camera.GetPosition().Data());
        
        // Pass shadow mapping specific uniforms to the model shader
        glUniformMatrix4fv(10, 1, GL_FALSE, lightSpaceMatrix.Data());
        glUniform3fv(11, 1, lightPos.Data());
        glUniform1i(100, useShadows ? 1 : 0);
        
        // Ensure both textures (Cubemap for reflection and ShadowMap) are bound
        glBindTextureUnit(0, cubemapHandle);
        glBindTextureUnit(1, shadowMapTexture);
        
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)model.NumVertices());

        if (showNormals)
        {
            debugShader.Use();
            glBindVertexArray(VAO_ModelNormals);
            glUniformMatrix4fv(0, 1, GL_FALSE, scullModelMat.Data());
            glUniformMatrix4fv(1, 1, GL_FALSE, viewMat.Data());
            glUniformMatrix4fv(2, 1, GL_FALSE, projMat.Data());
            glDrawArrays(GL_LINES, 0, (GLsizei)modelNormals.size());
        }

        /* Render Light Marker (Yellow Cube) */
        debugShader.Use();
        Mat4f lightModelMat = Translate(lightPos) * Scale(Vec3f{0.2f, 0.2f, 0.2f});
        glUniformMatrix4fv(0, 1, GL_FALSE, lightModelMat.Data());
        glUniformMatrix4fv(1, 1, GL_FALSE, viewMat.Data());
        glUniformMatrix4fv(2, 1, GL_FALSE, projMat.Data());
        glBindVertexArray(VAO_LightMarker);
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)lightMarkerVertices.size());

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(pRamen->GetWindow());

        endCounter = SDL_GetPerformanceCounter();
    }

    /* GL Resources shutdown. */
    shader.Delete();
    groundShader.Delete();
    envShader.Delete();
    debugShader.Delete();
    glDeleteTextures(1, &cubemapHandle);
    glDeleteTextures(1, &groundTextureHandle);
    glDeleteBuffers(1, &VBO_Cube);
    glDeleteVertexArrays(1, &VAO_Cube);
    glDeleteBuffers(1, &VBO_Plane);
    glDeleteVertexArrays(1, &VAO_Plane);
    glDeleteBuffers(1, &VBO_Model);
    glDeleteVertexArrays(1, &VAO_Model);
    glDeleteBuffers(1, &VBO_CubeNormals);
    glDeleteVertexArrays(1, &VAO_CubeNormals);
    glDeleteBuffers(1, &VBO_PlaneNormals);
    glDeleteVertexArrays(1, &VAO_PlaneNormals);
    glDeleteBuffers(1, &VBO_ModelNormals);
    glDeleteVertexArrays(1, &VAO_ModelNormals);
    glDeleteBuffers(1, &VBO_LightMarker);
    glDeleteVertexArrays(1, &VAO_LightMarker);


    shadowShader.Delete();
    glDeleteFramebuffers(1, &shadowMapFBO);
    glDeleteTextures(1, &shadowMapTexture);

    /* Ramen Shutdown */
    pRamen->Shutdown();

    /* Filesystem deinit */
    PHYSFS_deinit();

    return 0;
}
