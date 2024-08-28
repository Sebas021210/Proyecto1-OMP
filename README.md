# Proyecto1_OMP - Screensaver con OpenMP y SDL2

## Descripción del Proyecto

Este proyecto es un salvapantallas escrito en C que utiliza la biblioteca SDL2 para renderizar gráficos en una ventana. El programa simula la animación de círculos que se mueven y rebotan dentro de la ventana, interactúan entre ellos, colisionan y crean nuevos círculos. La implementación aprovecha la paralelización con OpenMP para mejorar el rendimiento del cálculo y manejo de las físicas, en especial cuando se trata de la detección de colisiones.

### Características Principales

- **Movimiento y Rebote de Círculos**: Los círculos se mueven a través de la ventana y rebotan cuando tocan los bordes.
- **Colisiones entre Círculos**: Cuando dos círculos colisionan, cuentan el número de colisiones y, después de un cierto número de colisiones, se fusionan, crecen de tamaño, o crean nuevos círculos.
- **Paralelización con OpenMP**: Se usa OpenMP para paralelizar el cálculo de actualización de posiciones y la verificación de colisiones, mejorando así el rendimiento del programa en sistemas con múltiples núcleos.

### Estructura del Código

- **`main.c`**: Contiene la lógica principal del programa.
  - **Inicialización de Círculos**: Se inicializan todos los círculos con posiciones, direcciones y velocidades aleatorias.
  - **Bucle Principal**: Este bucle maneja la actualización de los círculos, la detección de colisiones y el renderizado de los círculos en la ventana.
  - **Funciones Auxiliares**:
    - `initCircle`: Inicializa un círculo con valores aleatorios.
    - `resetCircleSpeed`: Restablece la velocidad de un círculo.
    - `updateCircles`: Actualiza la posición de todos los círculos.
    - `checkCollisions`: Verifica las colisiones entre los círculos y maneja la lógica correspondiente.
    - `drawCircle`: Dibuja un círculo en la ventana.

### Rendimiento

El proyecto mide y registra datos de rendimiento como el FPS mínimo, máximo y promedio, así como los tiempos promedio de actualización, verificación de colisiones y renderizado.

---

## Uso del Proyecto

### Requisitos

- **SDL2**: Asegúrate de tener instalada la biblioteca SDL2 en tu sistema. El proyecto asume que SDL2 está disponible en la ruta especificada en el archivo `CMakeLists.txt`.

### Compilación con CMake

Este proyecto utiliza CMake para manejar el proceso de compilación. A continuación se describe el archivo `CMakeLists.txt` utilizado en este proyecto:

```cmake
cmake_minimum_required(VERSION 3.28)
project(Proyecto1_OMP C)

# Especifica el estándar de C
set(CMAKE_C_STANDARD 11)

# Añade las banderas de compilación para habilitar OpenMP
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopenmp")

# Configura el path para los módulos de CMake
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake_modules)

# Ruta a la instalación de SDL2
set(SDL2_PATH "C:/SDL2/SDL2/SDL2-2.30.6/x86_64-w64-mingw32")

# Busca la librería SDL2
find_package(SDL2 REQUIRED)

# Incluye los directorios de encabezados de SDL2
include_directories(${SDL2_INCLUDE_DIR})

# Define el ejecutable
add_executable(Proyecto1_OMP main.c)

# Enlaza la biblioteca SDL2 con el proyecto
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY})

```

### Explicación del Archivo CMakeLists.txt

- **cmake_minimum_required(VERSION 3.28)**: Establece la versión mínima de CMake que se requiere para construir el proyecto.

- **project(Proyecto1_OMP C)**: Define el nombre del proyecto como `Proyecto1_OMP` y especifica que es un proyecto en lenguaje C.

- **set(CMAKE_C_STANDARD 11)**: Especifica que el estándar de C a utilizar es el C11.

- **set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopenmp")**: Añade las banderas necesarias para habilitar OpenMP durante la compilación.

- **set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake_modules)**: Establece la ruta donde CMake buscará módulos adicionales, si es necesario.

- **set(SDL2_PATH "C:/SDL2/SDL2/SDL2-2.30.6/x86_64-w64-mingw32")**: Establece la ruta donde está instalada la biblioteca SDL2 en tu sistema. Este es un ajuste importante para que CMake pueda encontrar y usar SDL2.

- **find_package(SDL2 REQUIRED)**: Busca la biblioteca SDL2 en el sistema. Si no se encuentra, CMake arrojará un error.

- **include_directories(${SDL2_INCLUDE_DIR})**: Añade los directorios de encabezados de SDL2 al proyecto para que los archivos fuente puedan encontrar las cabeceras de SDL2.

- **add_executable(Proyecto1_OMP main.c)**: Define el ejecutable que se va a generar, en este caso `Proyecto1_OMP`, y especifica que debe construirse a partir de `main.c`.

- **target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY})**: Enlaza la biblioteca SDL2 con el ejecutable.

### Compilacion y Ejecucion

1. Crear un directorio de compilacion

```sh
mkdir build
cd build
```

2. Generar el proyecto con CMake

```sh
cmake ..
```

3. Compilar el proyecto

```sh
make
```

4. Ejecutar el programa
```sh
./Proyecto1_OMP
```

### Autores
- Manuel Rodas 
- Sebastian Solorzano
- Jose Santisteban
