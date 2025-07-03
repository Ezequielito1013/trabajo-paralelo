cmake_minimum_required(VERSION 3.10)
project(trabajo_paralelo)

set(CMAKE_CXX_STANDARD 17)

# Busca todos los archivos .cpp en la carpeta actual
file(GLOB FUENTES "*.cpp")

add_executable(ProyectoParalelo ${FUENTES})

add_custom_target(run_external
    COMMAND cmd.exe /C start cmd /k "$<TARGET_FILE:ProyectoParalelo>"
    DEPENDS ProyectoParalelo
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Ejecutando en terminal externa"
)