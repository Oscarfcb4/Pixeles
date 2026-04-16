#pragma once
#include "TR_Mesh.hpp"

// Clase modelo, generica. En un futuro separare los distintos tipos de modelos en distintas clases especificas, pero de momento se hace todo aqui.

struct TR_Model: virtual public TResource{
	// Constructor por defecto
	TR_Model(){};

	// Metodo de dibujado, requiere de un shader que se le pasara desde TModel
	void drawWithShader(Shader& shader){
		// Para cada una de sus mallas, llama a su meto de deibujado con el mismo shader
		for(unsigned int i{}; i<meshes.size(); i++)
			meshes[i].draw(shader);
	};

	// Metodo de cargado, con la ruta del archivo, el tipo de archivo y una referencia a las texturas cargadas, mas util en el futuro para los billboards
	void load(const std::string& path, std::vector<Texture>& textLoaded, RType rt) override{
		// Asignamos el tipo de recurso
		type = rt;
		// Segun el tipo de recursos hace una cosa u otra
		if (rt == RType::TRCube) {
			// Metemos en una mesh el cubo
			meshes.push_back(processCube());
		}
	};

	// Seteamos el shader del modelo
	void setShader(Shader* sh){shader = sh;};

	// Recupera las mallas del modelo
	std::vector<TR_Mesh>& getMeshes(){return meshes;};

	// Recupera el valor de mezcla entre el color y la textura
	float getMixValue(){return mixValue;};

	private:

		// Procesa el cubo
		TR_Mesh processCube(){
			std::vector<Vertex> vertices{};
			std::vector<unsigned int> indices{};
			std::vector<Texture> textures{};

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
			return TR_Mesh(vertices, indices, textures);
		}
		
		// Tipo del recurso
		RType type{};
		// Vectores de meses y texturas
		std::vector<TR_Mesh> meshes{};
		std::vector<Texture>* texturesLoaded{};
		// La carpeta contenedora del modelo o textura
		std::string directory{};
		// Puntero al Shader
		Shader* shader{};
		// Correccion del valor gamma y el valor de mezcla
		bool gammaCorrection{};
		float mixValue{0.99f};
};