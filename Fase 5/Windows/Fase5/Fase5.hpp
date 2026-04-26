#pragma once
#include <glad/glad.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <cstdint>
#include <Shader.hpp>

constexpr unsigned int ANCHO{ 1280 };
constexpr unsigned int ALTO{ 720 };
constexpr const char* NOMBRE = "Iluminando la escena";

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