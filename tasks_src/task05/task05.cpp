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

std::vector<Vertex> CreateCube(const Vec3f& color);
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

std::vector<Vertex> CreateNormalLines(const std::vector<Vertex>& vertices, float lineLength, bool drawNormals) {
    std::vector<Vertex> lines;
    if (!drawNormals) return lines;

    for (const auto& v : vertices) {
        Vec3f start = v.position;
        Vec3f end = v.position + v.normal * lineLength;
        lines.push_back({start, {0, 0, 0}, {1, 1, 1}});
        lines.push_back({end, {0, 0, 0}, {1, 1, 1}});
    }
    return lines;
}

int main(int argc, char** argv)
{
    Filesystem* pFS = Filesystem::Init(argc, argv);

    Ramen* pRamen = Ramen::Instance();
    pRamen->Init("Task 05 - Cubemapping", 800, 600);

    /* Load shaders. */
    Shader shader{};
    if ( !shader.Load("shaders/task05.vert", "shaders/task05.frag") )
    {
        fprintf(stderr, "Could not load shader.\n");
    }

    /* Create camera */
    Camera camera(Vec3f{ 0.0f, 0.0f, 5.0f });
    camera.RotateAroundSide(0.0f);

    /* Model mat*/
    Mat4f modelMat = Mat4f::Identity();

    /* show normal vectors */
    bool showNormals = false;

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

    /* Create normal lines */
    const std::vector<Vertex> cubeNormals = CreateNormalLines(cubeVertices);
    auto [VAO_CubeNormals, VBO_CubeNormals] = CreateGeometryBuffers(cubeNormals);

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
                case SDLK_W:
                    {
                        camera.Translate(camera.GetForward() * 0.1f);
                    }
                    break;
                case SDLK_S:
                    {
                        camera.Translate(-camera.GetForward() * 0.1f);
                    }
                    break;
                case SDLK_A:
                    {
                        camera.Translate(-camera.GetRight() * 0.1f);
                    }
                    break;
                case SDLK_D:
                    {
                        camera.Translate(camera.GetRight() * 0.1f);
                    }
                    break;
                case SDLK_Q:
                    {
                        camera.Translate(camera.GetUp() * 0.1f);
                    }
                    break;
                case SDLK_E:
                    {
                        camera.Translate(-camera.GetUp() * 0.1f);
                    }
                    break;
                case SDLK_UP:
                    {
                        camera.Pitch(1.0f);
                    }
                    break;
                case SDLK_DOWN:
                    {
                        camera.Pitch(-1.0f);
                    }
                    break;
                case SDLK_LEFT:
                    {
                        camera.Yaw(1.0f);
                    }
                    break;
                case SDLK_RIGHT:
                    {
                        camera.Yaw(-1.0f);
                    }
                    break;
                case SDLK_PAGEUP:
                    {
                        camera.Roll(1.0f);
                    }
                    break;
                case SDLK_PAGEDOWN:
                    {
                        camera.Roll(-1.0f);
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

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Cubemap settings");

        /* TODO: Implement your own UI here. */

        ImGui::End();

        /* ImGUI Rendering */
        ImGui::Render();

        /* Rendering */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);

        shader.Use();

        glBindVertexArray(VAO_Cube);
        glUniformMatrix4fv(0, 1, GL_FALSE, modelMat.Data());
        glUniformMatrix4fv(1, 1, GL_FALSE, viewMat.Data());
        glUniformMatrix4fv(2, 1, GL_FALSE, projMat.Data());
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)cubeVertices.size());

        if (showNormals)
        {
            glBindVertexArray(VAO_CubeNormals);
            glUniformMatrix4fv(0, 1, GL_FALSE, modelMat.Data());
            glUniformMatrix4fv(1, 1, GL_FALSE, viewMat.Data());
            glUniformMatrix4fv(2, 1, GL_FALSE, projMat.Data());
            glDrawArrays(GL_LINES, 0, (GLsizei)cubeNormals.size());
        }

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(pRamen->GetWindow());

        endCounter = SDL_GetPerformanceCounter();
    }

    /* GL Resources shutdown. */
    shader.Delete();
    glDeleteBuffers(1, &VBO_Cube);
    glDeleteVertexArrays(1, &VAO_Cube);
    glDeleteBuffers(1, &VBO_CubeNormals);
    glDeleteVertexArrays(1, &VAO_CubeNormals);

    /* Ramen Shutdown */
    pRamen->Shutdown();

    /* Filesystem deinit */
    PHYSFS_deinit();

    return 0;
}
