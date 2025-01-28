
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
#include <fstream>

struct Point
{
    float x,y;
    Point(float x1, float y1) : x(x1), y(y1) {}
};

int main() {

    float spin = 0.0f; // spin angle
    // read the file
    std::vector<Point> verticesOfDog;
    std::ifstream file("dog.txt");

    float x, y;
    while (file >> x >> y) {
        verticesOfDog.push_back(Point(x,y));
    }

    // std::cout << "Number of vertices read: " << verticesOfDog.size() << std::endl;
    // glfw init
    if (!glfwInit()) {
        return -1;
    }
    
    // create a window 1280px by 1000px with the title "Dog Animation"
    GLFWwindow* window = glfwCreateWindow(1280, 1000, "Spin of Dogs", NULL,NULL);

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
    glOrtho(0, 60, 0, 60, -1, 1); // viewing volume (left,right,bottom,top)

    // main loop keeps going until closing the window
    while (!glfwWindowShouldClose(window)) {

        // poll for and process events
        glfwPollEvents();

        // render here
        // set background color to white
        glClearColor(1.0f, 1.0f, 1.0f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw the dog polyline image at different angles
        // Draw the polyline for the dog at different rotation angles
        glColor3f(0.0f, 0.0f, 0.0f);
        for (int angle = 0; angle < 360; angle += 45) {
            glPushMatrix(); // Save the current matrix

        // Translate and rotate around the circle of radius 25
            glTranslatef(30, 30, 0);
            glRotatef(angle, 0, 0, 1);
            glTranslatef(25, 0, 0);

            // Apply the spinning effect by rotating each dog image around its own center
            glRotatef(spin, 0, 0, 1);

            glBegin(GL_LINE_STRIP); // Begin drawing the line strip
            for (size_t i = 0; i < verticesOfDog.size(); ++i) {
                glVertex2f(verticesOfDog[i].x, verticesOfDog[i].y);
            }
            glEnd(); // End drawing

            glPopMatrix(); // Restore the matrix
        }

        // Increase the spin angle by 1 degree for the next frame
        spin += 1.0f;
        if(spin >= 360.0f) spin -= 360.0f; // Keep the angle within bounds [0,360)

        // swap front and back buffers
        // consider each frame as one iteration of the loop and end with glfwSwapBuffers(window)
        glfwSwapBuffers(window);

    }
    

    // clean up
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
