#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>

int screenWidth = 1000;
int screenHeight = 1000;

struct Point
{   
    float x,y;

    Point() : x(0), y(0) {} // Default constructor
    Point(float x1, float y1) : x(x1), y(y1) {} // Constructor with parameters
};


struct Node: Point
{
    bool hasHandle1, hasHandle2; 
    Point handle1; 
    Point handle2;
    Node(float x1, float y1) : Point(x1, y1), hasHandle1(false), hasHandle2(false) {}
};
std::vector<Node> nodes;

bool isDraggingNode = false;
bool isDraggingControlPoint = false;
Node* selectedNode = nullptr;
Point* selectedControlPoint = nullptr;

Point calculateBezierPoint(float t, Node p0, Point p1, Point p2, Node p3) {
    float u = 1 - t;
    float tt = t * t;
    float uu = u * u;
    float uuu = uu * u;
    float ttt = tt * t;
    Point p = Point((uuu * p0.x), (uuu * p0.y)); //first term
    p.x += 3 * uu * t * p1.x; //second term
    p.y += 3 * uu * t * p1.y;
    p.x += 3 * u * tt * p2.x; //third term
    p.y += 3 * u * tt * p2.y;
    p.x += ttt * p3.x; //fourth term
    p.y += ttt * p3.y;
    return p;
}

double distanceBetween(const Point& a, const Point& b) {
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

void renderBezierCurve() {
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(2.0f);
    glColor3f(0.0f, 0.0f, 0.0f); // black color for the spline

    if (nodes.size() > 1) { // Ensure there are enough nodes to form at least one segment
        for (size_t i = 0; i < nodes.size() - 1; ++i) {
            glBegin(GL_LINE_STRIP);
            for (float t = 0; t <= 1.0; t += 0.01) {
                Point p = calculateBezierPoint(t, nodes[i], nodes[i].handle2, nodes[i+1].handle1, nodes[i+1]);
                glVertex2f(p.x, p.y);
            }
            glEnd();
        }
    }
}

// callback function to handle mouse clicks for selecting and creating nodes or control points
void mouse_callback(GLFWwindow* window,int button, int action, int mods) {
    if (button != GLFW_MOUSE_BUTTON_LEFT) return;

    double xPos, yPos;
     
    glfwGetCursorPos(window, &xPos, &yPos);
    yPos = screenHeight - yPos - 100;
    
    if (action == GLFW_PRESS) {
        bool nodeSelected = false;
        for (size_t i = 0; i < nodes.size() && !nodeSelected; ++i) {
            Node& node = nodes[i];
            // Check if the click is on a node
            if (std::abs(node.x - xPos) < 10 && std::abs(node.y - yPos) < 10) {
                selectedNode = &node;
                isDraggingNode = true;
                nodeSelected = true;
            } else {
                // Check if the click is on a control point
                if (node.hasHandle1 && std::abs(node.handle1.x - xPos) < 10 && std::abs(node.handle1.y - yPos) < 10) {
                    selectedNode = &node;
                    selectedControlPoint = &node.handle1;
                    isDraggingControlPoint = true;
                    nodeSelected = true;

                } else if (node.hasHandle2 && std::abs(node.handle2.x - xPos) < 10 && std::abs(node.handle2.y - yPos) < 10) {
                    selectedNode = &node;
                    selectedControlPoint = &node.handle2;
                    isDraggingControlPoint = true;
                    nodeSelected = true;
                    break; // select only one item
                }
            }
        }

        // Add a new node if no node or control point is selected
        if (!nodeSelected) {
            Node newNode(xPos, yPos); // Create a new node at cursor position
            if (nodes.empty()) { // This means it's the first node
                newNode.hasHandle2 = true; // Assuming the first node will have an upper control point
                newNode.hasHandle1 = false;
                newNode.handle2 = Point(xPos, yPos + 50); // This control point is 50 pixels above
                // Add the new node
                nodes.push_back(newNode);
            }else{
                double distToFirst = distanceBetween(nodes.front(), newNode);
                double distToLast = distanceBetween(nodes.back(), newNode);
                // Handle intermediate nodes
                if (distToFirst < distToLast) {
                // The new node is closer to the start, so insert it at the beginning
                    newNode.hasHandle2 = nodes.front().hasHandle2; // Copy control point state
                    newNode.handle2 = Point(newNode.x, newNode.y + 50); // Set control point if necessary
                    nodes.front().hasHandle1 = true; // Ensure the now second node has a control point
                    nodes.front().handle1 = Point(nodes.front().x, nodes.front().y - 50); // Adjust its position
                    nodes.insert(nodes.begin(), newNode); // Insert at the beginning
                } else {
                    // The new node is closer to the end, so add it there
                    nodes.back().hasHandle2 = true; // Ensure the last node now has a second control point
                    nodes.back().handle2 = Point(nodes.back().x, nodes.back().y + 50); // Adjust its position 
                    newNode.hasHandle1 = true; // The new node becomes the new end node with a control point
                    newNode.handle1 = Point(newNode.x, newNode.y - 50); // Set this control point
                    nodes.push_back(newNode); // Add to the end
                }
            }
            
        }

    } else if (action == GLFW_RELEASE) {
        isDraggingNode = isDraggingControlPoint = false;
        selectedNode = nullptr;
        selectedControlPoint = nullptr;
    }
}


// callback function to handle the dragging of nodes or control points
void cursor_callback(GLFWwindow* window, double xpos, double ypos) {
  
    ypos = screenHeight - ypos;

    if (isDraggingNode && selectedNode != nullptr) {
        double dx = xpos - selectedNode->x;
        double dy = ypos - selectedNode->y;

        // Move the node
        selectedNode->x = xpos;
        selectedNode->y = ypos;

        // Move control points, maintaining the same offset
        if (selectedNode->hasHandle1) {
            selectedNode->handle1.x += dx;
            selectedNode->handle1.y += dy;
        }
        if (selectedNode->hasHandle2) {
            selectedNode->handle2.x += dx;
            selectedNode->handle2.y += dy;
        }

    } else if (selectedControlPoint != nullptr) {
        // Directly update the dragged control point's position
        selectedControlPoint->x = xpos;
        selectedControlPoint->y = ypos;

        // calculate the distance and direction for the other control point
        double dx1 = selectedNode->x - xpos;
        double dy1 = selectedNode->y - ypos;

        // to see if the dragged control point is handle1 or handle2
        Point* otherControlPoint = (selectedControlPoint == &selectedNode->handle1) ? &selectedNode->handle2 : &selectedNode->handle1;
        otherControlPoint->x = selectedNode->x + dx1;
        otherControlPoint->y = selectedNode->y + dy1;
    }
}

// callback function to handle the clearing of all nodes when the 'E' key is pressed
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_E && action == GLFW_PRESS) {
        nodes.clear(); // Clears all nodes
        isDraggingNode = isDraggingControlPoint = false;
        selectedNode = nullptr;
        selectedControlPoint = nullptr;
    }
}


