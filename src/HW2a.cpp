// Skeleton code for hw2a
// Based on example code from: Interactive Computer Graphics: A Top-Down Approach with Shader-Based OpenGL (6th Edition), by Ed Angel

#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <iostream>
#include <sstream>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#define DEBUG_ON 0  // repetitive of the debug flag in the shader loading code, included here for clarity only

// This file contains the code that reads the shaders from their files and compiles them
#include "core/Matrix.hpp"
#include "ShaderStuff.hpp"

//----------------------------------------------------------------------------

// initialize some basic structure types
struct FloatType2D {
    GLfloat x;
	GLfloat y;
};

struct ColorType3D {
    GLfloat r;
	GLfloat g;
	GLfloat b;
};

// General transformation matrix
GLmatrix M;
GLint gUniformMatrixLocation = -1;

// Input Globals
enum MouseMode {
	NO_MODE = 0,
	ROTATE_MODE,
	TRANSLATE_MODE
};
MouseMode gCurrentMode = NO_MODE;

GLdouble gPreviousMouseX = 0.0, gPreviousMouseY = 0.0;

// Window Globals
GLint window_width = 500;
GLint window_height = 500;

// Coordinate Conversion Globals
const double kMouseMovementScale = 0.004;

// Some different cursors
GLFWcursor *arrow_cursor = nullptr, *crosshair_cursor = nullptr, *move_cursor = nullptr;

const GLint NVERTICES = 9; // part of the hard-coded model


//----------------------------------------------------------------------------
// function that is called whenever an error occurs
static void
error_callback(int error, const char* description)
{
	// Write the error description to stderr
	std::cerr << "Error #" << std::to_string(error) << ": " << description << std::endl;
}

//----------------------------------------------------------------------------
// function that is called whenever a keyboard event occurs; defines how keyboard input will be handled
static void
key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action != GLFW_PRESS && action != GLFW_REPEAT)
		return;

	switch (key) {
		case GLFW_KEY_ESCAPE:
		case GLFW_KEY_Q:
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;
		}

		case GLFW_KEY_LEFT:
		{
			M.ScaleXBy(0.95f);
			break;
		}

		case GLFW_KEY_RIGHT:
		{
			M.ScaleXBy(1.05f);
			break;
		}

		case GLFW_KEY_UP:
		{
			M.ScaleYBy(1.05f);
			break;
		}

		case GLFW_KEY_DOWN:
		{
			M.ScaleYBy(0.95f);
			break;
		}

		case GLFW_KEY_R:
		{
			M.Reset();
			break;
		}

		default:
			break;
	}
}

//----------------------------------------------------------------------------
// function that is called whenever a mouse or trackpad button press event occurs
static void
mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    // Check which mouse button triggered the event, e.g. GLFW_MOUSE_BUTTON_LEFT, etc.
    // and what the button action was, e.g. GLFW_PRESS, GLFW_RELEASE, etc.
    // (Note that ordinary trackpad click = mouse left button)
    // Also check if any modifier keys were active at the time of the button press, e.g. GLFW_MOconst D_ALT, etc.
    // Take the appropriate action, which could (optionally) also include changing the cursor's appearance
	if (action != GLFW_PRESS || button != GLFW_MOUSE_BUTTON_LEFT) {
		glfwSetCursor(window, arrow_cursor);
		gCurrentMode = NO_MODE;
		return;
	}

	if (mods == GLFW_MOD_CONTROL) {
		gCurrentMode = TRANSLATE_MODE;
		glfwSetCursor(window, move_cursor);
	} else {
		gCurrentMode = ROTATE_MODE;
		glfwSetCursor(window, crosshair_cursor);
	}
}

//----------------------------------------------------------------------------
// function that is called whenever a cursor motion event occurs
static void
cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
    // Determine the direction of the mouse or cursor motion
    // update the current mouse or cursor location
    //  (necessary to quantify the amount and direction of cursor motion)
    // take the appropriate action
	const double scaledXPos = (window_width - xpos) * kMouseMovementScale;
	const double scaledYPos = ypos * kMouseMovementScale;

	double mouseXDelta = gPreviousMouseX - scaledXPos;
	double mouseYDelta = gPreviousMouseY - scaledYPos;
	if (gCurrentMode == ROTATE_MODE) {
		static constexpr double piFraction = (2 * std::numbers::pi);
		M.Rotate2DBy(static_cast<float>(piFraction) * static_cast<float>(mouseXDelta));
	} else if (gCurrentMode == TRANSLATE_MODE) {
		M.TranslateXBy(static_cast<float>(mouseXDelta));
		M.TranslateYBy(static_cast<float>(mouseYDelta));
	}

	gPreviousMouseX = scaledXPos;
	gPreviousMouseY = scaledYPos;
}

//----------------------------------------------------------------------------

