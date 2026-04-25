#include <glad/glad.h>

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

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

#define NUM_QUAD_VERTICES 4
#define NUM_QUAD_INDICES 6

// #####################################
// ### Task 4.1 (Add UV coordinates) ###
// #####################################
/* add .uv behind every vector */
static Vertex   quadVertices[ NUM_QUAD_VERTICES ] = { { .position = Vec3f{ -1.0f, -1.0f, 0.0f }, .uv = Vec3f{1.0f, 1.0f, 0.0f} },
                                                      { .position = Vec3f{ 1.0f, -1.0f, 0.0f }, .uv = Vec3f{0.0f, 1.0f, 0.0f} },
                                                      { .position = Vec3f{ 1.0f, 1.0f, 0.0f }, .uv = Vec3f{0.0f, 0.0f, 0.0f} },
                                                      { .position = Vec3f{ -1.0f, 1.0f, 0.0f }, .uv = Vec3f{1.0f, 0.0f, 0.0f} } };
static uint16_t quadIndices[ NUM_QUAD_INDICES ]   = { 0, 1, 2, 2, 3, 0 };


// #################
// ### Task 4.5  ###
// #################
/* copy sphere from task 3 and add UV coordinates*/
std::vector<Vertex> CreateSphere(const Vec3f& color, int stacks = 128, int slices = 128);

std::vector<Vertex> CreateSphere(const Vec3f& color, int stacks, int slices) {
    std::vector<Vertex> vertices;
    for (int i = 0; i < stacks; i++) {
        const float phi1 = RAMEN_PI * i / stacks;
        const float phi2 = RAMEN_PI * (i + 1) / stacks;
        for (int j = 0; j < slices; j++) {
            constexpr float radius = 0.5f;
            const float theta1 = 2 * RAMEN_PI * j / slices;
            const float theta2 = 2 * RAMEN_PI * (j + 1) / slices;
            Vec3f p1 = {radius * sinf(phi1) * cosf(theta1), radius * cosf(phi1), radius * sinf(phi1) * sinf(theta1)};
            Vec3f p2 = {radius * sinf(phi1) * cosf(theta2), radius * cosf(phi1), radius * sinf(phi1) * sinf(theta2)};
            Vec3f p3 = {radius * sinf(phi2) * cosf(theta1), radius * cosf(phi2), radius * sinf(phi2) * sinf(theta1)};
            Vec3f p4 = {radius * sinf(phi2) * cosf(theta2), radius * cosf(phi2), radius * sinf(phi2) * sinf(theta2)};
            const Vec3f n1 = Normalize(p1);
            const Vec3f n2 = Normalize(p2);
            const Vec3f n3 = Normalize(p3);
            const Vec3f n4 = Normalize(p4);

            // Calculate UV coordinates for world map mapping
            const Vec3f uv1 = {static_cast<float>(theta1 / (2 * RAMEN_PI)), static_cast<float>(phi1 / RAMEN_PI), 0.0f};
            const Vec3f uv2 = {static_cast<float>(theta2 / (2 * RAMEN_PI)), static_cast<float>(phi1 / RAMEN_PI), 0.0f};
            const Vec3f uv3 = {static_cast<float>(theta1 / (2 * RAMEN_PI)), static_cast<float>(phi2 / RAMEN_PI), 0.0f};
            const Vec3f uv4 = {static_cast<float>(theta2 / (2 * RAMEN_PI)), static_cast<float>(phi2 / RAMEN_PI), 0.0f};

            vertices.push_back({p1, n1, color, uv1});
            vertices.push_back({p2, n2, color, uv2});
            vertices.push_back({p3, n3, color, uv3});
            vertices.push_back({p2, n2, color, uv2});
            vertices.push_back({p4, n4, color, uv4});
            vertices.push_back({p3, n3, color, uv3});
        }
    }
    return vertices;
}


