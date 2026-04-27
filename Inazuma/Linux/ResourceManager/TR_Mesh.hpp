#pragma once
#include "../Shaders/shader.hpp"

#define MAX_BONE_INFLUENCE 4

// Clase Vertex, que contiene la informacion de un punto 3D necesario para el renderizado en OpenGL
struct Vertex{
	glm::vec3 position{};
	glm::vec3 normal{};
	glm::vec2 texCord{};

    int aBoneIDs[MAX_BONE_INFLUENCE]{};
    float aWeights[MAX_BONE_INFLUENCE]{};
};

// Clase textura, con lo necesario para el renderizado de en OpenGL
struct _Texture{
	unsigned int id{};
	std::string type{};
	std::string path{};
};

struct _BoneInfo{
    int id{};
    glm::mat4 offset{};
};

// Clase malla
struct TR_Mesh{
    // Constructor con un vector de Vertex, indices y texturas
	TR_Mesh(std::vector<Vertex> vertices_, std::vector<unsigned int> indices_, std::vector<_Texture> textures_){
    	vertices = vertices_;
    	indices = indices_;
    	textures = textures_;
        // Cuando tenemos los datos los procesamos
    	setupMesh();
    };
    
    TR_Mesh(std::vector<Vertex> vertices_, std::vector<unsigned int> indices_, std::vector<_Texture> textures_, _BoundingBox aabb_){
    	vertices = vertices_;
    	indices = indices_;
    	textures = textures_;
        aabb = aabb_;
        // Cuando tenemos los datos los procesamos
    	setupMesh();
    };

