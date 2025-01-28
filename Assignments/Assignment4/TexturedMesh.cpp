#define GLM_ENABLE_EXPERIMENTAL
#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype> 
#include <algorithm>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

struct VertexData
{
    float x, y, z;         // Position is mandatory
    float nx, ny, nz;      // Normal vector (optional)
    unsigned char r, g, b; // Color (optional)
    float u, v;            // Texture coordinates (optional)

    VertexData() : x(0), y(0), z(0), nx(0), ny(0), nz(0), r(255), g(255), b(255), u(0), v(0) {}
};

struct TriData
{
    int v1, v2, v3; // Indices for the triangle's vertices

    TriData(int vertex1, int vertex2, int vertex3) : v1(vertex1), v2(vertex2), v3(vertex3) {}
};

class Camera
{
public:
    glm::vec3 position;
    glm::vec3 direction;
    const float moveSpeed = 0.05f;
    const float rotationAngle = 3.0f;

    // Begin with the camera being positioned at (0.5,0.4,0.5) in world space and looking in the direction of (0,0,−1).
    Camera() : position(0.5f, 0.4f, 0.5f), direction(0.0f, 0.0f, -1.0f) {}

    // Use the arrow keys (up, down, left, right), to move around the camera.
    void update(int key)
    {
        if (key == GLFW_KEY_UP)
        {
            position += direction * moveSpeed;
        }
        else if (key == GLFW_KEY_DOWN)
        {
            position -= direction * moveSpeed;
        }
        else if (key == GLFW_KEY_LEFT)
        {
            float angle = glm::radians(rotationAngle);
            direction = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(direction, 1.0f);
        }
        else if (key == GLFW_KEY_RIGHT)
        {
            float angle = glm::radians(-rotationAngle);
            direction = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(direction, 1.0f);
        }
    }

    glm::mat4 getViewMatrix() const
    {
        // Create a view matrix directed at where the camera is looking
        glm::vec3 up(0, 1, 0); // Y is up in this context
        return glm::lookAt(position, position + direction, up);
    }
};

class TexturedMesh
{
private:
    GLuint vertexBufferID; // An integer ID for the VBO to store the vertex positions.
    GLuint uvBufferID; // An integer ID for the VBO to store the texture coordinates.
    GLuint indexBufferID; // An integer ID for the VBO to store the face’s vertex indices
    GLuint textureID; // An integer ID for the Texture Object created to store the bitmap image.
    GLuint vaoID; // An integer ID for the VAO used to render the texture mesh.
    GLuint shaderProgramID; // An integer ID for the shader program created and linked to render the particular textured mesh.
    std::vector<VertexData> vertices;
    std::vector<TriData> faces;

