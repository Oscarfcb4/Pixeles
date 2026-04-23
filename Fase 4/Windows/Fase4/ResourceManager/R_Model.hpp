#pragma once
#include "R_Mesh.hpp"
#include <iostream>

// Clase modelo, generica.

struct R_Model: virtual public R_Resource{
	// Constructor por defecto
	R_Model(){};

	// Metodo de dibujado, requiere de un shader que se le pasara desde E_Model
	void drawWithShader(Shader& shader){
		// Para cada una de sus mallas, llama a su método de dibujado con el mismo shader
		for(unsigned int i{}; i<meshes.size(); i++)
			meshes[i].draw(shader);
	};

	// Metodo de cargado, con la ruta del archivo y el tipo de archivo
	void load(const std::string& path, std::vector<Texture>& texturesLoaded, RType rt) override{
		// Asignamos el tipo de recurso
		type = rt;
		// Segun el tipo de recursos hace una cosa u otra
		if (rt == RType::RCube) {
			// Metemos en una mesh el cubo
			meshes.push_back(processCube());
		}
	};

	// Seteamos el shader del modelo
	void setShader(Shader* shader){this->shader = shader;};

	// Recupera las mallas del modelo
	std::vector<R_Mesh>& getMeshes(){return meshes;};

	// Recupera el valor de mezcla entre el color y la textura
	float getMixValue() { return mixValue; };

	// Anade una textura a todas las mallas del modelo
	void addTexture(const std::string& path, const std::string& type, std::vector<Texture>& texturesLoaded);

	private:

	private:
		// Procesa el cubo
		R_Mesh processCube(){
			std::vector<Vertex> vertices{};
			std::vector<unsigned int> indices{};
			std::vector<Texture> textures{};

			// Cara superior (y = 0.5)
			vertices.push_back(Vertex{ glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2(0.0f, 1.0f) });
			vertices.push_back(Vertex{ glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2(1.0f, 1.0f) });
			vertices.push_back(Vertex{ glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2(1.0f, 0.0f) });
			vertices.push_back(Vertex{ glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2(0.0f, 0.0f) });

			// Cara frontal (z = 0.5)
			vertices.push_back(Vertex{ glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2(0.0f, 1.0f) });
			vertices.push_back(Vertex{ glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2(1.0f, 1.0f) });
			vertices.push_back(Vertex{ glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2(1.0f, 0.0f) });
			vertices.push_back(Vertex{ glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2(0.0f, 0.0f) });

			// Cara derecha (x = 0.5)
			vertices.push_back(Vertex{ glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 1.0f) });
			vertices.push_back(Vertex{ glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 1.0f) });
			vertices.push_back(Vertex{ glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 0.0f) });
			vertices.push_back(Vertex{ glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 0.0f) });

			// Cara izquierda (x = -0.5)
			vertices.push_back(Vertex{ glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 1.0f) });
			vertices.push_back(Vertex{ glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 1.0f) });
			vertices.push_back(Vertex{ glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 0.0f) });
			vertices.push_back(Vertex{ glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 0.0f) });

			// Cara inferior (y = -0.5)
			vertices.push_back(Vertex{ glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2(0.0f, 1.0f) });
			vertices.push_back(Vertex{ glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2(1.0f, 1.0f) });
			vertices.push_back(Vertex{ glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2(1.0f, 0.0f) });
			vertices.push_back(Vertex{ glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2(0.0f, 0.0f) });

			// Cara trasera (z = -0.5)
			vertices.push_back(Vertex{ glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2(0.0f, 1.0f) });
			vertices.push_back(Vertex{ glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2(1.0f, 1.0f) });
			vertices.push_back(Vertex{ glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2(1.0f, 0.0f) });
			vertices.push_back(Vertex{ glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2(0.0f, 0.0f) });

			// Indices para cada cara (2 triangulos por cara)
			for (int i = 0; i < 6; i++) {
				int base = i * 4;
				indices.push_back(base + 0); indices.push_back(base + 1); indices.push_back(base + 2);
				indices.push_back(base + 0); indices.push_back(base + 2); indices.push_back(base + 3);
			}

			return R_Mesh(vertices, indices, textures);
		}
		
		// Tipo del recurso
		RType type{};
		// Vector de mallas
		std::vector<R_Mesh> meshes{};
		// Puntero al Shader
		Shader* shader{};
		// Correccion del valor gamma y el valor de mezcla
		bool gammaCorrection{};
		float mixValue{ 0.99f };
};