cmake_minimum_required(VERSION 3.28)
project(Proyecto1_OMP C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopenmp")

# Configuración para SDL2
set(SDL2_INCLUDE_DIR "C:/Libs/SDL/include")
set(SDL2_LIB_DIR "C:/Libs/SDL/lib/x64")

include_directories(${SDL2_INCLUDE_DIR})
link_directories(${SDL2_LIB_DIR})

add_executable(Proyecto1_OMP main.c)

# Enlazar la biblioteca SDL2
target_link_libraries(Proyecto1_OMP SDL2)

# Indicar que es una aplicación de consola
set_target_properties(Proyecto1_OMP PROPERTIES WIN32_EXECUTABLE OFF)
