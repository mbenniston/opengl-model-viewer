cmake_minimum_required(VERSION 3.15)

project(OpenGL-Model-Viewer)

# Basic global project settings
include(cmake/StandardProjectSettings.cmake)

# Build dependencies
# Set options for glfw 
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(modules/glfw)

add_subdirectory(modules/glTF-reader)
add_subdirectory(modules/portable-file-dialogs)

# Project meta targets
add_library(project_options INTERFACE)
add_library(project_warnings INTERFACE)

# Create Compiler warnings interface
include(cmake/CompilerWarnings.cmake)
set_project_warnings(project_warnings)

# Create Project options interface
include(cmake/Sanitizers.cmake)
enable_sanitizers(project_options)

# Add targets
add_subdirectory(dependencies)
add_subdirectory(model-viewer)