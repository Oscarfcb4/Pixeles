#pragma once
#include "R_Mesh.hpp"

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
	void load(const std::string& path, RType rt) override{
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

	private:
		// Procesa el cubo
		R_Mesh processCube(){
			std::vector<Vertex> vertices{};
			std::vector<unsigned int> indices{};

			// La forma mas optima de hacerlo es metiendole yo los datos a mano, el cubo basico no tiene textura

			// Cara delantera (z = 0.5)
			vertices.push_back(Vertex{glm::vec3(-1.0f, 1.0f, 1.0f)}); // Vértice 0 (Esquina superior izquierda)
			vertices.push_back(Vertex{glm::vec3( 1.0f, 1.0f, 1.0f)}); // Vértice 1 (Esquina superior derecha)
			vertices.push_back(Vertex{glm::vec3(-1.0f,-1.0f, 1.0f)}); // Vértice 2 (Esquina inferior izquierda)
			vertices.push_back(Vertex{glm::vec3( 1.0f,-1.0f, 1.0f)}); // Vértice 3 (Esquina inferior derecha)

			// Cara trasera (Eje Z = -0.5)
			vertices.push_back(Vertex{glm::vec3(-1.0f, 1.0f,-1.0f)}); // Vértice 4 
			vertices.push_back(Vertex{glm::vec3( 1.0f, 1.0f,-1.0f)}); // Vértice 5 
			vertices.push_back(Vertex{glm::vec3(-1.0f,-1.0f,-1.0f)}); // Vértice 6 
			vertices.push_back(Vertex{glm::vec3( 1.0f,-1.0f,-1.0f)}); // Vértice 7 

			// Cara delantera
			indices.push_back(3);
			indices.push_back(2);
			indices.push_back(0);
			indices.push_back(3);
			indices.push_back(0);
			indices.push_back(1);

			// Cara trasera
			indices.push_back(7);
			indices.push_back(6);
			indices.push_back(4);
			indices.push_back(7);
			indices.push_back(4);
			indices.push_back(5);

			// Cara izquierda
			indices.push_back(7);
			indices.push_back(5);
			indices.push_back(1);
			indices.push_back(7);
			indices.push_back(1);
			indices.push_back(3);

			// Cara derecha
			indices.push_back(6);
			indices.push_back(4);
			indices.push_back(0);
			indices.push_back(6);
			indices.push_back(0);
			indices.push_back(2);

			// Cara inferior
			indices.push_back(7);
			indices.push_back(7);
			indices.push_back(2);
			indices.push_back(7);
			indices.push_back(2);
			indices.push_back(6);

			// Cara superior
			indices.push_back(5);
			indices.push_back(4);
			indices.push_back(0);
			indices.push_back(5);
			indices.push_back(0);
			indices.push_back(1);

			// Devuelve la mesh
			return R_Mesh(vertices, indices);
		}
		
		// Tipo del recurso
		RType type{};
		// Vector de mallas
		std::vector<R_Mesh> meshes{};
		// Puntero al Shader
		Shader* shader{};
};