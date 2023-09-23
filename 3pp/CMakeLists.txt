cmake_minimum_required(VERSION 3.25)
set(PROJECT_NAME 3pp_libs)
project(${PROJECT_NAME})


include(ExternalProject)
ExternalProject_Add(3pp_freeglut
  GIT_REPOSITORY  https://github.com/FreeGLUTProject/freeglut
  GIT_PROGRESS  ON
  PREFIX  ${TPP_DIR}/freeglut
  UPDATE_DISCONNECTED 1
  INSTALL_DIR ${TPP_LIB_DIR}
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${TPP_LIB_DIR} -DFREEGLUT_BUILD_STATIC_LIBS=ON
)

ExternalProject_Add(3pp_glfw
  GIT_REPOSITORY  https://github.com/glfw/glfw
  GIT_PROGRESS  ON
  PREFIX  ${TPP_DIR}/glfw
  UPDATE_DISCONNECTED 1
  INSTALL_DIR ${TPP_LIB_DIR}
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${TPP_LIB_DIR}
)

ExternalProject_Add(3pp_jpeg
  GIT_REPOSITORY  https://github.com/libjpeg-turbo/libjpeg-turbo
  GIT_PROGRESS  ON
  GIT_TAG main
  PREFIX  ${TPP_DIR}/jpeg
  UPDATE_DISCONNECTED 1
  INSTALL_DIR ${TPP_LIB_DIR}
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${TPP_LIB_DIR}
)


