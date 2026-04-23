#pragma once
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
        /// Inicializamos los contadores para las texturas difusas y especulares
        uint16_t diffuseNr{ 1 };
        uint16_t specularNr{ 1 };
        // Por cada una de las texturasm se la vamos pasando a OpenGL siguiendo un sistema donde sabemos que la 0 es la 
        // textura base del color por lo que el resto deben ser de 1 para arriba por lo que con cada textura que enviamos
        // vamos aumentando los contadores
        for (uint16_t i{}; i < textures.size(); i++) {
            // Activamos la textura con el contador
            glActiveTexture(GL_TEXTURE0 + i);
            // Segun el tipo, cogemos el numero del contador de difusas o el de especulares
            std::string number{};
            std::string name{ textures[i].type };
            if (name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if (name == "texture_specular")
                number = std::to_string(specularNr++);
            // Se lo mandamos al shader
            shader.setFloat(("material." + name + number).c_str(), static_cast<float>(i));
            // Y anidamos la textura
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }
        // Activamos la textura inicial
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

    // Los vectores con la informacion almacenada
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

            // Normales
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

            // Textures
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));

            // Desenlazo el VBO
            glBindVertexArray(0);
        };

        // Guardamos los indices de los buffers que hemos creado segun OpenGL
		unsigned int VAO{}, VBO{}, EBO{};
};