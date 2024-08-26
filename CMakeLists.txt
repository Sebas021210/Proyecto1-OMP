cmake_minimum_required(VERSION 3.28)
project(Proyecto1_OMP C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopenmp")
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake_modules)

set(SDL2_PATH "C:/SDL2/SDL2/SDL2-2.30.6/x86_64-w64-mingw32")

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})


# Configuración para SDL2
add_executable(Proyecto1_OMP main.c)

# Enlazar la biblioteca SDL2
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY})

