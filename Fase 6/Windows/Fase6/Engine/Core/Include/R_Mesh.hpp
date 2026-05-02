#pragma once
#include <GLM/glm.hpp>
#include <Shader.hpp>

// Clase Vertex, que contiene la informacion de un punto 
// 3D necesario para el renderizado en OpenGL
struct Vertex{
	glm::vec3 position{};
	glm::vec3 normal{};
	glm::vec2 texCoord{};
};

// Clase textura, con lo necesario para el renderizado de en OpenGL
struct Texture {
    unsigned int id{};
    std::string type{};
    std::string path{};
};

// Clase malla
struct R_Mesh{
    // Constructor con un vector de Vertex, indices y texturas
	R_Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures){
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;
        // Cuando tenemos los datos los procesamos
    	setupMesh();
    };

    // Metodo de dibujado en OpenGL
	void draw(Shader& shader){
        /// Inicializamos el contador para las texturas
        uint8_t texturas = 0;
        // Busca texturas difusas y especulares en sus texturas
        for (const auto& tex : textures) {
            if (tex.type == "textureDiffuse") {
                // Activamos la textura por el número en el que vaya
                glActiveTexture(GL_TEXTURE0 + texturas);
                // Enlazamos la ID de cuando la cargamos
                glBindTexture(GL_TEXTURE_2D, tex.id);
                // La pasamos como uniform
                shader.setInt("material.diffuse", texturas);
                texturas++;
            }
            else if (tex.type == "textureSpecular") {
                glActiveTexture(GL_TEXTURE0 + texturas);
                glBindTexture(GL_TEXTURE_2D, tex.id);
                shader.setInt("material.specular", texturas);
                texturas++;
            }
        }
        // Activamos la textura inicial, la 0, por si no tenía textura
        glActiveTexture(GL_TEXTURE0);
        // Anidamos el Vertex Array Object
        glBindVertexArray(VAO);
        // Dibujamos los triangulos segun los indices
        glDrawElements(GL_TRIANGLES, static_cast<int>(indices.size()), GL_UNSIGNED_INT, 0);
        // Desanidamos el Vertex Aray Object
        glBindVertexArray(0);
    };

    // Si el usuario ha cambiado la mesh y quiere volver a setearlo todo internamente
    void changeMesh(){setupMesh();};

    // Los vectores con la información almacenada
	std::vector<Vertex> vertices{};
	std::vector<unsigned int> indices{};
    std::vector<Texture> textures{};

	private:
        // Seteamos los buffers necesarios para el renderizado de OpenGL
		void setupMesh(){
            // Generamos un Vertex Array Object
            glGenVertexArrays(1, &VAO);
            // Generamos un Vertex Buffer Object
            glGenBuffers(1, &VBO);
            // Para los indices, generamos tambien un Element Buffer Object
            glGenBuffers(1, &EBO);

            // Anidamos el VAO
            glBindVertexArray(VAO);
            // Anidamos tambien el VBO
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            // Rellenamos el VBO con los datos del vector de vertices
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

            // Tambien rellenamos el EBO con los indices
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

            // Indicamos el orden en el que va a recibir el VAO la informacion para que luego lo pase por el layout al shader
            // Positions
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

            // Normales, con un offset ya que no es la primera propiedad de
            // la clase Vertex, offsetof te lo calcula automáticamente
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

            // Textures, con un offset también
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));

            // Desenlazo el VBO
            glBindVertexArray(0);
        };

        // Guardamos los indices de los buffers que hemos creado segun OpenGL
		unsigned int VAO{}, VBO{}, EBO{};
};