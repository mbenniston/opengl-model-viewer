cmake_minimum_required(VERSION 3.15)

add_library(glm INTERFACE)
target_include_directories(glm 
INTERFACE 
    .
    ../../modules/glm    
)