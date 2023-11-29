# CSCI5607-Assignment2a
Programming Interactive 2D Graphics with OpenGL 

Work by Jacob Secunda

### Some Notes
- This project has been updated with the latest version of GLFW from Git as of November 2023.
- The program uses the existing CMake build system and can be used as normal
- The HW2a.cpp file has been modified to use more C++ constructs.
- A Matrix.hpp file has been introduced with a new Matrix class that I created to more easily manage transformations

### Controls
- The left, right, up, and down arrow keys scale the model accordingly
- The model is rotated about its centroid when no modifier key is pressed and the mouse is dragged across the window from left to right and vice-versa.
- The model is translated with the mouse when the Ctrl key is held while dragging the model in the window.
- Both the 'q' and 'Esc' keys quit the program
- The "r" key will reset the model to its default state

