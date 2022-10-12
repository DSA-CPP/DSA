# Compilation

## General

- C++ 20 with G++
- should pass pedantic, all, extra and conversion
- `-O3` for releases, `-D _DEBUG` for debugs
- compile ***test/main.cpp*** with `-Isrc` under release-settings

## Client

- Dear ImGui with GLFW & OpenGL3
- NETpp (aka. net++)

### Windows (MinGW-w64)

- ***lib/libgui.a*** containing:
    - all of ***libglfw3.a*** (64-bit pre-compiled binaries)
    - compiled ***imgui_\*.cpp***s (Intel amd64 PE/COFF)
- ***opengl32.dll***
- ***gdi32.dll*** (often linked by default)
- target = Windows (not necessary, but highly recommended)

### Linux

- ***lib/libimgui.a*** containing:
    - compiled ***imgui_\*.cpp***s (ELF 64-bit)
- ***libglfw.so***
    - use package's corresponding headers
- ***libOpenGL.so***

## Server

*todo*