void
init()
{
    ColorType3D colors[NVERTICES];
    FloatType2D vertices[NVERTICES];
    GLuint vao[1], buffer, program, location1, location2;
  
    // set up some hard-coded colors and geometry
    // this part can be customized to read in an object description from a file
    colors[0].r = 1;  colors[0].g = 1;  colors[0].b = 1;  // white
    colors[1].r = 1;  colors[1].g = 0;  colors[1].b = 0;  // red
    colors[2].r = 1;  colors[2].g = 0;  colors[2].b = 0;  // red
    colors[3].r = 1;  colors[3].g = 1;  colors[3].b = 1;  // white
    colors[4].r = 0;  colors[4].g = 0;  colors[4].b = 1;  // blue
    colors[5].r = 0;  colors[5].g = 0;  colors[5].b = 1;  // blue
    colors[6].r = 1;  colors[6].g = 1;  colors[6].b = 1;  // white
    colors[7].r = 0;  colors[7].g = 1;  colors[7].b = 1;  // cyan
    colors[8].r = 0;  colors[8].g = 1;  colors[8].b = 1;  // cyan
    
    vertices[0].x =  0;     vertices[0].y =  0.25; // center
    vertices[1].x =  0.25;  vertices[1].y =  0.5; // upper right
    vertices[2].x = -0.25;  vertices[2].y =  0.5; // upper left
    vertices[3].x =  0;     vertices[3].y =  0.25; // center (again)
    vertices[4].x =  0.25;  vertices[4].y = -0.5; // low-lower right
    vertices[5].x =  0.5;   vertices[5].y = -0.25; // mid-lower right
    vertices[6].x =  0;     vertices[6].y =  0.25; // center (again)
    vertices[7].x = -0.5;   vertices[7].y = -0.25; // low-lower left
    vertices[8].x = -0.25;  vertices[8].y = -0.5; // mid-lower left
    
    // Create and bind a vertex array object
    glGenVertexArrays(1, vao);
    glBindVertexArray(vao[0]);

    // Create and initialize a buffer object large enough to hold both vertex position and color data
    glGenBuffers(1, &buffer );
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(colors), vertices, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(colors), colors);
    
    // Define the names of the shader files
    std::stringstream vshader, fshader;
    vshader << SRC_DIR << "/vshader2a.glsl";
    fshader << SRC_DIR << "/fshader2a.glsl";
    
    // Load the shaders and use the resulting shader program
    program = InitShader( vshader.str().c_str(), fshader.str().c_str() );
    
    // Determine locations of the necessary attributes and matrices used in the vertex shader
    location1 = glGetAttribLocation( program, "vertex_position" );
    glEnableVertexAttribArray( location1 );
    glVertexAttribPointer( location1, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
    location2 = glGetAttribLocation( program, "vertex_color" );
    glEnableVertexAttribArray( location2 );
    glVertexAttribPointer( location2, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(vertices)) );
	gUniformMatrixLocation = glGetUniformLocation(program, "M");
    
    // Define static OpenGL state variables
    glClearColor( 1.0, 1.0, 1.0, 1.0 ); // white, opaque background
    
    // Define some GLFW cursors (in case you want to dynamically change the cursor's appearance)
    // If you want, you can add more cursors, or even define your own cursor appearance
	arrow_cursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
	crosshair_cursor = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
	move_cursor = glfwCreateStandardCursor(GLFW_RESIZE_ALL_CURSOR);

	// Initialize the transform to be an identity matrix
	M.Reset();
}

//----------------------------------------------------------------------------

int
main(int argc, char* argv[])
{
    // Define the error callback function
    glfwSetErrorCallback(error_callback);
    
    // Initialize GLFW (performs platform-specific initialization)
    if (!glfwInit())
		exit(EXIT_FAILURE);
    
    // Ask for OpenGL 3.2
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Use GLFW to open a window within which to display your graphics
	GLFWwindow* window = glfwCreateWindow(window_width, window_height, "HW2a", nullptr, nullptr);
	
    // Verify that the window was successfully created; if not, print error message and terminate
    if (!window)
	{
        printf("GLFW failed to create window; terminating\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
	}
    
	glfwMakeContextCurrent(window); // makes the newly-created context current
    
    // Load all OpenGL functions (needed if using Windows)
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		printf("gladLoadGLLoader failed; terminating\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
    
	glfwSwapInterval(1);  // tells the system to wait for the rendered frame to finish updating before swapping buffers; can help to avoid tearing

    // Define the keyboard callback function
    glfwSetKeyCallback(window, key_callback);
    // Define the mouse button callback function
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    // Define the mouse motion callback function
    glfwSetCursorPosCallback(window, cursor_pos_callback);

	// Create the shaders and perform other one-time initializations
	init();

	// event loop
    while (!glfwWindowShouldClose(window)) {
        // fill/re-fill the window with the background color
		glClear(GL_COLOR_BUFFER_BIT);
        
        // sanity check that your matrix contents are what you expect them to be
        if (DEBUG_ON)
			printf("M = [%f %f %f %f\n     %f %f %f %f\n     %f %f %f %f\n     %f %f %f %f]\n",M[0],M[4],M[8],M[12], M[1],M[5],M[9],M[13], M[2],M[6],M[10],M[14], M[3],M[7],M[11],M[15]);
        
        glUniformMatrix4fv(gUniformMatrixLocation, 1, GL_FALSE, M );   // send the updated model transformation matrix to the GPU
		glDrawArrays( GL_TRIANGLES, 0, NVERTICES );    // draw a triangle between the first vertex and each successive vertex pair in the hard-coded model
		glFlush();	// ensure that all OpenGL calls have executed before swapping buffers

        glfwSwapBuffers(window);  // swap buffers
        glfwWaitEvents(); // wait for a new event before re-drawing
	} // end graphics loop

	// Clean up
	glfwDestroyWindow(window);
	glfwTerminate();  // destroys any remaining objects, frees resources allocated by GLFW
	exit(EXIT_SUCCESS);
}
