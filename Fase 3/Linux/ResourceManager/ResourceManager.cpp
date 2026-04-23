#include <string>
#include <vector>
#include "ResourceManager.hpp"
#include "../Shaders/Shader.hpp"  

// Llamada a la funcion de recoger recurso segun un modelo
R_Model* ResourceManager::getModel(std::string name, std::string src, RType rt) {
    return getResource<R_Model>(name, src, rt);
}

// Llamada a la funcion de recoger recurso segun un shader
R_Shader* ResourceManager::getShader(std::string name, std::string vs, std::string fs) {
    return getResource<R_Shader>(name, vs, fs);
}