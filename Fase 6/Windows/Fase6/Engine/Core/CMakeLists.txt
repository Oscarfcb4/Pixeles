cmake_minimum_required(VERSION 3.15)
project(PixelesEngine VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ruta a las dependencias Extern (está al mismo nivel que Core, dentro de Engine)
set(EXTERNAL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../Extern)

# --- Directorios de inclusión de dependencias ---
set(GLFW_INCLUDE_DIR ${EXTERNAL_DIR}/glfw/include)
set(GLFW_LIBRARY ${EXTERNAL_DIR}/glfw/lib/glfw3.lib)

set(ASSIMP_INCLUDE_DIR ${EXTERNAL_DIR}/assimp/include)
set(ASSIMP_LIBRARY ${EXTERNAL_DIR}/assimp/lib/assimp.lib)

set(GLM_INCLUDE_DIR ${EXTERNAL_DIR}/glm/include)
set(GLAD_INCLUDE_DIR ${EXTERNAL_DIR}/glad/include)
# STB: stb_image.h está directamente en Extern/, no en subcarpeta include
set(STB_INCLUDE_DIR ${EXTERNAL_DIR})

# --- Archivos fuente del motor ---
set(ENGINE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/Pixeles.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/Shader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/SceneTree.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/ResourceManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/STBImpl.cpp
    ${EXTERNAL_DIR}/glad/src/glad.c
)

# Archivos de cabecera (opcional, para mostrarlos en IDE)
set(ENGINE_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/Pixeles/Pixeles.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/Pixeles/PixelesTypes.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/Pixeles/Shader.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/Pixeles/SceneTree.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/Pixeles/E_AllEntities.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/Pixeles/E_Entity.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/Pixeles/E_Camera.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/Pixeles/E_Light.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/Pixeles/E_Model.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/Pixeles/E_Skybox.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/Pixeles/ResourceManager.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/Pixeles/R_Resource.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/Pixeles/R_Shader.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/Pixeles/R_Model.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/Pixeles/R_Mesh.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/Pixeles/R_AllResources.hpp
)

# Crear librería estática
add_library(PixelesEngine STATIC ${ENGINE_SOURCES} ${ENGINE_HEADERS})

# Directorios de inclusión (importante: ahora apuntamos al directorio que contiene la carpeta Pixeles)
target_include_directories(PixelesEngine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Include          # Para que se pueda hacer #include <Pixeles/...>
        ${GLM_INCLUDE_DIR}
        ${GLAD_INCLUDE_DIR}
        ${STB_INCLUDE_DIR}                           # Para stb_image.h
    PRIVATE
        ${GLFW_INCLUDE_DIR}
        ${ASSIMP_INCLUDE_DIR}
)

# Enlazar librerías
target_link_libraries(PixelesEngine
    ${GLFW_LIBRARY}
    ${ASSIMP_LIBRARY}
    opengl32
)