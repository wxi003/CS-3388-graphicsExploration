
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>

int main() {

    // glfw init
    if (!glfwInit()) {
        return -1;
    }
    
    // create a window 1280px by 1000px with the title "Hello World"
    GLFWwindow* window = glfwCreateWindow(1280, 1000, "Hello World", NULL,NULL);

    //check to see if the window is created successfully
    if (!window) {
        glfwTerminate();
        return -1;
    }
    
    // make the window's context current for the calling thread
    glfwMakeContextCurrent(window);

    // main loop keeps going until closing the window
    while (!glfwWindowShouldClose(window)) {

        // poll for and process events
        glfwPollEvents();

        // render here
        // set background color
        glClearColor(0.0f, 0.0f, 0.0f,0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw a triangle
        glBegin(GL_TRIANGLES);

        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex2f(0,0.5);

        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex2f(0.5,-0.25);

        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex2f(-0.5,-0.25);


        glEnd();

        // swap front and back buffers
        glfwSwapBuffers(window);


    }
    

    // clean up
    glfwTerminate();
    return 0;
}
