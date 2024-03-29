# Set the minimum required version of cmake for this project
cmake_minimum_required (VERSION 3.12)

# Create a project called 'HW2a'
project(Assignment2A, C CXX)
set(CMAKE_CXX_STANDARD 20)

set(TARGET_NAME HW2a)

# Define in the C++ code what the variable "SRC_DIR" should be equal to the current_path/src
add_definitions(-DSRC_DIR="${CMAKE_CURRENT_SOURCE_DIR}/src")

# Find OpenGL, and set link library names and include paths
find_package(OpenGL REQUIRED)
set(OPENGL_LIBRARIES ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY})
set(OPENGL_INCLUDE_DIRS ${OPENGL_INCLUDE_DIR})
include_directories(${OPENGL_INCLUDE_DIRS})

# Also disable building some of the extra things GLFW has (examples, tests, docs)
set(GLFW_BUILD_EXAMPLES  OFF CACHE BOOL " " FORCE)
set(GLFW_BUILD_TESTS     OFF CACHE BOOL " " FORCE)
set(GLFW_BUILD_DOCS      OFF CACHE BOOL " " FORCE)

# Now actually run cmake on the CMakeLists.txt file found inside of the GLFW directory
add_subdirectory(ext/glfw)

# Make a list of all the source files
set(SOURCES
    src/HW2a.cpp
    ext/glad/src/glad.c
)

# Make a list of all the header files (optional-- only necessary to make them appear in IDE)
set(INCLUDES
    src/ShaderStuff.hpp
    src/core/Matrix.hpp
    src/core/Vector3D.hpp
    src/core/Point.hpp
)

# Make a list of all of the directories to look in when doing #include "whatever.h"
set(INCLUDE_DIRS
    ext/
    ext/glfw/include
    ext/glad/include
)

set(LIBS
    glfw
    ${OPENGL_LIBRARIES}
)

# Define what we are trying to produce here (an executable), as
# well as what items are needed to create it (the header and source files)
add_executable(${TARGET_NAME} ${SOURCES} ${INCLUDES})

# Tell cmake which directories to look in when you #include a file
# Equivalent to the "-I" option for g++
include_directories(${INCLUDE_DIRS})

# Tell cmake which libraries to link to
# Equivalent to the "-l" option for g++
target_link_libraries(${TARGET_NAME} PRIVATE ${LIBS})

# For Visual Studio only
if (MSVC)
    # Do a parallel compilation of this project
    target_compile_options(${PROJECT_NAME} PRIVATE "/MP")
    # Have this project be the default startup project (the one to build/run when hitting F5)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
endif()