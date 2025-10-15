# CMake minimum version required
cmake_minimum_required(VERSION 3.12)

# Define the project
project(OpenGLTemplate VERSION 1.0)

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Find Dependencies ---
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

# --- Define Source Files ---
set(SOURCE_FILES
    src/main.cpp
    src/Application.cpp
    src/Window.cpp
    src/Renderer.cpp
    libs/glad/src/glad.c
)

# --- Create Executable ---
add_executable(OpenGLTemplate ${SOURCE_FILES})

# --- Link Libraries and Include Directories ---
target_include_directories(OpenGLTemplate PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/glad/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/glm
    ${OPENGL_INCLUDE_DIR}
    ${glfw3_INCLUDE_DIRS}
)

target_link_libraries(OpenGLTemplate PRIVATE
    ${OPENGL_LIBRARIES}
    glfw
)

# Print a message upon configuration
message(STATUS "Project configured. Build with 'cmake --build build'")
