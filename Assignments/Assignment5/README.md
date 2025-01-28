# Assignment 5

This program generates triangle meshes and uses non-trivial shaders in OpenGL.

- Implement and execute the marching cubes algorithm on an arbitrary scalar field.
- Implement a shader with lighting.
- Manipulate the view matrix to have the appearance that the object is rotating around the origin.
- Write triangle mesh data to a PLY file.
- The triangle mesh is rendered during the marching algorithm. Thus, the mesh is continuously “grow” as the algorithm progresses.


### Build and Run Example
compile: g++ Assign_5.cpp -o Assign_5 -lGLEW -lGLFW -lGL -lGLU -std=c++11
run: ./TAssign_5
