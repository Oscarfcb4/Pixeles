#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <cstdint>
#include <Shader.hpp>

constexpr unsigned int ANCHO{ 1280 };
constexpr unsigned int ALTO{ 720 };
constexpr const char* NOMBRE = "Hora de organizar";

// Definimos nuestro propio tipo color
typedef struct Color {
	float r, g, b, a;
	bool operator==(const Color& c) const {
		return (
			r == c.r &&
			g == c.g &&
			b == c.b &&
			a == c.a
			);
	}
} Color;