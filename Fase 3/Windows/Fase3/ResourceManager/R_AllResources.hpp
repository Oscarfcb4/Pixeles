#pragma once

// Los tipos de recursos que acepta el gestor de recursos, asi incluyendo esto se incluyen todos 
// de golpe y permite encapsular los includes y asi nunca te dejas uno sin incluir. Además 
// facilita la escalabilidad con nuevos recursos

// Tipos actuales posibles de recursos
enum RType{
	TRShader, TRCube
};

#include "R_Resource.hpp"
#include "R_Shader.hpp"
#include "R_Model.hpp"