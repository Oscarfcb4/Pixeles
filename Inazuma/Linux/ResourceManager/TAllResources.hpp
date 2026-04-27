#pragma once

#include "./assimpConverter.hpp"

// Los tipos de recursos que acepta el gestor de recursos

enum RType{
	TRModel, TRShader, TRCube, TRImage, TRRectangle, TRAnim, TRRecWired
};

#include "TResource.hpp"
#include "TR_Shader.hpp"
#include "TR_Model.hpp"