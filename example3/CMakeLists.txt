cmake_minimum_required(VERSION 3.10)

project(example2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(example2
    main.cpp)


find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLU REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(glm REQUIRED)

include_directories(${GLM_INCLUDE_DIRS})
link_directories(${GLM_LIBRARY_DIRS})
add_definitions(${GLM_DEFINITIONS})

target_link_libraries(example2
    PRIVATE
        OpenGL::OpenGL
        OpenGL::GLU
        GLUT::GLUT
        GLEW::GLEW
        glfw
        ${GLM_LIBRARIES}
)
