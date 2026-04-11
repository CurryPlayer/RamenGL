#include <glad/glad.h>

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <vector>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_sdl3.h>

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
#include "../ramen/rgl_utils.h"

std::vector<Vertex> CreateCube(const Vec3f& color);
std::vector<Vertex> CreateCylinder(const Vec3f& color, int slices = 16);
std::vector<Vertex> CreateSphere(const Vec3f& color, int stacks = 16, int slices = 16);

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

std::vector<Vertex> CreateCylinder(const Vec3f& color, const int slices) {
    std::vector<Vertex> vertices;
    constexpr float height = 1.0f;
    constexpr float radius = 0.5f;
    // Bottom cap
    for (int i = 0; i < slices; i++) {
        const float angle = 2 * RAMEN_PI * i / slices;
        const float nextAngle = 2 * RAMEN_PI * (i + 1) / slices;
        const Vec3f p1 = {0, -height / 2, 0};
        const Vec3f p2 = {radius * cosf(angle), -height / 2, radius * sinf(angle)};
        const Vec3f p3 = {radius * cosf(nextAngle), -height / 2, radius * sinf(nextAngle)};
        vertices.push_back({p1, {0, -1, 0}, color});
        vertices.push_back({p2, {0, -1, 0}, color});
        vertices.push_back({p3, {0, -1, 0}, color});
    }
    // Top cap
    for (int i = 0; i < slices; i++) {
        const float angle = 2 * RAMEN_PI * i / slices;
        const float nextAngle = 2 * RAMEN_PI * (i + 1) / slices;
        const Vec3f p1 = {0, height / 2, 0};
        const Vec3f p2 = {radius * cosf(angle), height / 2, radius * sinf(angle)};
        const Vec3f p3 = {radius * cosf(nextAngle), height / 2, radius * sinf(nextAngle)};
        vertices.push_back({p1, {0, 1, 0}, color});
        vertices.push_back({p3, {0, 1, 0}, color});
        vertices.push_back({p2, {0, 1, 0}, color});
    }
    // Sides
    for (int i = 0; i < slices; i++) {
        const float angle = 2 * RAMEN_PI * i / slices;
        const float nextAngle = 2 * RAMEN_PI * (i + 1) / slices;
        const Vec3f n = {cosf(angle), 0, sinf(angle)};
        const Vec3f p1 = {radius * cosf(angle), -height / 2, radius * sinf(angle)};
        const Vec3f p2 = {radius * cosf(angle), height / 2, radius * sinf(angle)};
        const Vec3f p3 = {radius * cosf(nextAngle), -height / 2, radius * sinf(nextAngle)};
        const Vec3f p4 = {radius * cosf(nextAngle), height / 2, radius * sinf(nextAngle)};
        vertices.push_back({p1, n, color});
        vertices.push_back({p2, n, color});
        vertices.push_back({p3, n, color});
        vertices.push_back({p2, n, color});
        vertices.push_back({p4, n, color});
        vertices.push_back({p3, n, color});
    }
    return vertices;
}

std::vector<Vertex> CreateSphere(const Vec3f& color, int stacks, int slices) {
    std::vector<Vertex> vertices;
    float radius = 0.5f;
    for (int i = 0; i < stacks; i++) {
        float phi1 = RAMEN_PI * i / stacks;
        float phi2 = RAMEN_PI * (i + 1) / stacks;
        for (int j = 0; j < slices; j++) {
            float theta1 = 2 * RAMEN_PI * j / slices;
            float theta2 = 2 * RAMEN_PI * (j + 1) / slices;
            Vec3f p1 = {radius * sinf(phi1) * cosf(theta1), radius * cosf(phi1), radius * sinf(phi1) * sinf(theta1)};
            Vec3f p2 = {radius * sinf(phi1) * cosf(theta2), radius * cosf(phi1), radius * sinf(phi1) * sinf(theta2)};
            Vec3f p3 = {radius * sinf(phi2) * cosf(theta1), radius * cosf(phi2), radius * sinf(phi2) * sinf(theta1)};
            Vec3f p4 = {radius * sinf(phi2) * cosf(theta2), radius * cosf(phi2), radius * sinf(phi2) * sinf(theta2)};
            Vec3f n1 = Normalize(p1);
            Vec3f n2 = Normalize(p2);
            Vec3f n3 = Normalize(p3);
            Vec3f n4 = Normalize(p4);
            vertices.push_back({p1, n1, color});
            vertices.push_back({p2, n2, color});
            vertices.push_back({p3, n3, color});
            vertices.push_back({p2, n2, color});
            vertices.push_back({p4, n4, color});
            vertices.push_back({p3, n3, color});
        }
    }
    return vertices;
}