    // Metodo de dibujado en OpenGL
	void draw(_Shader& shader){
        // Por cada una de las texturasm se la vamos pasando a OpenGL siguiendo un sistema donde sabemos que la 0 es la 
        // textura base del color por lo que el resto deben ser de 1 para arriba por lo que con cada textura que enviamos
        // vamos aumentando los contadores
        for(uint16_t i{0}; i<textures.size(); i++){
            // Activamos la textura con el contador
            glActiveTexture(GL_TEXTURE0 + i);
            // Y anidamos la textura
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }
        unsigned int shadowTexture = static_cast<unsigned int>(textures.size());
        if(shadowTexture==0){
            shader.setFloat("material.diffuse", 0);
            shader.setFloat("material.specular", 0);
            shadowTexture = 1;
        }
        glActiveTexture(GL_TEXTURE0 + shadowTexture+1);
        glBindTexture(GL_TEXTURE_2D_ARRAY, 1);
        shader.setInt("shadowMap", shadowTexture+1);
        glActiveTexture(GL_TEXTURE0 + shadowTexture+2);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 2);
        shader.setInt("shadowCubes", shadowTexture+2);
        // Activamos la textura inicial
        glActiveTexture(GL_TEXTURE0);
        // Anidamos el Vertex Array Object
        glBindVertexArray(VAO);
        // Dibujamos los triangulos segun los indices
        glDrawElements(mode, static_cast<int>(indices.size()), GL_UNSIGNED_INT, 0);
        if(boundiboxesActived){
            glDisable(GL_CULL_FACE);
            setupBoundingBoxes();
            glDrawElements(GL_TRIANGLES, static_cast<int>(indicesAABB.size()), GL_UNSIGNED_INT, 0);
            glEnable(GL_CULL_FACE);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
        }
        // Desanidamos el Vertex Aray Object
        glBindVertexArray(0);
    };

    void drawShadows([[maybe_unused]] _Shader& shader){
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
	std::vector<unsigned int> indicesAABB{};
	std::vector<Vertex> vertexAABB{};
	std::vector<_Texture> textures{};
    bool boundiboxesActived{};
    int mode{GL_TRIANGLES};

    void setupBoundingBoxes(){
        glm::vec3 sizeAABB{};
        glm::vec3 semiSizeAABB{};

        sizeAABB.x = fabsf(aabb.max.x - aabb.min.x);
        sizeAABB.y = fabsf(aabb.max.y - aabb.min.y);
        sizeAABB.z = fabsf(aabb.max.z - aabb.min.z);
        semiSizeAABB.x = sizeAABB.x/2.0f;
        semiSizeAABB.y = sizeAABB.y/2.0f;
        semiSizeAABB.z = sizeAABB.z/2.0f;

        glm::vec3 centerAABB = {aabb.min.x + semiSizeAABB.x, aabb.min.y + semiSizeAABB.y, aabb.min.z + semiSizeAABB.z };

        // Sacamos las esquinas
        vertexAABB = {
            Vertex{glm::vec3{(centerAABB.x - semiSizeAABB.x), (centerAABB.y - semiSizeAABB.y), (centerAABB.z - semiSizeAABB.z)}, glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec2{1.0f, 1.0f}}, // Inferior cercana izquieda
            Vertex{glm::vec3{(centerAABB.x - semiSizeAABB.x), (centerAABB.y - semiSizeAABB.y), (centerAABB.z + semiSizeAABB.z)}, glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec2{1.0f, 1.0f}}, // Inferior lejana izquierda
            Vertex{glm::vec3{(centerAABB.x - semiSizeAABB.x), (centerAABB.y + semiSizeAABB.y), (centerAABB.z - semiSizeAABB.z)}, glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec2{1.0f, 1.0f}}, // Superior cercana izquierda 
            Vertex{glm::vec3{(centerAABB.x - semiSizeAABB.x), (centerAABB.y + semiSizeAABB.y), (centerAABB.z + semiSizeAABB.z)}, glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec2{1.0f, 1.0f}}, // Superior lejana izquierda 

            Vertex{glm::vec3{(centerAABB.x + semiSizeAABB.x), (centerAABB.y - semiSizeAABB.y), (centerAABB.z - semiSizeAABB.z)}, glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec2{1.0f, 1.0f}}, // Inferior cercana derecha
            Vertex{glm::vec3{(centerAABB.x + semiSizeAABB.x), (centerAABB.y - semiSizeAABB.y), (centerAABB.z + semiSizeAABB.z)}, glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec2{1.0f, 1.0f}}, // Inferior lejana derecha
            Vertex{glm::vec3{(centerAABB.x + semiSizeAABB.x), (centerAABB.y + semiSizeAABB.y), (centerAABB.z - semiSizeAABB.z)}, glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec2{1.0f, 1.0f}}, // Superior cercana derecha
            Vertex{glm::vec3{(centerAABB.x + semiSizeAABB.x), (centerAABB.y + semiSizeAABB.y), (centerAABB.z + semiSizeAABB.z)}, glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec2{1.0f, 1.0f}} // Superior lejana derecha
        };

        // 0 -> Inferior cercana izquieda
        // 1 -> Inferior lejana izquieda
        // 2 -> Superior cercana izquieda
        // 3 -> Superior lejana izquieda
        // 4 -> Inferior cercana derecha
        // 5 -> Inferior lejana derecha
        // 6 -> Superior cercana derecha
        // 7 -> Superior lejana derecha

        indicesAABB = {
            0, 1, 2, // Cara izquierda
            2, 3, 1,

            4, 5, 6, // Cara derecha
            6, 7, 5,

            2, 3, 7, // Cara superior
            2, 7, 6,

            0, 1, 5, // Cara inferior
            0, 4, 5,

            0, 2, 6, // Cara cercana
            0, 6, 4,

            1, 3, 7, // Cara alejada
            1, 5, 7
        };

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertexAABB.size() * sizeof(Vertex), &vertexAABB[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesAABB.size() * sizeof(unsigned int), &indicesAABB[0], GL_STATIC_DRAW);
    }

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
            // Position
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

            // Normal
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

            // _Texture
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCord));	

            glEnableVertexAttribArray(3);
            glVertexAttribIPointer(3, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, aBoneIDs));	

            glEnableVertexAttribArray(4);
            glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, aWeights));	

            // Desenlazo el VBO
            glBindVertexArray(0);
        };

        // Guardamos los indices de los buffers que hemos creado segun OpenGL
		unsigned int VAO{}, VBO{}, EBO{};
		_BoundingBox aabb{};
};