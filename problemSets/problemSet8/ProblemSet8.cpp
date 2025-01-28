// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <cmath>

#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

#include <iostream>
#include <vector>

#define TOP_LEFT     8
#define TOP_RIGHT    4
#define BOTTOM_RIGHT 2
#define BOTTOM_LEFT  1

// function to draw grid
void GridDraw(float minx, float maxx, float miny, float maxy, float stepsize) {
	glLineWidth(1.0f); // Thinner lines for the grid
    glColor3f(0.5f, 0.5f, 0.5f); // Light grey color for the grid
	glBegin(GL_LINES);

	for (float x = minx; x <= maxx; x += stepsize) {
		glVertex2f(x, miny);
		glVertex2f(x, maxy);
	}

	for (float y = miny; y <= maxy; y += stepsize) {
		glVertex2f(minx, y);
		glVertex2f(maxx, y);
	}

	glEnd();
}

typedef float (*scalar_field_2d)(float, float);

int marching_squares_lut[16][4] = {
	{-1, -1, -1, -1},
	{2, 3, -1, -1},
	{1, 2, -1, -1},
	{1, 3, -1, -1},
	{0, 1, -1, -1},
	{0, 1, 2, 3},
	{0, 2, -1, -1},
	{0, 3, -1, -1},
	{0, 3, -1, -1},
	{0, 2, -1, -1},
	{0, 3, 1, 2},
	{0, 1, -1, -1},
	{1, 3, -1, -1},
	{1, 2, -1, -1},
	{2, 3, -1, -1},
	{-1, -1, -1, -1}
};

float g_verts[4][2] = {
	{0.5f, 1.0f},
	{1.0f, 0.5f},
	{0.5f, 0.0f},
	{0.0f, 0.5f}
};

float f1(float x, float y) {
	return x*x + y*y;
}

float f2(float x, float y) {
	return sin(x*y);
}

float f3(float x, float y) {
	return sin(x)*cos(y);
}

std::vector<float> marching_rectangles(scalar_field_2d f, float isoval, float minx, float maxx, float miny, float maxy, float stepsizeX, float stepsizeY) {

	std::vector<float> vertices;

	float x = minx;
	float y = miny;
	float tl, tr, br, bl;
	int which = 0;
	int* verts;
	for ( ; y < maxy; y += stepsizeY) {
		for (x = minx ; x < maxx; x += stepsizeX) {
			//test the square
			tl = (*f)(x, y+stepsizeY);
			tr = (*f)(x+stepsizeX, y+stepsizeY);
			br = (*f)(x+stepsizeX, y);
			bl = (*f)(x, y);

			which = 0;
			if (tl < isoval) {
				which |= TOP_LEFT;
			}
			if (tr < isoval) {
				which |= TOP_RIGHT;
			}
			if (br < isoval) {
				which |= BOTTOM_RIGHT;
			}
			if (bl < isoval) {
				which |= BOTTOM_LEFT;
			}

			verts = marching_squares_lut[which];
			if (verts[0] >= 0) {
				vertices.emplace_back(x+stepsizeX*g_verts[verts[0]][0]);
				vertices.emplace_back(y+stepsizeY*g_verts[verts[0]][1]);
				vertices.emplace_back(x+stepsizeX*g_verts[verts[1]][0]);
				vertices.emplace_back(y+stepsizeY*g_verts[verts[1]][1]);
			}
			if (verts[2] >= 0) {
				vertices.emplace_back(x+stepsizeX*g_verts[verts[2]][0]);
				vertices.emplace_back(y+stepsizeY*g_verts[verts[2]][1]);
				vertices.emplace_back(x+stepsizeX*g_verts[verts[3]][0]);
				vertices.emplace_back(y+stepsizeY*g_verts[verts[3]][1]);
			}
		}
	}

	return vertices;
}

//////////////////////////////////////////////////////////////////////////////
// Main
//////////////////////////////////////////////////////////////////////////////

int main( int argc, char* argv[])
{

	///////////////////////////////////////////////////////
	float screenW = 1400;
	float screenH = 900;
	float stepsize = 0.5f;

	// for the horizontal and vertical step sizes
	// when the value of stepsizeX equal to stepsizeY, we can generate marching squares
	float stepsizeX = 0.5f;
	float stepsizeY = 2.5f;


	float xmin = -5;
	float xmax = 5;
	float isoval = 1;

	if (argc > 1 ) {
		screenW = atoi(argv[1]);
	}
	if (argc > 2) {
		screenH = atoi(argv[2]);
	}
	if (argc > 3) {
		stepsize = atof(argv[3]);
	}
	if (argc > 4) {
		xmin = atof(argv[4]);
	}
	if (argc > 5) {
		xmax = atof(argv[5]);
	}
	if (argc > 6) {
		isoval = atof(argv[6]);
	}
	if (argc > 7) {
    stepsizeX = atof(argv[7]);
	}
	if (argc > 8) {
    stepsizeY = atof(argv[8]);
	}
	float ymin = xmin;
	float ymax = xmax;
	///////////////////////////////////////////////////////

	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	// glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	// glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	// glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( screenW, screenH, "Marching", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}


	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark blue background
	glClearColor(0.2f, 0.2f, 0.3f, 0.0f);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(xmin, xmax, ymin, ymax, -1, 1);

	std::vector<float> marchingVerts = marching_rectangles(f1, isoval, xmin, xmax, ymin, ymax, stepsizeX, stepsizeY);

	do{
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLineWidth(2.0f);
		glBegin(GL_LINES);
		// Marching Rectangles is set to white color 
		glColor3f(1.0f, 1.0f, 1.0f); 
			for (size_t i = 0; i < marchingVerts.size(); i += 4) {
				glVertex2f(marchingVerts[i], marchingVerts[i+1]);
				glVertex2f(marchingVerts[i+2], marchingVerts[i+3]);
			}
		glEnd();

		// draw grid
		GridDraw(xmin, xmax, ymin, ymax, stepsize);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();


	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Close OpenGL window and terminate GLFW
	glfwTerminate();
	return 0;
}

