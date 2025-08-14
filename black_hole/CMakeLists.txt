cmake_minimum_required(VERSION 3.15)
project(black_hole_simulation)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

set(SOURCES
    main.cpp
    engine.cpp
    black_hole.cpp
    ray.cpp
)

set(HEADERS
    engine.h
    black_hole.h
    ray.h
)

add_executable(black_hole_sim ${SOURCES} ${HEADERS})

target_link_libraries(black_hole_sim PRIVATE GLEW::GLEW glfw glm::glm)
