cmake_minimum_required(VERSION 3.25)
set(PROJECT_NAME FakePlayer)
project(${PROJECT_NAME})


set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set (TPP_DIR ${CMAKE_BINARY_DIR}/external)
set (TPP_FREE_GLUT ${TPP_DIR}/freeglut)


include(ExternalProject)
ExternalProject_Add(freeglut
  GIT_REPOSITORY  https://github.com/FreeGLUTProject/freeglut
  GIT_PROGRESS  ON
  PREFIX  ${TPP_FREE_GLUT}
  # CMAKE_INSTALL_PREFIX
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${TPP_FREE_GLUT} -DFREEGLUT_BUILD_STATIC_LIBS=ON
)

include_directories(3pp/lodepng)

set (SOURCES
  src/main.cc
  3pp/lodepng/lodepng.cc
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC ${TPP_FREE_GLUT}/include )
target_link_directories(${PROJECT_NAME} PUBLIC ${TPP_FREE_GLUT}/lib)
target_link_libraries(${PROJECT_NAME} PUBLIC freeglut_staticd)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)


# Copy the FreeGLUT DLLs to the executable's directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
      ${TPP_FREE_GLUT}/bin
      $<TARGET_FILE_DIR:${PROJECT_NAME}>
)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/res
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
)
