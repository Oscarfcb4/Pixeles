#pragma once
#include <Shader.hpp>

// Clase de recruso Shader
struct R_Shader: virtual public R_Resource{
    // Constructor por defecto
    R_Shader(){};
    // Constructor con la ruta a un archivo de un vertex y de un fragment shader
    R_Shader(const char* vertex, const char* fragment){
        // Recumeramos el constructor de la clase Shader 
        shader = Shader{vertex, fragment};
    }
    
    // Cargamos igual que en el constructor, con ayuda del constructor de la clase Shader
    void load(const std::string& path, std::vector<Texture>& texturesLoaded, RType rt){
        shader = Shader(path.c_str(), path.c_str());
        // Siempre será RShader pero es sano y escalable mantenerlo
        type = rt;
    };
    
    // El shader recuperado, almacenado aqui
    Shader shader{};
    // Tipo del recurso
    RType type{};
};