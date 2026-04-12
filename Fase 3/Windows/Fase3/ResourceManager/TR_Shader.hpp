#pragma once
#include "../Shaders/shader.hpp"

// Clase de recruso Shader
struct TR_Shader: virtual public TResource{
    // Constructor por defecto
    TR_Shader(){};
    // Constructor con la ruta a un archivo de un vertex y de un fragment shader
    TR_Shader(const char* vertex, const char* fragment){
        // Recumeramos el constructor de la clase Shader 
        shader = _Shader{vertex, fragment};
    }
    
    // Cargamos igual que en el constructor, con ayuda del constructor de la clase Shader
    void load(const std::string& path, [[maybe_unused]] std::vector<_Texture>& texturesLoaded, [[maybe_unused]] RType rt){
        shader = _Shader(path.c_str(), path.c_str());
    };
    
    // El shader recuperado, almacenado aqui
    _Shader shader{};
};