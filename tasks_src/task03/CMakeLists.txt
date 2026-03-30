cmake_minimum_required(VERSION 3.30.2)
project(Tasks VERSION 0.0.1)

add_executable(task03
    ../../dependencies/glad/src/glad.c

    ../../dependencies/tiny_obj_loader.h

    ../../dependencies/stb_image.h
    
    ../../dependencies/imgui/imgui.cpp
    ../../dependencies/imgui/imgui_impl_sdl3.cpp
    ../../dependencies/imgui/imgui_impl_opengl3.cpp
    ../../dependencies/imgui/imgui_draw.cpp
    ../../dependencies/imgui/imgui_tables.cpp
    ../../dependencies/imgui/imgui_demo.cpp
    ../../dependencies/imgui/imgui_widgets.cpp

    ../../ramen/ramen.cpp
    ../../ramen/rgl_math.cpp
    ../../ramen/rgl_platform.cpp
    ../../ramen/rgl_shader.cpp
    ../../ramen/rgl_filesystem.cpp
    ../../ramen/rgl_image.cpp
    ../../ramen/rgl_utils.cpp

    ./task03.cpp
)

target_include_directories(task03 PUBLIC
    ../../dependencies/glad/include
    ../../dependencies/)
target_link_libraries(task03
    PRIVATE SDL3::SDL3
    PRIVATE physfs)


if (UNIX)
    set(CMAKE_BUILD_RPATH_USE_ORIGIN TRUE)
    set(CMAKE_SKIP_RPATH OFF CACHE BOOL "Skip RPATH" FORCE)
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    set(CMAKE_INSTALL_RPATH "$ORIGIN/")
    set(ldflags "${ldflags} -Wl,-z,origin")
endif()
