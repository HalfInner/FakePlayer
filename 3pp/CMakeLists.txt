cmake_minimum_required(VERSION 3.25)
set(PROJECT_NAME 3pp_libs)
project(${PROJECT_NAME})

set (SOURCES
  lodepng/lodepng.cc
  picojpeg/picojpeg.c
  picojpeg/helper.cc
  picojpeg/helper.h #? Why
)

add_library(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC lodepng/ picojpeg)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)