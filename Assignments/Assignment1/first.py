from OpenGL.GL import *
import glfw

glfw.init()
window = #...
glfw.make_context_current(window)

while not glfw.window_should_close(window):
    glfw.poll_events()
    glClear(GL_COLOR_BUFFER_BIT)

	#...

    glfw.swap_buffers(window)

glfw.terminate()
