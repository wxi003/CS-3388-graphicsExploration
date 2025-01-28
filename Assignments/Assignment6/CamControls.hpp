// Create a global camera
#ifndef _CAM_CONTROLS_H_
#define _CAM_CONTROLS_H_

#include <glm/glm.hpp>
#include "math.h"

static const double _PI = 2.0*asin(1);



void cameraControlsGlobe(glm::mat4& V, float start) {

    glm::vec3 eye = {start, start/2, start};
    glm::vec3 targ = {0.0f, 0.0f, 0.0f};
    glm::vec3 up = {0.0f, 1.0f, 0.0f};
    static float radiusFromOrigin = glm::length(eye);

    static glm::vec3 position = eye;
    //exactly the angles to look at origin from (x,y,z) = (+v, +v/2, +v);
    static GLfloat theta = 0.0f;//0.25f*3.14159f;
    static GLfloat phi = 0.392f*3.14159f;

    static double mouseDownX;
    static double mouseDownY;
    static bool firstPress = true;
    static double lastTime = glfwGetTime();

    double dx = 0.0, dy = 0.0;
    int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    if (state == GLFW_PRESS)
    {
        if (firstPress) {
            glfwGetCursorPos(window, &mouseDownX, &mouseDownY);
        }
        firstPress = false;

        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        dx = xpos - mouseDownX;
        dy = ypos - mouseDownY;

        mouseDownX = xpos;
        mouseDownY = ypos;
    }
    if (state == GLFW_RELEASE) {
        firstPress = true;
    }

    //Avoid super jumpy motion
    // if (fabs(dx) < 10 && fabs(dy) < 10) {
        theta += 0.002f * dx;
        phi   += -0.002f * dy;
        if (theta > 2*_PI) {
            theta -= 2*_PI;
        }
        if (phi >= _PI) {
            phi = 0.9999999 * _PI;
        }
        if (phi <= 0) {
            phi = 0.0000001;//0.9999999 * _PI;
        }
    // }

    glm::vec3 direction(
        sin(phi) * cos(theta),
        cos(phi),
        sin(phi) * sin(theta)
    );
    glm::normalize(direction);

    float speed = 0.25f * radiusFromOrigin; //move faster further away
    double currentTime = glfwGetTime();
    float deltaTime = (currentTime - lastTime);
    lastTime = currentTime;

    // Move forward
    if (glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS) {
        radiusFromOrigin -= deltaTime * speed;
    }
    // Move backward
    if (glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS) {
        radiusFromOrigin += deltaTime * speed;
    }

    position = direction * radiusFromOrigin;
    V = glm::lookAt(position, targ, up);

}

#endif