    void readPLYFile(const std::string &filename, std::vector<VertexData> &vertices, std::vector<TriData> &faces)
    {
        std::ifstream file(filename);
        if (!file.is_open())
        {
            throw std::runtime_error("Could not open file: " + filename);
        }

        std::string line;
        bool endHeader = false;
        bool hasNormal = false;
        bool hasColor = false;
        bool hasTexture = false;

        size_t numVertices = 0;
        size_t numFaces = 0;

        while (std::getline(file, line) && !endHeader)
        {
            std::istringstream iss(line);
            std::string token;
            iss >> token;
            if (token == "element")
            {
                iss >> token;
                if (token == "vertex")
                {
                    iss >> numVertices;
                }
                else if (token == "face")
                {
                    iss >> numFaces;
                }
            }
            else if (token == "property")
            {
                iss >> token; // Skip the data type token
                iss >> token;
                if (token == "nx" || token == "ny" || token == "nz")
                {
                    hasNormal = true;
                }
                else if (token == "red" || token == "green" || token == "blue")
                {
                    hasColor = true;
                }
                else if (token == "u" || token == "v")
                {
                    hasTexture = true;
                }
            }
            else if (token == "end_header")
            {
                endHeader = true;
            }
        }

        vertices.reserve(numVertices);
        for (size_t i = 0; i < numVertices; ++i)
        {
            std::getline(file, line);
            std::istringstream vertexLine(line);

            VertexData vertex;
            vertexLine >> vertex.x >> vertex.y >> vertex.z;

            if (hasNormal)
            {
                vertexLine >> vertex.nx >> vertex.ny >> vertex.nz;
            }
            if (hasColor)
            {
                int r, g, b;
                vertexLine >> r >> g >> b;
                vertex.r = static_cast<unsigned char>(r);
                vertex.g = static_cast<unsigned char>(g);
                vertex.b = static_cast<unsigned char>(b);
            }
            if (hasTexture)
            {
                vertexLine >> vertex.u >> vertex.v;
            }

            vertices.push_back(vertex);
        }

        faces.reserve(numFaces);
        for (size_t i = 0; i < numFaces; ++i)
        {
            std::getline(file, line);
            std::istringstream faceLine(line);

            int verticesCount;
            faceLine >> verticesCount;
            if (verticesCount != 3)
            {
                std::cerr << "Non-triangular face detected." << std::endl;
                continue;
            }

            TriData tri(0, 0, 0);
            faceLine >> tri.v1 >> tri.v2 >> tri.v3;
            faces.push_back(tri);
        }

        file.close();
    }
    void loadARGB_BMP(const char *imagepath, unsigned char **data, unsigned int *width, unsigned int *height)
    {

        printf("Reading image %s\n", imagepath);

        // Data read from the header of the BMP file
        unsigned char header[54];
        unsigned int dataPos;
        unsigned int imageSize;
        // Actual RGBA data

        // Open the file
        FILE *file = fopen(imagepath, "rb");
        if (!file)
        {
            printf("%s could not be opened. Are you in the right directory?\n", imagepath);
            getchar();
            return;
        }

        // Read the header, i.e. the 54 first bytes

        // If less than 54 bytes are read, problem
        if (fread(header, 1, 54, file) != 54)
        {
            printf("Not a correct BMP file1\n");
            fclose(file);
            return;
        }

        // Read the information about the image
        dataPos = *(int *)&(header[0x0A]);
        imageSize = *(int *)&(header[0x22]);
        *width = *(int *)&(header[0x12]);
        *height = *(int *)&(header[0x16]);
        // A BMP files always begins with "BM"
        if (header[0] != 'B' || header[1] != 'M')
        {
            printf("Not a correct BMP file2\n");
            fclose(file);
            return;
        }
        // Make sure this is a 32bpp file
        if (*(int *)&(header[0x1E]) != 3)
        {
            printf("Not a correct BMP file3\n");
            fclose(file);
            return;
        }

        // Some BMP files are misformatted, guess missing information
        if (imageSize == 0)
            imageSize = (*width) * (*height) * 4; // 4 : one byte for each Red, Green, Blue, Alpha component
        if (dataPos == 0)
            dataPos = 54; // The BMP header is done that way

        // Create a buffer
        *data = new unsigned char[imageSize];

        if (dataPos != 54)
        {
            fread(header, 1, dataPos - 54, file);
        }

        // Read the actual data from the file into the buffer
        fread(*data, 1, imageSize, file);

        // Everything is in memory now, the file can be closed.
        fclose(file);
    }

    
    void loadBuffers()
    {
        // Generate and bind the VAO
        glGenVertexArrays(1, &vaoID);
        glBindVertexArray(vaoID);

        // Load data into vertex buffers
        glGenBuffers(1, &vertexBufferID);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexData), &vertices[0], GL_STATIC_DRAW);
        std::cout << "Vertex buffer size: " << vertices.size() * sizeof(VertexData) << " bytes" << std::endl;

        // Set the vertex attribute pointers
        // Vertex Positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void *)0);

        // Texture Coordinates
        glGenBuffers(1, &uvBufferID);
        glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexData), &vertices[0].u, GL_STATIC_DRAW);
        std::cout << "Texture coordinate buffer size: " << vertices.size() * sizeof(VertexData) << " bytes" << std::endl;

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void *)offsetof(VertexData, u));

        // Indices for drawing triangles
        glGenBuffers(1, &indexBufferID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof(TriData), &faces[0], GL_STATIC_DRAW);
        std::cout << "Index buffer size: " << faces.size() * sizeof(TriData) << " bytes" << std::endl;

        // Unbind the VAO
        glBindVertexArray(0);
    }

    void loadTexture(const std::string &texturePath)
    {
        // Generate texture ID and load texture data
        unsigned char *imageData;
        unsigned int width, height;
        loadARGB_BMP(texturePath.c_str(), &imageData, &width, &height);

        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
        glGenerateMipmap(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, 0);
        delete[] imageData;
    }

    void loadShaders()
    {

        // Create the shaders
        GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
        GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
        std::string VertexShaderCode = "\
    	#version 330 core\n\
		// Input vertex data, different for all executions of this shader.\n\
		layout(location = 0) in vec3 vertexPosition;\n\
		layout(location = 1) in vec2 uv;\n\
		// Output data ; will be interpolated for each fragment.\n\
		out vec2 uv_out;\n\
		// Values that stay constant for the whole mesh.\n\
		uniform mat4 MVP;\n\
		void main(){ \n\
			// Output position of the vertex, in clip space : MVP * position\n\
			gl_Position =  MVP * vec4(vertexPosition,1);\n\
			// The color will be interpolated to produce the color of each fragment\n\
			uv_out = uv;\n\
		}\n";

        // Read the Fragment Shader code from the file
        std::string FragmentShaderCode = "\
		#version 330 core\n\
		in vec2 uv_out; \n\
		uniform sampler2D tex;\n\
		void main() {\n\
			gl_FragColor = texture(tex, uv_out);\n\
		}\n";
        char const *VertexSourcePointer = VertexShaderCode.c_str();
        glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
        glCompileShader(VertexShaderID);

        // Compile Fragment Shader
        char const *FragmentSourcePointer = FragmentShaderCode.c_str();
        glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
        glCompileShader(FragmentShaderID);

        GLuint ProgramID = glCreateProgram();
        glAttachShader(ProgramID, VertexShaderID);
        glAttachShader(ProgramID, FragmentShaderID);
        glLinkProgram(ProgramID);

        glDetachShader(ProgramID, VertexShaderID);
        glDetachShader(ProgramID, FragmentShaderID);

        glDeleteShader(VertexShaderID);
        glDeleteShader(FragmentShaderID);
    }