int main(int argc, char** argv) {
    if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <width> <height>\n";
    return -1;
    }
    // command line arguments
    screenWidth = std::atoi(argv[1]); 
    screenHeight = std::atoi(argv[2]);

    // glfw init
    if (!glfwInit()) {
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4); // Enable 4x multisampling for anti-aliasing

    // create a window 1280px by 1000px with the title "Hello World"
    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "A Spline Tool", NULL,NULL);

    // check to see if the window is created successfully
    if (!window) {
        glfwTerminate();
        return -1;
    }
    
    // make the window's context current for the calling thread
    glfwMakeContextCurrent(window);

    /*
    Setup: 
    Configure the OpenGL projection matrix and viewport to match the window dimensions. 
    This involves setting up an orthographic projection that maps the coordinate space directly to the window size.
    */
    // set up viewport and projection
    glViewport(0, 0, screenWidth, screenHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, screenWidth, 0, screenHeight, -1, 1); 
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    
    // Enable blending for anti-aliasing
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // set callfunctions with GLFW
    glfwSetMouseButtonCallback(window, mouse_callback);
    glfwSetCursorPosCallback(window, cursor_callback);
    glfwSetKeyCallback(window, key_callback);


    /*
    Rendering
    */ 
    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // set background color to white
        glClearColor(1.0f, 1.0f, 1.0f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Render the Bezier spline
        renderBezierCurve(); 
    
        // Render nodes
        glColor3f(0.0f, 0.0f, 1.0f); // Set color to blue for nodes
        float size = 5.0f; // Half-size of the square
        for (size_t i = 0; i < nodes.size(); ++i) {
            glBegin(GL_QUADS);
            glVertex2f(nodes[i].x - size, nodes[i].y - size);
            glVertex2f(nodes[i].x + size, nodes[i].y - size);
            glVertex2f(nodes[i].x + size, nodes[i].y + size);
            glVertex2f(nodes[i].x - size, nodes[i].y + size);
            glEnd();
        }

        // render control points
        glEnable(GL_POINT_SMOOTH);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor3f(0.0f, 0.0f, 0.0f); // Set color to black for control points
        glPointSize(10.0f); // Set point size for control points
        glBegin(GL_POINTS);
        for (size_t i = 0; i < nodes.size(); ++i) {
            if (nodes[i].hasHandle1) {
                glVertex2f(nodes[i].handle1.x, nodes[i].handle1.y);
            }
            if (nodes[i].hasHandle2) {
                glVertex2f(nodes[i].handle2.x, nodes[i].handle2.y);
            }
        }
        glEnd();

        // connect control points with their nodes
        glEnable(GL_LINE_STIPPLE);
        glLineStipple(10, 0xAAAA); // Set line pattern as dotted line
        glColor3f(0.0f, 1.0f, 0.0f); // Green color for dotted lines
        for (size_t i = 0; i < nodes.size(); ++i) {
            if (nodes[i].hasHandle1) {
                glBegin(GL_LINES);
                glVertex2f(nodes[i].x, nodes[i].y);
                glVertex2f(nodes[i].handle1.x, nodes[i].handle1.y);
                glEnd();
            }
            if (nodes[i].hasHandle2) {
                glBegin(GL_LINES);
                glVertex2f(nodes[i].x, nodes[i].y);
                glVertex2f(nodes[i].handle2.x, nodes[i].handle2.y);
                glEnd();
            }
        }
        glDisable(GL_LINE_STIPPLE);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
