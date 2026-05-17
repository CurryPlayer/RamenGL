#include <glad/glad.h>

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <vector>
#include <iostream>

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
    vertices.push_back({{-h, 0.0f, -h}, {0, 1, 0}, color});
    vertices.push_back({{ h, 0.0f, -h}, {0, 1, 0}, color});
    vertices.push_back({{ h, 0.0f,  h}, {0, 1, 0}, color});
    vertices.push_back({{-h, 0.0f, -h}, {0, 1, 0}, color});
    vertices.push_back({{ h, 0.0f,  h}, {0, 1, 0}, color});
    vertices.push_back({{-h, 0.0f,  h}, {0, 1, 0}, color});
    return vertices;
}

int main(int argc, char** argv)
{
    Filesystem* pFS = Filesystem::Init(argc, argv);
    Ramen* pRamen = Ramen::Instance();
    pRamen->Init("Task 06 - Shadowmapping", 1280, 720);

    /* Load shaders. */
    Shader shadowMapShader{};
    if ( !shadowMapShader.Load("shaders/task06_shadowmap.vert", "shaders/task06_shadowmap.frag") )
    {
        fprintf(stderr, "Could not load shader.\n");
    }
    Shader groundShader{};
    if ( !groundShader.Load("shaders/task06_ground.vert", "shaders/task06_ground.frag") )
    {
        fprintf(stderr, "Could not load shader.\n");
    }
    Shader lightShader{};
    if ( !lightShader.Load("shaders/task06_light.vert", "shaders/task06_light.frag") )
    {
        fprintf(stderr, "Could not load shader.\n");
    }
    Shader skyboxShader{};
    if ( !skyboxShader.Load("shaders/task05.vert", "shaders/task05.frag") )
    {
        fprintf(stderr, "Could not load shader.\n");
    }
    Shader envShadowShader{};
    if ( !envShadowShader.Load("shaders/task06_env_shadow.vert", "shaders/task06_env_shadow.frag") )
    {
        fprintf(stderr, "Could not load shader.\n");
    }

    /* Load images */
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

    /* Load ground texture */
    Image groundImage{};
    if ( !groundImage.Load("textures/linux-quake-512x512.png") )
    {
        fprintf(stderr, "Could not load texture.\n");
    }

    GLuint groundTextureHandle;
    glCreateTextures(GL_TEXTURE_2D, 1, &groundTextureHandle);
    glTextureStorage2D(groundTextureHandle, 1, GL_RGBA8, groundImage.GetWidth(), groundImage.GetHeight());
    glTextureSubImage2D(groundTextureHandle, 0, 0, 0, groundImage.GetWidth(), groundImage.GetHeight(), GL_RGBA, GL_UNSIGNED_BYTE, groundImage.Data());
    glTextureParameteri(groundTextureHandle, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(groundTextureHandle, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    /* Load model */
    Model skullModel{};
    if ( !skullModel.Load("models/Skull.obj") )
    {
        fprintf(stderr, "Could not load model.\n");
    }
    Model sphereModel{};
    if ( !sphereModel.Load("models/sphere.obj") )
    {
        fprintf(stderr, "Could not load model.\n");
    }

    /* Camera and Light */
    Camera camera(Vec3f{ 0.0f, 2.0f, 5.0f });
    camera.Pitch(-15.0f);
    Vec3f lightPos = { 2.0f, 4.0f, 1.0f };
    bool useShadows = true, useTexture = true, fpsCamera = false;

    /* Shadow map setup */
    const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
    GLuint depthMapFBO, depthMap;
    glCreateFramebuffers(1, &depthMapFBO);
    glCreateTextures(GL_TEXTURE_2D, 1, &depthMap);
    glTextureStorage2D(depthMap, 1, GL_DEPTH_COMPONENT32F, SHADOW_WIDTH, SHADOW_HEIGHT);
    glTextureParameteri(depthMap, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(depthMap, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(depthMap, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTextureParameteri(depthMap, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTextureParameterfv(depthMap, GL_TEXTURE_BORDER_COLOR, borderColor);
    glNamedFramebufferTexture(depthMapFBO, GL_DEPTH_ATTACHMENT, depthMap, 0);
    glNamedFramebufferDrawBuffer(depthMapFBO, GL_NONE);
    glNamedFramebufferReadBuffer(depthMapFBO, GL_NONE);

    /* Geometries */
    auto CreateBuffers = [](const std::vector<Vertex>& v) {
        GLuint VBO, VAO;
        glCreateBuffers(1, &VBO);
        glNamedBufferData(VBO, v.size() * sizeof(Vertex), v.data(), GL_STATIC_DRAW);
        glCreateVertexArrays(1, &VAO);
        glVertexArrayVertexBuffer(VAO, 0, VBO, 0, sizeof(Vertex));
        glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
        glEnableVertexArrayAttrib(VAO, 0);
        glVertexArrayAttribBinding(VAO, 0, 0);
        glVertexArrayAttribFormat(VAO, 1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float));
        glEnableVertexArrayAttrib(VAO, 1);
        glVertexArrayAttribBinding(VAO, 1, 0);
        glVertexArrayAttribFormat(VAO, 2, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float));
        glEnableVertexArrayAttrib(VAO, 2);
        glVertexArrayAttribBinding(VAO, 2, 0);
        return std::make_pair(VAO, VBO);
    };

    const std::vector<Vertex> planeVertices = CreatePlane(20.0f, {1.0f, 1.0f, 1.0f});
    auto [VAO_Plane, VBO_Plane] = CreateBuffers(planeVertices);
    auto [VAO_Model, VBO_Model] = CreateBuffers(skullModel.GetVertices());
    auto [VAO_Sphere, VBO_Sphere] = CreateBuffers(sphereModel.GetVertices());
    const std::vector<Vertex> cubeVertices = CreateCube({1,1,1});
    auto [VAO_Cube, VBO_Cube] = CreateBuffers(cubeVertices);

    glEnable(GL_DEPTH_TEST);

    bool isRunning = true;
    while ( isRunning )
    {
        SDL_Event e;
        while ( SDL_PollEvent(&e) ) {
            ImGui_ImplSDL3_ProcessEvent(&e);
            pRamen->ProcessInputEvent(e);
            if ( e.type == SDL_EVENT_QUIT ) isRunning = false;
            if ( e.type == SDL_EVENT_KEY_DOWN ) {
                switch ( e.key.key )
                {
                case SDLK_ESCAPE: isRunning = false; break;
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
                case SDLK_F: { fpsCamera = !fpsCamera; }
                    break;
                default:{}
                }
            }
        }

        ImGui_ImplOpenGL3_NewFrame(); ImGui_ImplSDL3_NewFrame(); ImGui::NewFrame();
        ImGui::Begin("Settings");
        ImGui::Checkbox("Enable Shadows", &useShadows);
        ImGui::Checkbox("Use Ground Texture", &useTexture);
        ImGui::Checkbox("FPS Camera", &fpsCamera);
        ImGui::DragFloat3("Light Position", reinterpret_cast<float*>(&lightPos), 0.1f);
        ImGui::End();

        // Shadow Pass
        Mat4f lightProj = OrthographicProjection(-15.0f, 15.0f, -15.0f, 15.0f, 1.0f, 50.0f);
        Mat4f lightView = LookAt(lightPos, {0,0,0}, {0,1,0});
        Mat4f lightSpaceMatrix = lightProj * lightView;

        shadowMapShader.Use();
        glUniformMatrix4fv(glGetUniformLocation(shadowMapShader.GetID(), "view"), 1, GL_FALSE, lightView.Data());
        glUniformMatrix4fv(glGetUniformLocation(shadowMapShader.GetID(), "proj"), 1, GL_FALSE, lightProj.Data());
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        
        Mat4f modelMat = Scale({0.2f, 0.2f, 0.2f});
        glUniformMatrix4fv(glGetUniformLocation(shadowMapShader.GetID(), "model"), 1, GL_FALSE, modelMat.Data());
        glBindVertexArray(VAO_Model);
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)skullModel.NumVertices());
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Normal Pass
        int w, h; SDL_GetWindowSize(pRamen->GetWindow(), &w, &h);
        glViewport(0, 0, w, h);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Mat4f viewMat = LookAt(camera.GetPosition(), camera.GetPosition() + camera.GetForward(), camera.GetUp());
        Mat4f projMat = PerspectiveProjection(TO_RAD(60.0f), (float)w/h, 0.1f, 100.0f);

        // Skybox
        skyboxShader.Use();
        Mat4f skyView = LookAt({0,0,0}, camera.GetForward(), camera.GetUp());
        glUniformMatrix4fv(0, 1, GL_FALSE, Mat4f::Identity().Data());
        glUniformMatrix4fv(1, 1, GL_FALSE, skyView.Data());
        glUniformMatrix4fv(2, 1, GL_FALSE, projMat.Data());
        glUniform1i(99, 1);
        glBindTextureUnit(0, cubemapHandle);
        glDepthMask(GL_FALSE);
        glBindVertexArray(VAO_Cube);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthMask(GL_TRUE);

        // Ground
        groundShader.Use();
        glUniformMatrix4fv(glGetUniformLocation(groundShader.GetID(), "view"), 1, GL_FALSE, viewMat.Data());
        glUniformMatrix4fv(glGetUniformLocation(groundShader.GetID(), "proj"), 1, GL_FALSE, projMat.Data());
        glUniformMatrix4fv(glGetUniformLocation(groundShader.GetID(), "lightSpaceMatrix"), 1, GL_FALSE, lightSpaceMatrix.Data());
        glUniform3fv(glGetUniformLocation(groundShader.GetID(), "lightPos"), 1, lightPos.Data());
        glUniform3fv(glGetUniformLocation(groundShader.GetID(), "viewPos"), 1, camera.GetPosition().Data());
        glUniform1i(glGetUniformLocation(groundShader.GetID(), "useShadows"), useShadows);
        glUniform1i(glGetUniformLocation(groundShader.GetID(), "useTexture"), useTexture);
        glUniformMatrix4fv(glGetUniformLocation(groundShader.GetID(), "model"), 1, GL_FALSE, Mat4f::Identity().Data());
        glBindTextureUnit(0, depthMap);
        glBindTextureUnit(1, groundTextureHandle);
        glBindVertexArray(VAO_Plane);
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)planeVertices.size());

        // Env Shadow Model
        envShadowShader.Use();
        glUniformMatrix4fv(glGetUniformLocation(envShadowShader.GetID(), "view"), 1, GL_FALSE, viewMat.Data());
        glUniformMatrix4fv(glGetUniformLocation(envShadowShader.GetID(), "proj"), 1, GL_FALSE, projMat.Data());
        glUniformMatrix4fv(glGetUniformLocation(envShadowShader.GetID(), "model"), 1, GL_FALSE, modelMat.Data());
        glUniformMatrix4fv(glGetUniformLocation(envShadowShader.GetID(), "lightSpaceMatrix"), 1, GL_FALSE, lightSpaceMatrix.Data());
        glUniform3fv(glGetUniformLocation(envShadowShader.GetID(), "lightPos"), 1, lightPos.Data());
        glUniform3fv(glGetUniformLocation(envShadowShader.GetID(), "viewPos"), 1, camera.GetPosition().Data());
        glUniform1i(glGetUniformLocation(envShadowShader.GetID(), "useShadows"), useShadows);
        glBindTextureUnit(0, depthMap);
        glUniform1i(glGetUniformLocation(envShadowShader.GetID(), "shadowMap"), 0);
        glBindTextureUnit(1, cubemapHandle);
        glUniform1i(glGetUniformLocation(envShadowShader.GetID(), "skybox"), 1);
        glBindVertexArray(VAO_Model);
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)skullModel.NumVertices());

        // Light sphere
        lightShader.Use();
        Mat4f lModel = Translate(lightPos) * Scale({0.1f, 0.1f, 0.1f});
        glUniformMatrix4fv(glGetUniformLocation(lightShader.GetID(), "model"), 1, GL_FALSE, lModel.Data());
        glUniformMatrix4fv(glGetUniformLocation(lightShader.GetID(), "view"), 1, GL_FALSE, viewMat.Data());
        glUniformMatrix4fv(glGetUniformLocation(lightShader.GetID(), "proj"), 1, GL_FALSE, projMat.Data());
        Vec3f yel = {1,1,0}; glUniform3fv(glGetUniformLocation(lightShader.GetID(), "lightColor"), 1, yel.Data());
        glBindVertexArray(VAO_Sphere);
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)sphereModel.NumVertices());

        ImGui::Render(); ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(pRamen->GetWindow());
    }

    pRamen->Shutdown(); PHYSFS_deinit();
    return 0;
}
