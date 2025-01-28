#ifndef LOADPLY_HPP
#define LOADPLY_HPP

#include <vector>
#include <string>
#include <fstream>
#include <sstream>

struct Vertex {
    float x, y, z;    // Position
    float nx, ny, nz; // Normal
    float u, v;       // Texture coordinates
};

struct Face {
    std::vector<int> indices;
};

void loadPLY(const char* filename, std::vector<Vertex>& vertices, std::vector<Face>& faces) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        printf("%s could not be opened.\n", filename);
        return;
    }

    std::string line;
    bool headerEnded = false;
    int vertexCount = 0;
    int faceCount = 0;

    // Read header
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string token;
        iss >> token;

        if (token == "end_header") {
            headerEnded = true;
            break;
        } else if (token == "element") {
            iss >> token;
            if (token == "vertex") {
                iss >> vertexCount;
            } else if (token == "face") {
                iss >> faceCount;
            }
        }
    }

    if (!headerEnded) {
        printf("PLY header not properly terminated.\n");
        return;
    }

    // Read vertices
    vertices.reserve(vertexCount);
    for (int i = 0; i < vertexCount; ++i) {
        std::getline(file, line);
        std::istringstream iss(line);
        Vertex vertex;
        iss >> vertex.x >> vertex.y >> vertex.z;
        iss >> vertex.nx >> vertex.ny >> vertex.nz;
        iss >> vertex.u >> vertex.v;
        vertices.push_back(vertex);
    }

    // Read faces
    faces.reserve(faceCount);
    for (int i = 0; i < faceCount; ++i) {
        std::getline(file, line);
        std::istringstream iss(line);
        int vertexCount;
        iss >> vertexCount;
        Face face;
        for (int j = 0; j < vertexCount; ++j) {
            int index;
            iss >> index;
            face.indices.push_back(index);
        }
        faces.push_back(face);
    }

    file.close();
}

#endif // LOADPLY_HPP
