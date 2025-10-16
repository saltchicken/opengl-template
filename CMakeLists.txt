# CMake minimum version required
cmake_minimum_required(VERSION 3.12)

# Define the project
project(OpenGLTemplate VERSION 1.0)

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Fetch Dependencies ---
include(FetchContent)
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.2
)
FetchContent_MakeAvailable(glm)

set_property(DIRECTORY ${glm_SOURCE_DIR} PROPERTY SYSTEM TRUE)


# --- Find Dependencies ---
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

# --- Define Source Files ---
set(SOURCE_FILES
    src/main.cpp
    src/Application.cpp
    src/Window.cpp
    src/Renderer.cpp
    src/Shader.cpp
    src/Input.cpp
    src/Time.cpp
    libs/glad/src/glad.c
)

# --- Create Executable ---
add_executable(OpenGLTemplate ${SOURCE_FILES})

# --- Link Libraries and Include Directories ---
target_include_directories(OpenGLTemplate PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/glad/include
    ${OPENGL_INCLUDE_DIR}
    ${glfw3_INCLUDE_DIRS}
)

target_link_libraries(OpenGLTemplate PRIVATE
    ${OPENGL_LIBRARIES}
    glfw
    glm
)

function(copy_directory_to_target_dir target directory)
    get_target_property(target_dir ${target} BINARY_DIR)
    add_custom_command(
        TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/${directory}"
            "${target_dir}/${directory}"
        COMMENT "Copying ${directory} to build directory"
    )
endfunction()

# Copy the shaders directory after building the executable
copy_directory_to_target_dir(OpenGLTemplate shaders)

# Print a message upon configuration
message(STATUS "Project configured. Build with 'cmake --build build'")
