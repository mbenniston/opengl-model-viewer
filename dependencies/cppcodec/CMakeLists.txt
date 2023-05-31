cmake_minimum_required(VERSION 3.15)

add_library(cppcodec INTERFACE)
target_include_directories(cppcodec 
INTERFACE 
    ../../modules/cppcodec    
)