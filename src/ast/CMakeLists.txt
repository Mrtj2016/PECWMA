
CMAKE_MINIMUM_REQUIRED(VERSION 3.9)

PROJECT(ast)

aux_source_directory(. DIR_SRCS)

# Add a library with the above sources
add_library(${PROJECT_NAME} ${DIR_SRCS})
add_library(sub::ast ALIAS ${PROJECT_NAME})
