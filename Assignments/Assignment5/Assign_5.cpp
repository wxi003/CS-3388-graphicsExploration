#define GLM_ENABLE_EXPERIMENTAL
// Include standard headers
#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow *window;


// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <functional>
#include <cmath>
#include "TriTable.hpp"

#define FRONT_TOP_LEFT 128
#define FRONT_TOP_RIGHT 64
#define BACK_TOP_RIGHT 32
#define BACK_TOP_LEFT 16
#define FRONT_BOTTOM_LEFT 8
#define FRONT_BOTTOM_RIGHT 4
#define BACK_BOTTOM_RIGHT 2
#define BACK_BOTTOM_LEFT 1

float function1(float x, float y, float z)
{

    return x * x - y * y - z * z - z;
}

float function2(float x, float y, float z)
{

    return y - sin(x) * cos(z);
}

const char *vertex_shader = R"glsl(
#version 330 core
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
out vec3 normal;
out vec3 eyeDirection;
out vec3 lightDirection;
uniform mat4 MVP; // The model-view-projection matrix
uniform mat4 V;   // The view matrix
uniform vec3 LightDir; // The direction of the light source

void main() {
    gl_Position = MVP * vec4(vertexPosition, 1.0);
    normal = mat3(V) * vertexNormal;
    vec3 viewPosition = vec3(5.0, 5.0, 5.0); // Camera position in world space
    eyeDirection = viewPosition - vec3(V * vec4(vertexPosition, 1.0));
    lightDirection = LightDir - vec3(V * vec4(vertexPosition, 1.0));
}
)glsl";

const char *fragment_shader = R"glsl(
#version 330 core
in vec3 normal;
in vec3 lightDirection;
in vec3 eyeDirection;
out vec4 color;
uniform vec4 modelColor;

void main() {
    vec3 n = normalize(normal);
    vec3 l = normalize(lightDirection);
    vec3 e = normalize(eyeDirection);
    vec3 r = reflect(-l, n);

    float ambientStrength = 0.2;
    vec3 ambientColor = ambientStrength * modelColor.rgb;

    float diffuseStrength = 0.8;
    float cosTheta = max(dot(n, l), 0.0);
    vec3 diffuseColor = diffuseStrength * cosTheta * modelColor.rgb;

    float specularStrength = 1.0;
    float shininess = 64.0;
    float cosAlpha = pow(max(dot(e, r), 0.0), shininess);
    vec3 specularColor = specularStrength * cosAlpha * vec3(1.0, 1.0, 1.0);

    color = vec4(ambientColor + diffuseColor + specularColor, modelColor.a);
}

)glsl";

// This function creates a first-person camera.
void cameraControlsGlobe(GLFWwindow *window, glm::mat4 &V, float &r, float &theta, float &phi) {
    static const float moveSpeed = 0.05f;  
    static const float rotateSpeed = 0.5f; 

    // Handle keyboard inputs
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        r = std::max(r - moveSpeed, 0.1f);
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        r += moveSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        theta += rotateSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        theta -= rotateSpeed;
    }

    // Handle mouse inputs for rotation
    static double lastXPos = -1, lastYPos = -1; // last cursor position
    double xPos, yPos;
    glfwGetCursorPos(window, &xPos, &yPos); // current cursor position
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        if (lastXPos >= 0 && lastYPos >= 0) { 
            theta -= rotateSpeed * float(xPos - lastXPos); // y-axis
            phi += rotateSpeed * float(yPos - lastYPos);   // x-axis
            phi = glm::clamp(phi, -89.0f, 89.0f);          
        }
    }
    lastXPos = xPos; // update last cursor position
    lastYPos = yPos; // update last cursor position

    // Update camera position and direction
    glm::vec3 cameraPosition(
        r * sin(glm::radians(theta)) * cos(glm::radians(phi)), // x
        r * sin(glm::radians(phi)),                            // y
        r * cos(glm::radians(theta)) * cos(glm::radians(phi))  // z
    );
    glm::vec3 cameraDirection = glm::normalize(-cameraPosition);
    glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);

    // Update view matrix
    V = glm::lookAt(cameraPosition, cameraPosition + cameraDirection, cameraUp);
}


