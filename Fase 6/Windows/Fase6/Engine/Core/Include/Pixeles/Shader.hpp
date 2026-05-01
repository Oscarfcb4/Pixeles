#pragma once
#include <GLM/glm.hpp>
#include <GLAD/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
// Forward declaration
struct Pixeles;

struct Shader{
	// ID del programa
	unsigned int ID;

	// Constructor por defecto
	Shader() {};

	// Constructor con el path del VertexShader y el FragmentShader
	Shader(const char* vertexPath, const char* fragmentPath, bool isFile = true);

	void loadShaderFromFile(const char* vertexPath, const char* fragmentPath);
	void loadShaderFromString(const char* vShaderCode, const char* fShaderCode);

	// Activa o desactiva el shader
	void use();

	// Para declarar uniforms, un poco más completo que en la Fase2
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setVec2(const std::string& name, const glm::vec2& value) const;
	void setVec2(const std::string& name, float x, float y) const;
	void setVec3(const std::string& name, const glm::vec3& value) const;
	void setVec3(const std::string& name, float x, float y, float z) const;
	void setVec4(const std::string& name, const glm::vec4& value) const;
	void setVec4(const std::string& name, float x, float y, float z, float w) const;
	void setMat2(const std::string& name, const glm::mat2& mat) const;
	void setMat3(const std::string& name, const glm::mat3& mat) const;
	void setMat4(const std::string& name, const glm::mat4& mat) const;
};