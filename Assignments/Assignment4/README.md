# Assignment 4

This program loads triangle mesh data and textures from files, renders them, and allows the user to explore that rendered world by moving the camera around via arrow keys.

- Manipulating the view matrix to move the camera around in world space
- Reading triangle mesh data from PLY files
- Rendering textured triangle meshes using Vertex Buffer Objects (VBOs), Vertex Array Objects (VAOs), and shaders

### Build and Run Example
compile: g++ TexturedMesh.cpp -o TexturedMesh -lGLEW -lGLFW -lGL -lGLU -std=c++11
run: ./TexturedMesh