// This function is using to draw the cube with axies
class Cube
{
    float min;
    float max;
    glm::vec3 z = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 y = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 x = glm::vec3(1.0f, 0.0f, 0.0f);

public:
    Cube(float min, float max) : min(min + 0.1), max(max + 0.1) {}
    void draw()
    {
        float axisExtension = 0.5f; // length of the axis extension
        float arrowSize = 0.25f;    // size of the arrow

        glMatrixMode(GL_MODELVIEW); // set the modelview matrix
        glPushMatrix();

        glLineWidth(5.0f);
        glBegin(GL_LINES);
        // Draw the cube whose opposite corners are (min, min, min) and (max, max, max). 
        // Where min and max are as the parameters to marching_cubes.
        glColor4f(1.0, 0.0f, 0.0f, 1.0f);
        glVertex3f(min, min, min);
        glVertex3f(max + axisExtension, min, min);

        glColor4f(0.0, 1.0f, 0.0f, 1.0f);
        glVertex3f(min, min, min);
        glVertex3f(min, max + axisExtension, min);

        glColor4f(0.0, 0.0f, 1.0f, 1.0f);
        glVertex3f(min, min, min);
        glVertex3f(min, min, max + axisExtension);

        glColor4f(0.5f, 0.5f, 0.5f, 1.0f);
        glColor4f(0.5f, 0.5f, 0.5f, 1.0f);
        glVertex3f(min, max, min);
        glVertex3f(max, max, min);

        glVertex3f(max, min, min);
        glVertex3f(max, max, min);

        glVertex3f(max, min, min);
        glVertex3f(max, min, max);

        glVertex3f(max, min, max);
        glVertex3f(max, max, max);

        glVertex3f(max, max, min);
        glVertex3f(max, max, max);

        glVertex3f(min, max, min);
        glVertex3f(min, max, max);

        glVertex3f(min, max, max);
        glVertex3f(min, min, max);

        glVertex3f(min, min, max);
        glVertex3f(max, min, max);

        glVertex3f(min, max, max);
        glVertex3f(max, max, max);

        glEnd();
        glBegin(GL_TRIANGLES); // draw the arrows

        glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
        glVertex3f(max + axisExtension, min, min);
        glVertex3f(max + axisExtension - arrowSize, min + arrowSize / 2, min);
        glVertex3f(max + axisExtension - arrowSize, min - arrowSize / 2, min);

        glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
        glVertex3f(min, max + axisExtension, min);
        glVertex3f(min + arrowSize / 2, max + axisExtension - arrowSize, min);
        glVertex3f(min - arrowSize / 2, max + axisExtension - arrowSize, min);

        glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
        glVertex3f(min, min, max + axisExtension);
        glVertex3f(min, min + arrowSize / 2, max + axisExtension - arrowSize);
        glVertex3f(min, min - arrowSize / 2, max + axisExtension - arrowSize);

        glEnd();

        glPopMatrix();
    }
};


// This function is to generate marching cube mesh
class MarchingCubes {
    std::function<float(float, float, float)> generationFunction;
    float isoVal = 0;
    float minCoor = 0;
    float maxCoor = 1;
    float stepSize = 0.1;
    float curIteration = 0;
    std::vector<float> vertices;

    void addTriangles(int *verts, float x, float y, float z) {
        for (int i = 0; verts[i] >= 0; i += 3) {
            for (int j = 0; j < 3; ++j) {
                vertices.emplace_back(x + stepSize * vertTable[verts[i + j]][0]);
                vertices.emplace_back(y + stepSize * vertTable[verts[i + j]][1]);
                vertices.emplace_back(z + stepSize * vertTable[verts[i + j]][2]);
            }
        }
    }