int main(int argc, char** argv)
{
    Filesystem* pFS = Filesystem::Init(argc, argv);

    Ramen* pRamen = Ramen::Instance();
    pRamen->Init("Model loading", 800, 600);

    /* Load shaders. */
    Shader shader{};
    if ( !shader.Load("shaders/textures.vert", "shaders/textures.frag") )
    {
        fprintf(stderr, "Could not load shader.\n");
    }

    Image image{};
    if ( !image.Load("textures/linux-quake-512x512.png") )
    {
        fprintf(stderr, "Could not load texture file.\n");
    }

    Image imageWorldMap{};
    if ( !imageWorldMap.Load("textures/bluemarble-1024x512.png") )
    {
        fprintf(stderr, "Could not load world map texture file.\n");
    }

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
        /* UV */
        glVertexArrayAttribFormat(VAO, 3, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float));
        glEnableVertexArrayAttrib(VAO, 3);
        glVertexArrayAttribBinding(VAO, 3, 0);

        return std::make_pair(VAO, VBO);
    };

    const std::vector<Vertex> sphereVertices = CreateSphere({0.0f, 0.0f, 1.0f}); // Blue
    auto [VAO_Sphere, VBO_Sphere] = CreateGeometryBuffers(sphereVertices);

    /* Create GPU buffer and upload model's vertices. */
    GLuint VBO;
    glCreateBuffers(1, &VBO);
    glNamedBufferData(VBO, NUM_QUAD_VERTICES * sizeof(Vertex), quadVertices, GL_STATIC_DRAW);
    GLuint EBO;
    glCreateBuffers(1, &EBO);
    glNamedBufferData(EBO, NUM_QUAD_INDICES * sizeof(uint16_t), quadIndices, GL_STATIC_DRAW);

    /* Create camera */
    Camera camera(Vec3f{ 0.0f, 0.0f, 5.0f });
    camera.RotateAroundSide(0.0f);

    /* Model mat*/
    Mat4f modelMat = Mat4f::Identity();

    /* VAO. */
    GLuint VAO;
    glCreateVertexArrays(1, &VAO);
    glVertexArrayVertexBuffer(VAO, 0, VBO, 0, sizeof(Vertex));
    glVertexArrayElementBuffer(VAO, EBO);
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
    /* UV */
    glVertexArrayAttribFormat(VAO, 3, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float));
    glEnableVertexArrayAttrib(VAO, 3);
    glVertexArrayAttribBinding(VAO, 3, 0);


    // ########################################
    // ### Task 4.2 (Create texture on GPU) ###
    // ########################################
    /* create the texture on the gpu for Quad */
    GLuint textureHandle;
    glCreateTextures(GL_TEXTURE_2D, 1, &textureHandle);
    glTextureParameteri(textureHandle, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(textureHandle, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(textureHandle, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(textureHandle, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureStorage2D(textureHandle, 1, GL_RGBA8, image.GetWidth(), image.GetHeight());
    glTextureSubImage2D(textureHandle, 0, 0, 0, image.GetWidth(), image.GetHeight(), GL_RGBA, GL_UNSIGNED_BYTE, image.Data());

    /* create the texture on the gpu for World Map Sphere */
    GLuint textureHandleWorldMap;
    glCreateTextures(GL_TEXTURE_2D, 1, &textureHandleWorldMap);
    glTextureParameteri(textureHandleWorldMap, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(textureHandleWorldMap, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(textureHandleWorldMap, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(textureHandleWorldMap, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureStorage2D(textureHandleWorldMap, 1, GL_RGBA8, imageWorldMap.GetWidth(), imageWorldMap.GetHeight());
    glTextureSubImage2D(textureHandleWorldMap, 0, 0, 0, imageWorldMap.GetWidth(), imageWorldMap.GetHeight(), GL_RGBA, GL_UNSIGNED_BYTE, imageWorldMap.Data());


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

        /* View mat */
        Mat4f viewMat = LookAt(
            camera.GetPosition(), camera.GetPosition() + camera.GetForward(), camera.GetUp()); // Mat4f::Identity();

        /* Projection mat */
        float aspect  = (float)windowWidth / (float)windowHeight;
        Mat4f projMat = PerspectiveProjection(TO_RAD(60.0f), aspect, 0.01f, 500.0f);

        /* Rendering */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);

        shader.Use();

        // #################################
        // ### Task 4.4 & 4.5 (Draw Quad and Sphere with textures) ###
        // #################################

        /* Render Quad with linux-quake texture */
        glBindVertexArray(VAO);
        glUniformMatrix4fv(0, 1, GL_FALSE, modelMat.Data());
        glUniformMatrix4fv(1, 1, GL_FALSE, viewMat.Data());
        glUniformMatrix4fv(2, 1, GL_FALSE, projMat.Data());
        glBindTextureUnit(0, textureHandle);    // --> works with openGL 4.5 and up
        glDrawElementsBaseVertex(GL_TRIANGLES, NUM_QUAD_INDICES, GL_UNSIGNED_SHORT, 0, 0);

        /* Render Sphere with world map texture */
        glBindVertexArray(VAO_Sphere);
        glUniformMatrix4fv(0, 1, GL_FALSE, modelMat.Data());
        glUniformMatrix4fv(1, 1, GL_FALSE, viewMat.Data());
        glUniformMatrix4fv(2, 1, GL_FALSE, projMat.Data());
        glBindTextureUnit(0, textureHandleWorldMap);
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)sphereVertices.size());



        SDL_GL_SwapWindow(pRamen->GetWindow());

        endCounter = SDL_GetPerformanceCounter();
    }

    /* GL Resources shutdown. */
    shader.Delete();
    glDeleteTextures(1, &textureHandle);
    glDeleteTextures(1, &textureHandleWorldMap);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO_Sphere);
    glDeleteVertexArrays(1, &VAO_Sphere);

    /* Ramen Shutdown */
    pRamen->Shutdown();

    /* Filesystem deinit */
    PHYSFS_deinit();

    return 0;
}
