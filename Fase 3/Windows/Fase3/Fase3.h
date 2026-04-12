#pragma once

#include <glad/glad.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <iostream>
#include <shader.hpp>

constexpr unsigned int ANCHO{ 1280 };
constexpr unsigned int ALTO{ 720 };
constexpr const char* NOMBRE = "Una escena cubista";

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

// Guardamos las posiciones de los vertices para crear una caja
float vertices[] = {
	// Cara delantera (z = 0.5)
	-0.5f,  0.5f,  0.5f,  // Vértice 3 (Esquina superior izquierda)
	 0.5f,  0.5f,  0.5f,  // Vértice 2 (Esquina superior derecha)
	-0.5f, -0.5f,  0.5f,  // Vértice 0 (Esquina inferior izquierda)
	 0.5f, -0.5f,  0.5f,  // Vértice 1 (Esquina inferior derecha)

	 // Cara trasera (Eje Z = -0.5)
	 -0.5f,  0.5f, -0.5f,  // Vértice 7 
	  0.5f,  0.5f, -0.5f,  // Vértice 6 
	 -0.5f, -0.5f, -0.5f,  // Vértice 4 
	  0.5f, -0.5f, -0.5f   // Vértice 5 
};

// El orden de las caras en el cubo
unsigned int indices[] = {
	// Cara delantera
	3, 2, 0,
	3, 0, 1,

	// Cara trasera
	7, 6, 4,
	7, 4, 5,

	// Cara izquierda
	7, 5, 1,
	7, 1, 3,

	// Cara derecha
	6, 4, 0,
	6, 0, 2,

	// Cara inferior
	7, 7, 2,
	7, 2, 6,

	// Cara superior
	5, 4, 0,
	5, 0, 1
};

// Guardamos las posiciones de todas las cajas que vamos a iterar
glm::vec3 cubePositions[] = {
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(2.0f, 5.0f, -15.0f),
	glm::vec3(-1.5f, -2.2f, -2.5f),
	glm::vec3(-3.8f, -2.0f, -12.3f),
	glm::vec3(2.4f, -0.4f, -3.5f),
	glm::vec3(-1.7f, 3.0f, -7.5f),
	glm::vec3(1.3f, -2.0f, -2.5f),
	glm::vec3(1.5f, 2.0f, -2.5f),
	glm::vec3(1.5f, 0.2f, -1.5f),
	glm::vec3(-1.3f, 1.0f, -1.5f)
};

// Guardamos los colores de cada cubo
Color cubeColors[] = {
	Color(1.0f, 0.0f, 0.0f, 1.0f), // Rojo
	Color(0.0f, 1.0f, 0.0f, 1.0f), // Verde
	Color(0.0f, 0.0f, 1.0f, 1.0f), // Azul
	Color(1.0f, 1.0f, 0.0f, 1.0f), // Amarillo
	Color(0.0f, 1.0f, 1.0f, 1.0f), // Cian
	Color(1.0f, 0.0f, 1.0f, 1.0f), // Magenta
	Color(1.0f, 1.0f, 1.0f, 1.0f), // Blanco
	Color(0.0f, 0.0f, 0.0f, 1.0f), // Negro
	Color(0.5f, 0.5f, 0.5f, 1.0f), // Gris
	Color(1.0f, 0.5f, 0.0f, 1.0f)  // Naranja
};