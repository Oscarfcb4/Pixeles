cmake_minimum_required(VERSION 3.15)
project(PixelesEngine VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ${CMAKE_CURRENT_SOURCE_DIR} es desde donde está el archivo, es decir Engine/Core/

# Guardamos en EXTERNAL_DIR la ruta a las dependencias Extern, las librerias
set(EXTERNAL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../Extern)

# Vamos añadiendo nosotros a mano las cabeceras y librerias para enlazarlas
set(GLFW_INCLUDE_DIR ${EXTERNAL_DIR}/glfw/include)
set(GLFW_LIBRARY ${EXTERNAL_DIR}/glfw/lib/glfw3.lib)

set(ASSIMP_INCLUDE_DIR ${EXTERNAL_DIR}/assimp/include)
set(ASSIMP_LIBRARY ${EXTERNAL_DIR}/assimp/lib/assimp.lib)

set(GLM_INCLUDE_DIR ${EXTERNAL_DIR}/glm/include)
set(GLAD_INCLUDE_DIR ${EXTERNAL_DIR}/glad/include)

set(STB_INCLUDE_DIR ${EXTERNAL_DIR})

# Le decimos dónde está el source code, que están dentro de Src, archivo a archivo
set(ENGINE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/Pixeles.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/Shader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/SceneTree.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/ResourceManager.cpp
    ${EXTERNAL_DIR}/glad/src/glad.c
)

# Archivos de cabecera, importante si quieres que tu IDE los reconozca
set(ENGINE_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/Pixeles.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/PixelesTypes.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/Shader.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/ShadersDefault.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/SceneTree.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/E_AllEntities.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/E_Entity.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/E_Camera.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/E_Light.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/E_Model.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/E_Skybox.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/ResourceManager.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/R_Resource.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/R_Shader.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/R_Model.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/R_Mesh.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/R_AllResources.hpp
)

# Creamos la librería estática
add_library(PixelesEngine STATIC ${ENGINE_SOURCES} ${ENGINE_HEADERS})

# Directorios de inclusión, los de nuestra cabecera y las librerías
target_include_directories(PixelesEngine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Include          
        ${GLM_INCLUDE_DIR}
        ${GLAD_INCLUDE_DIR}
        ${STB_INCLUDE_DIR}    		
    PRIVATE
        ${GLFW_INCLUDE_DIR}
        ${ASSIMP_INCLUDE_DIR}
)

# Linkamos las librerías para que no las tenga que enlazar el usuario
target_link_libraries(PixelesEngine
    ${GLFW_LIBRARY}
    ${ASSIMP_LIBRARY}
    opengl32
)