public:
    TexturedMesh(const std::string &plyPath, const std::string &texturePath)
    {
        readPLYFile(plyPath, vertices, faces);
        loadBuffers();
        loadTexture(texturePath);
        loadShaders();
    }

    ~TexturedMesh()
    {
        // Properly delete all the buffers and the texture
        glDeleteBuffers(1, &vertexBufferID);
        glDeleteBuffers(1, &uvBufferID);
        glDeleteBuffers(1, &indexBufferID);
        glDeleteTextures(1, &textureID);
        glDeleteVertexArrays(1, &vaoID);
        glDeleteProgram(shaderProgramID);
    }

    void draw(const glm::mat4 &MVP)
    {
        // Use shader program
        glUseProgram(shaderProgramID);

        // Bind texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);

        // Bind VAO
        glBindVertexArray(vaoID);

        // Set the MVP matrix for the shader
        glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "MVP"), 1, GL_FALSE, &MVP[0][0]);

        // Draw the mesh
        glDrawElements(GL_TRIANGLES, faces.size() * 3, GL_UNSIGNED_INT, 0);

        // Unbind everything
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glUseProgram(0);
    }
};

int main()
{
    std::vector<TexturedMesh> opaqueMeshes;
    std::vector<TexturedMesh> transparentMeshes;

    // File names without extension
    std::vector<std::string> fileNames = {
        "Bottles", "Curtains", "DoorBG", "Floor", "MetalObjects",
        "Patio", "Table", "Walls", "WindowBG", "WoodObjects"};

    // Iterate over each file name, create TexturedMesh objects and add to the vector
    for (const std::string &name : fileNames)
    {   
        // Convert the name to lowercase for file paths
        std::string lowerCaseName = name;
        std::transform(lowerCaseName.begin(), lowerCaseName.end(), lowerCaseName.begin(),
               [](unsigned char c){ return std::tolower(c); });

        std::string plyFilePath = "LinksHouse/" + name + ".ply";
        std::string bmpFilePath = "LinksHouse/" + lowerCaseName + ".bmp";
        
        if (name == "DoorBG" || name == "MetalObjects" || name == "Curtains")
        {
            transparentMeshes.emplace_back(plyFilePath, bmpFilePath);
        }
        else
        {
            opaqueMeshes.emplace_back(plyFilePath, bmpFilePath);
        }
    }

    // Initialise GLFW
    if (!glfwInit())
    {
        return -1;
    }

    // Open a window and create its OpenGL context
    float screenW = 1400;
    float screenH = 900;
    GLFWwindow *window = glfwCreateWindow(screenW, screenH, "Room View", NULL, NULL);
    if (window == NULL)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Create camera
    Camera camera;

    // Create TexturedMesh objects
    TexturedMesh myMesh("mesh.ply", "texture.bmp");

    // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);

    // Camera matrix
    glm::mat4 View = glm::lookAt(
        glm::vec3(4, 3, -3), // Camera is at (4,3,-3), in World Space
        glm::vec3(0, 0, 0),  // and looks at the origin
        glm::vec3(0, 1, 0)   // Head is up (set to 0,-1,0 to look upside-down)
    );

    // Model matrix : an identity matrix (model will be at the origin)
    glm::mat4 Model = glm::mat4(1.0f);

    // Our ModelViewProjection : multiplication of our 3 matrices
    glm::mat4 MVP = Projection * View * Model; // Remember, matrix multiplication is the other way around

    do
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Camera key controls
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.update(GLFW_KEY_UP);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.update(GLFW_KEY_DOWN);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.update(GLFW_KEY_LEFT);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.update(GLFW_KEY_RIGHT);

        // Update camera view matrix
        View = camera.getViewMatrix();

        // Render opaque meshes
        for (auto &mesh : opaqueMeshes)
        {
            glm::mat4 MVP = Projection * View * Model; // Compute the MVP matrix
            mesh.draw(MVP);
        }

         // Enable blending
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Render transparent meshes
        for (auto &mesh : transparentMeshes)
        {
            glm::mat4 MVP = Projection * View * Model; // Compute the MVP matrix
            mesh.draw(MVP);
        }

        // Disable blending after rendering transparent meshes
        glDisable(GL_BLEND);
        
        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

    // Cleanup and close window
    glfwTerminate();

    return 0;
}
