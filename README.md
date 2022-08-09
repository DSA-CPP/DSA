# Compilation

## General

- C++ 20
- should pass pedantic, all, extra and conversion
- `-O3` for releases, `-D _DEBUG` for debugs
- Dear ImGui with GLFW & OpenGL3
- compile ***test/main.cpp*** with`-Isrc` under release-settings

## Windows (MinGW-w64)

- ***lib/libgui.a*** containing:
    - all of ***libglfw3.a***
    - compiled ***imgui_\*.cpp***s (Intel amd64 PE/COFF)
- ***opengl32.dll***
- ***gdi32.dll***
- target Windows

## Linux

- ***lib/libimgui.a*** containing:
    - compiled ***imgui_\*.cpp***s (ELF 64-bit)
- ***libglfw.so***
    - use corresponding headers
- ***libOpenGL.so***
