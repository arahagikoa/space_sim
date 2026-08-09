cmake_minimum_required(VERSION 3.21)
project(space_sim LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW  CONFIG REQUIRED)
find_package(glm   CONFIG REQUIRED)
find_package(Stb   REQUIRED)

set(PHYSICS_SOURCES
    src/physics/black_hole.cpp
    src/physics/black_hole.h)

add_executable(sim2d
    ${PHYSICS_SOURCES}
    src/sim2d/main.cpp
    src/sim2d/app2d.cpp
    src/sim2d/camera2d.cpp
    src/sim2d/spacetime_grid.cpp
    src/sim2d/lensing/ray.cpp)

add_executable(sim3d
    ${PHYSICS_SOURCES}
    src/sim3d/main.cpp
    src/sim3d/app3d.cpp
    src/sim3d/scene/camera.cpp
    src/sim3d/render/spacetime_grid.cpp
    src/sim3d/render/skybox.cpp
    src/sim3d/screen_quad.cpp)

target_include_directories(sim3d PRIVATE ${Stb_INCLUDE_DIR})

foreach(tgt sim2d sim3d)
    target_include_directories(${tgt} PRIVATE ${CMAKE_SOURCE_DIR}/src)
    target_link_libraries(${tgt} PRIVATE glfw GLEW::GLEW glm::glm)
    add_custom_command(TARGET ${tgt} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${tgt}>/assets)
endforeach()