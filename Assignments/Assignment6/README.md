# Assignment 5

This program generate a basic water shader that uses tesselation, geometry shaders, and displacement maps.

- Take a simple mesh of triangles as input.
- Tesselate that mesh to get more vertices to play with in the geometry shader.
- Manipulate the mesh in the geometry shader to create waves.
- Write triangle mesh data to a PLY file.
- The triangle mesh is rendered during the marching algorithm. Thus, the mesh is continuously “grow” as the algorithm progresses.


### Build and Run Example
compile: g++ -std=c++11 A6-Water.cpp -lglfw -lGLEW -lGL -o water
run: ./water
