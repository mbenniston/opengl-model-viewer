cmake_minimum_required(VERSION 3.15)

add_library(glad)

target_sources(glad
PRIVATE
    glad.c
)

target_include_directories(glad
PUBLIC
    .
)
