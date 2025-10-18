# CMake minimum version required
cmake_minimum_required(VERSION 3.12)
# Define the project
project(OpenGLTemplate VERSION 1.0)
if(NOT CMAKE_BUILD_TYPE)
  # Set the default to "Release"
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
  # Print a message to the user
  message(STATUS "No build type specified, defaulting to ${CMAKE_BUILD_TYPE}.")
endif()
# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# --- Fetch Dependencies ---
set(FETCHCONTENT_BASE_DIR "${CMAKE_SOURCE_DIR}/.cache")
include(FetchContent)
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.2
)
FetchContent_Declare(
  tomplusplus
  GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
  GIT_TAG          v3.4.0
)
# Add sol2 dependency
FetchContent_Declare(
  sol2
  GIT_REPOSITORY https://github.com/ThePhD/sol2.git
  GIT_TAG        main
)
FetchContent_MakeAvailable(glm sol2)
set_property(DIRECTORY ${glm_SOURCE_DIR} PROPERTY SYSTEM TRUE)

# --- Find Dependencies ---
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
# Simplified Lua search to only find the standard Lua package.
find_package(Lua REQUIRED)
message(STATUS "Found Lua: ${LUA_LIBRARIES}")

# --- Define Source Files ---
file(GLOB_RECURSE SOURCE_FILES "src/*.cpp")
list(APPEND SOURCE_FILES
    libs/glad/src/glad.c
)
# --- Create Executable ---
add_executable(OpenGLTemplate ${SOURCE_FILES})

target_compile_definitions(OpenGLTemplate PRIVATE SOL_USE_STD_OPTIONAL)

# --- Link Libraries and Include Directories ---
target_include_directories(OpenGLTemplate PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/glad/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libs
    ${OPENGL_INCLUDE_DIR}
    ${glfw3_INCLUDE_DIRS}
    ${LUA_INCLUDE_DIR} # Add Lua include dir
    ${sol2_SOURCE_DIR}/include # sol2 include
)
target_link_libraries(OpenGLTemplate PRIVATE
    ${OPENGL_LIBRARIES}
    glfw
    tomlplusplus
    glm
    ${LUA_LIBRARIES} # Link Lua library
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
copy_directory_to_target_dir(OpenGLTemplate assets)
copy_directory_to_target_dir(OpenGLTemplate scripts)
add_custom_command(
    TARGET OpenGLTemplate POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/settings.toml"
        "$<TARGET_FILE_DIR:OpenGLTemplate>/settings.toml"
    COMMENT "Copying settings.toml to build directory"
)
# Print a message upon configuration
message(STATUS "Project configured. Build with 'cmake --build build'")
