#include <string>
#include <vector>
#include "TResourceManager.hpp"
#include "../Shaders/shader.hpp"  

// Llamada a la funcion de recoger recurso segun un modelo
TR_Model* TResourceManager::getModel(std::string name, std::string src, RType rt) {
    return getResource<TR_Model>(name, src, rt);
}

// Llamada a la funcion de recoger recurso segun un shader
TR_Shader* TResourceManager::getShader(std::string name, std::string vs, std::string fs) {
    return getResource<TR_Shader>(name, vs, fs);
}