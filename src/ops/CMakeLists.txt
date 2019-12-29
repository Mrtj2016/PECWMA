
CMAKE_MINIMUM_REQUIRED(VERSION 3.9)

PROJECT(ops)

aux_source_directory(. DIR_SRCS)

# Add a library with the above sources
add_library(${PROJECT_NAME} ${DIR_SRCS})
add_library(sub::ops ALIAS ${PROJECT_NAME})