int main(int argc, char** argv)
{
    Filesystem* pFS = Filesystem::Init(argc, argv, "assets");

    Ramen* pRamen = Ramen::Instance();
    pRamen->Init("GUI", 800, 600);

    /* Load shaders. */
    Shader shader{};
    if ( !shader.Load("shaders/task03.vert", "shaders/task03.frag") )
    {
        fprintf(stderr, "Could not load shader.\n");
    }

    /* Create camera */
    Camera camera(Vec3f{ 0.0f, 2.0f, 3.0f });
    camera.RotateAroundSide(-10.0f);

    /* Model mat*/
    Mat4f modelMat = Mat4f::Identity();

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

    /* Create coordinate system */
    GLuint VAO_CoordSystem;
    GLuint VBO_CoordSystem;
    {
        glCreateBuffers(1, &VBO_CoordSystem);
        glNamedBufferData(VBO_CoordSystem, 6 * sizeof(Vertex), Utils::CoordSystemRHZU(), GL_STATIC_DRAW);
        glCreateVertexArrays(1, &VAO_CoordSystem);
        glVertexArrayVertexBuffer(VAO_CoordSystem, 0, VBO_CoordSystem, 0, sizeof(Vertex));
        glVertexArrayAttribFormat(VAO_CoordSystem, 0, 3, GL_FLOAT, GL_FALSE, 0);
        glEnableVertexArrayAttrib(VAO_CoordSystem, 0);
        glVertexArrayAttribBinding(VAO_CoordSystem, 0, 0);
        glVertexArrayAttribFormat(VAO_CoordSystem, 1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float));
        glEnableVertexArrayAttrib(VAO_CoordSystem, 1);
        glVertexArrayAttribBinding(VAO_CoordSystem, 1, 0);
        glVertexArrayAttribFormat(VAO_CoordSystem, 2, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float));
        glEnableVertexArrayAttrib(VAO_CoordSystem, 2);
        glVertexArrayAttribBinding(VAO_CoordSystem, 2, 0);
    }

    /* Create geometries */
    std::vector<Vertex> cubeVertices = CreateCube({1.0f, 0.0f, 0.0f}); // Red
    auto [VAO_Cube, VBO_Cube] = CreateGeometryBuffers(cubeVertices);

    std::vector<Vertex> cylinderVertices = CreateCylinder({0.0f, 1.0f, 0.0f}); // Green
    auto [VAO_Cylinder, VBO_Cylinder] = CreateGeometryBuffers(cylinderVertices);

    std::vector<Vertex> sphereVertices = CreateSphere({0.0f, 0.0f, 1.0f}); // Blue
    auto [VAO_Sphere, VBO_Sphere] = CreateGeometryBuffers(sphereVertices);

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
    while ( isRunning )
    {

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

        /* ImGUI Rendering */
        ImGui::Render();

        /* Rendering */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);

        shader.Use();

        /* Render coordinate system at origin */
        glBindVertexArray(VAO_CoordSystem);
        Mat4f coordSysMat = Mat4f::Identity();
        glUniformMatrix4fv(0, 1, GL_FALSE, coordSysMat.Data());
        glUniformMatrix4fv(1, 1, GL_FALSE, viewMat.Data());
        glUniformMatrix4fv(2, 1, GL_FALSE, projMat.Data());

        glDrawArrays(GL_LINES, 0, 6);

        /* Render cube (red) */
        glBindVertexArray(VAO_Cube);
        Mat4f cubeMat = Translate(Vec3f{-1.5f, 0.0f, 0.0f});
        glUniformMatrix4fv(0, 1, GL_FALSE, cubeMat.Data());
        glUniformMatrix4fv(1, 1, GL_FALSE, viewMat.Data());
        glUniformMatrix4fv(2, 1, GL_FALSE, projMat.Data());
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)cubeVertices.size());

        /* Render cylinder (green) */
        glBindVertexArray(VAO_Cylinder);
        Mat4f cylinderMat = Mat4f::Identity();
        glUniformMatrix4fv(0, 1, GL_FALSE, cylinderMat.Data());
        glUniformMatrix4fv(1, 1, GL_FALSE, viewMat.Data());
        glUniformMatrix4fv(2, 1, GL_FALSE, projMat.Data());
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)cylinderVertices.size());

        /* Render sphere (blue) */
        glBindVertexArray(VAO_Sphere);
        Mat4f sphereMat = Translate(Vec3f{1.5f, 0.0f, 0.0f});
        glUniformMatrix4fv(0, 1, GL_FALSE, sphereMat.Data());
        glUniformMatrix4fv(1, 1, GL_FALSE, viewMat.Data());
        glUniformMatrix4fv(2, 1, GL_FALSE, projMat.Data());
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)sphereVertices.size());

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(pRamen->GetWindow());
    }

    /* GL Resources shutdown. */
    shader.Delete();
    glDeleteBuffers(1, &VBO_CoordSystem);
    glDeleteVertexArrays(1, &VAO_CoordSystem);
    glDeleteBuffers(1, &VBO_Cube);
    glDeleteVertexArrays(1, &VAO_Cube);
    glDeleteBuffers(1, &VBO_Cylinder);
    glDeleteVertexArrays(1, &VAO_Cylinder);
    glDeleteBuffers(1, &VBO_Sphere);
    glDeleteVertexArrays(1, &VAO_Sphere);

    /* Ramen Shutdown */
    pRamen->Shutdown();

    /* Filesystem deinit */
    PHYSFS_deinit();

    return 0;
}
