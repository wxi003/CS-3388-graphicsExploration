
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <fstream>

struct Point
{
    float x,y;
    Point() : x(0), y(0) {} // Default constructor
    Point(float x1, float y1) : x(x1), y(y1) {} // Constructor with parameters
};

// create random floats in the range [-1, 1]
static inline double frand() {
    double x = (double)rand() / (double)RAND_MAX;
    if (rand() % 2) {
        x *= -1.0;
    }
    return x;
}


int main(int argc, char** argv) {
    if (argc != 4) {
    std::cerr << "Usage: " << argv[0] << " <N> <width> <height>\n";
    return -1;
}
    // command line arguments
    int N = std::atoi(argv[1]); // N points
    int screenWidth = std::atoi(argv[2]); 
    int screenHeight = std::atoi(argv[3]);


    // std::cout << "Number of vertices read: " << verticesOfDog.size() << std::endl;
    // glfw init
    if (!glfwInit()) {
        return -1;
    }
    
    // create a window 1280px by 1000px with the title "Hello World"
    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Dog Plot", NULL,NULL);

    //check to see if the window is created successfully
    if (!window) {
        glfwTerminate();
        return -1;
    }
    
    // make the window's context current for the calling thread
    glfwMakeContextCurrent(window);


    // set up the viewport and projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.1,1.1,-1.1,1.1,-1,1); // viewing volume (left,right,bottom,top)

    // set background color to white
    glClearColor(1.0f, 1.0f, 1.0f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);


    // Define the corners of the square
    std::vector<Point> corners;
    corners.push_back(Point(-1.0f, -1.0f));
    corners.push_back(Point(-1.0f, 1.0f));
    corners.push_back(Point(1.0f, 1.0f));
    corners.push_back(Point(1.0f, -1.0f));

    
    // Choose a random corner as the initial point
    Point c0 = corners[rand() % 4];

    // Generate a random initial point p0 within the range [-1, 1]
    Point p0(frand(), frand());

    // draw in black
    glColor3f(0.0f, 0.0f, 0.0f); 

    // Set the point size
    glPointSize(10.0f); 

    glBegin(GL_POINTS);
    for (int i = 0; i < N; ++i) {
        // Choose a random corner that is not diagonally opposite to the current point
        Point ci;
        do {
            ci = corners[rand() % 4];
        } while ((ci.x == -c0.x) && (ci.y == -c0.y));

        // Let pi be the point halfway between p(i-1) and ci
        p0.x = (p0.x + ci.x) / 2.0;
        p0.y = (p0.y + ci.y) / 2.0;

        // Draw the point at pi
        glVertex2f(p0.x, p0.y);

        //update the corner
        c0 = ci;
    }
    glEnd();

    // Swap buffers and display the points
    glfwSwapBuffers(window);

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window)) {
        glfwWaitEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
