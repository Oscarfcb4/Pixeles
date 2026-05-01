#pragma once
#include <vector>
#include <string>

// Los tipos de recursos que acepta el gestor de recursos, asi incluyendo esto se incluyen todos 
// de golpe y permite encapsular los includes y asi nunca te dejas uno sin incluir. Además 
// facilita la escalabilidad con nuevos recursos

#include <Pixeles/PixelesTypes.hpp>
#include <Pixeles/R_Resource.hpp>
#include <Pixeles/R_Shader.hpp>
#include <Pixeles/R_Model.hpp>