    void iterativeGeneration() {
        for (float a = minCoor; a < maxCoor; a += stepSize) {
            for (float b = minCoor; b < maxCoor; b += stepSize) {
                int cornerVal = 0;
                float cornerValues[8] = {
                    generationFunction(a, b + stepSize, curIteration + stepSize),
                    generationFunction(a + stepSize, b + stepSize, curIteration + stepSize),
                    generationFunction(a + stepSize, b, curIteration + stepSize),
                    generationFunction(a, b, curIteration + stepSize),
                    generationFunction(a, b + stepSize, curIteration),
                    generationFunction(a + stepSize, b + stepSize, curIteration),
                    generationFunction(a + stepSize, b, curIteration),
                    generationFunction(a, b, curIteration)
                };

                for (int i = 0; i < 8; ++i) {
                    if (cornerValues[i] < isoVal) {
                        cornerVal |= (1 << i);
                    }
                }

                addTriangles(marching_cubes_lut[cornerVal], a, b, curIteration);
            }
        }
        curIteration += stepSize;
        if (curIteration > maxCoor) {
            finished = true;
        }
    }

public:
    bool finished = false;

    MarchingCubes(std::function<float(float, float, float)> f, float isoval, float min, float max, float step)
        : generationFunction(f), isoVal(isoval), minCoor(min), maxCoor(max), stepSize(step), curIteration(min) {}

    void generate() {
        iterativeGeneration();
    }

    std::vector<float> getVertices() {
        return vertices;
    }

    std::vector<float> computeNormals(const std::vector<float> &vertices) {
        std::vector<float> normals;
        for (int i = 0; i < vertices.size(); i += 9) {
            glm::vec3 p1(vertices[i], vertices[i + 1], vertices[i + 2]);
            glm::vec3 p2(vertices[i + 3], vertices[i + 4], vertices[i + 5]);
            glm::vec3 p3(vertices[i + 6], vertices[i + 7], vertices[i + 8]);
            glm::vec3 normal = glm::normalize(glm::cross(p2 - p1, p3 - p1));
            for (int j = 0; j < 3; ++j) {
                normals.insert(normals.end(), {normal.x, normal.y, normal.z});
            }
        }
        return normals;
    }
};


// This function will create the file named fileName and,
// write to it a valid PLY file encoding the vertices and normals in vertices and normals.
void writePLY(const std::vector<float> &vertices, const std::vector<float> &normals, const std::string &fileName) {
    std::ofstream plyFile(fileName);
    if (!plyFile) {
        std::cerr << "Failed to open file: " << fileName << std::endl;
        return;
    }

    int vertexCount = vertices.size() / 3;
    int faceCount = vertexCount / 3;

    plyFile << "ply\n";
    plyFile << "format ascii 1.0\n";
    plyFile << "element vertex " << vertexCount << "\n";
    plyFile << "property float x\n";
    plyFile << "property float y\n";
    plyFile << "property float z\n";
    plyFile << "property float nx\n";
    plyFile << "property float ny\n";
    plyFile << "property float nz\n";
    plyFile << "element face " << faceCount << "\n";
    plyFile << "property list uchar int vertex_indices\n";
    plyFile << "end_header\n";

    for (size_t i = 0; i < vertices.size(); i += 3) {
        plyFile << vertices[i] << " " << vertices[i + 1] << " " << vertices[i + 2] << " "
                << normals[i] << " " << normals[i + 1] << " " << normals[i + 2] << "\n";
    }

    for (int i = 0; i < faceCount; ++i) {
        plyFile << "3 " << i * 3 << " " << i * 3 + 1 << " " << i * 3 + 2 << "\n";
    }

    plyFile.close();
}


