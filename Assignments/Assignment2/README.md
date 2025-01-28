# Assignment 2

In this program, I explored transformation and polyline drawings.


## Part 1
Exercise 1 reads vertex data from a file and uses it to draw a polyline that forms an image of a dog.

## Part 2
Exercise 2 contains an OpenGL program designed to generate fractal patterns. The current implementation aims to create a pattern similar to the Sierpiński carpet using a random algorithm based on choosing midpoints between randomly selected corners of a square.

### Build and Run Example
compile: g++ -o exercise_1 exercise_1.cpp -lGL -lglfw -lGLU
run: ./exercise_1 <N> <window_width> <window_height>