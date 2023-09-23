cmake_minimum_required(VERSION 3.25)
set(PROJECT_NAME FakePlayer)
project(${PROJECT_NAME})

set (TPP_DIR ${CMAKE_BINARY_DIR}/external)
set (TPP_LIB_DIR ${CMAKE_SOURCE_DIR}/3pp_output)

add_subdirectory(3pp)
set (SOURCES
  src/main.cc
  src/display.cc
)

add_executable(${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PRIVATE ${TPP_LIB_DIR}/include)
target_link_directories(${PROJECT_NAME} PRIVATE ${TPP_LIB_DIR}/lib)
target_link_libraries(${PROJECT_NAME} PRIVATE GL glfw3 glut rt dl pthread)
target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
add_dependencies(${PROJECT_NAME} 3pp_freeglut 3pp_glfw)

# # Copy the FreeGLUT DLLs to the executable's directory
# add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#   COMMAND ${CMAKE_COMMAND} -E copy_directory
#       ${TPP_DIR}/freeglut/bin
#       $<TARGET_FILE_DIR:${PROJECT_NAME}>
# )
# add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_directory
#       ${TPP_DIR}/freeglut/res
#         $<TARGET_FILE_DIR:${PROJECT_NAME}>
# )