int main(int argc, char *argv[]){
    std::vector<float> normals;
    float step = 0.05;
    float min = -5.0f;
    float max = 5.0f;
    float isoval = -1.5;
    std::string filename = "TriangleMesh.ply";
    bool generateFile = true;

    // Initialize window
    if (!glfwInit())
    {
        printf("Failed to initialize GLFW\n");
        return -1;
    }
    glfwWindowHint(GLFW_SAMPLES, 4);
    window = glfwCreateWindow(1200, 1200, "Marching Cubes", NULL, NULL);
    if (window == NULL)
    {
        printf("Failed to open window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true;
    if (glewInit() != GLEW_OK)
    {
        printf("Failed to initialize GLEW\n");
        glfwTerminate();
        return -1;
    }

    glClearColor(0.2, 0.2, 0.3, 0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Setup shaders
    glm::mat4 mvp; // model view projection matrix
    glm::vec3 eye(5, 5, 5); // Begin with the camera being positioned at (5,5,5) in world space
    glm::vec3 center(0, 0, 0); // looking at (0,0,0) in world space
    glm::vec3 up(0, 1, 0);
    glm::mat4 view = glm::lookAt(eye, center, up);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.001f, 1000.0f);
    glm::mat4 model = glm::mat4(1.0f);
    mvp = projection * view * model;

    MarchingCubes cubes(function1, isoval, min, max, step);
    Cube drawCube(min, max);

    // Set up VAO and VBO
    GLuint vao, vertex_VBO, normal_VBO, program_ID; 
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vertex_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_VBO);
    glBufferData(GL_ARRAY_BUFFER, 0, &cubes.getVertices()[0], GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        0,
        (void *)0);
    glGenBuffers(1, &normal_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, normal_VBO);
    glBufferData(GL_ARRAY_BUFFER, 0, &normals, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        0,
        (void *)0);
    glBindVertexArray(0);

    GLuint vertex_shader_ID = glCreateShader(GL_VERTEX_SHADER); // create and compile shaders
    GLuint fragment_shader_ID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(vertex_shader_ID, 1, &vertex_shader, NULL);
    glCompileShader(vertex_shader_ID);
    glShaderSource(fragment_shader_ID, 1, &fragment_shader, NULL);
    glCompileShader(fragment_shader_ID);
    program_ID = glCreateProgram();
    glAttachShader(program_ID, vertex_shader_ID);
    glAttachShader(program_ID, fragment_shader_ID);
    glLinkProgram(program_ID);
    glDetachShader(program_ID, vertex_shader_ID);
    glDetachShader(program_ID, fragment_shader_ID);
    glDeleteShader(vertex_shader_ID);
    glDeleteShader(fragment_shader_ID);
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window);

    float r = 30.0f; // camera controls
    float theta = 45.0f;
    float phi = 45.0f;
    GLfloat MODEL_COLOR[4] = {0.0f, 1.0f, 1.0f, 1.0f}; // model color
    GLfloat LIGHT_DIRECTION[3] = {5.0f, 5.0f, 5.0f};   // light direction
    double prevTime = glfwGetTime();
    bool wroteFile = false;

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        double currentTime = glfwGetTime(); // get time
        float deltaTime = currentTime - prevTime;
        prevTime = currentTime;

        // camera and MVP setup code
        cameraControlsGlobe(window, view, r, theta, phi);
        mvp = projection * view * model;

        if (!cubes.finished)
        { // if not finished, generate mesh
            cubes.generate();
            std::vector<float> vertices = cubes.getVertices();
            normals = cubes.computeNormals(vertices);

            glBindVertexArray(vao); // update buffers
            glBindBuffer(GL_ARRAY_BUFFER, normal_VBO);
            glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(GL_FLOAT), &normals[0], GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, vertex_VBO);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GL_FLOAT), &vertices[0], GL_DYNAMIC_DRAW);
            glBindVertexArray(0);
        }
        else if (!wroteFile && generateFile)
        { // if finished, write file
            std::vector<float> vertices = cubes.getVertices();
            normals = cubes.computeNormals(vertices);
            writePLY(vertices, normals, "meshes.ply");
            wroteFile = true;
        }

        glMatrixMode(GL_PROJECTION); // draw cube
        glPushMatrix();
        glLoadMatrixf(glm::value_ptr(projection));
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadMatrixf(glm::value_ptr(view));
        drawCube.draw();

        glUseProgram(program_ID); // draw mesh

        GLuint matrixID = glGetUniformLocation(program_ID, "MVP"); // get uniform locations
        glUniformMatrix4fv(matrixID, 1, GL_FALSE, &mvp[0][0]);

        GLuint viewID = glGetUniformLocation(program_ID, "V");
        glUniformMatrix4fv(viewID, 1, GL_FALSE, &view[0][0]);

        GLuint colorID = glGetUniformLocation(program_ID, "modelColor"); // set uniform values
        glUniform4fv(colorID, 1, MODEL_COLOR);

        GLuint lightDirID = glGetUniformLocation(program_ID, "lightDir");
        glUniform3fv(lightDirID, 1, LIGHT_DIRECTION);

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, normals.size());
        glBindVertexArray(0);
        glUseProgram(0);

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    return 0;
}