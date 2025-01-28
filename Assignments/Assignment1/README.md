# GL Initialization

This program represents my first attempt with OpenGL. In this program, I create a window and draw a triangle on that window using the GLFW library.

## Source Code Explanation

1. I first initialize GLFW, then create a 1280px by 1000px window with the title "Hello World" and check to see if the window is created successfully.
2. I make the window's context current for the calling thread.
3. I create the main loop to continuously draw a triangle until the user closes the window.
   3.1 I set up the background color.
   3.2 For the triangle, I use immediate mode and set the color and coordinates.
4. I terminate GLFW.

## Screenshot

The screenshot image depicts the window titled "Hello World" with the triangle drawn on it.
