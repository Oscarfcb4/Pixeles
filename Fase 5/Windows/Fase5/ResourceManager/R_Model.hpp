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
		}else if (rt == RType::RModel) {
			this->texturesLoaded = &texturesLoaded;
			// Iniciamos el importador de Assimp
			Assimp::Importer importer{};
			// Con el lector de Assimp, leemos el modelo y recuperamos la escena
			const aiScene* scene = importer.ReadFile(path, aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_GenSmoothNormals
				| aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
			// Si no ha habido escena, esta vacia o Assimp nos marcaa un error, lo mostramos por consola
			if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
				std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
				return;
			}
			directory = path.substr(0, path.find_last_of('/'));
			// Procesamos la informacion de Assimp
			processNode(scene->mRootNode, scene);
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
		// Procesa el cubo
		R_Mesh processCube(){
			std::vector<Vertex> vertices{};
			std::vector<unsigned int> indices{};
			std::vector<Texture> textures{};

			// Cara superior(y = 0.5), en CCW (Contrario a las agujas del reloj)
			vertices.push_back(Vertex{ glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2(1.0f, 1.0f) });
			vertices.push_back(Vertex{ glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2(0.0f, 1.0f) });
			vertices.push_back(Vertex{ glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2(0.0f, 0.0f) });
			vertices.push_back(Vertex{ glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2(1.0f, 0.0f) });

			// Cara frontal (z = 0.5)
			vertices.push_back(Vertex{ glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2(0.0f, 1.0f) });
			vertices.push_back(Vertex{ glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2(1.0f, 0.0f) });
			vertices.push_back(Vertex{ glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2(0.0f, 0.0f) });
			vertices.push_back(Vertex{ glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2(1.0f, 1.0f) });
			
			// Cara izquierda (x = -0.5)
			vertices.push_back(Vertex{ glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 1.0f) });
			vertices.push_back(Vertex{ glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 0.0f) });
			vertices.push_back(Vertex{ glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 0.0f) });
			vertices.push_back(Vertex{ glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 1.0f) });
			
			// Cara inferior(y = -0.5)
			vertices.push_back(Vertex{ glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2(1.0f, 1.0f) });
			vertices.push_back(Vertex{ glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2(0.0f, 1.0f) });
			vertices.push_back(Vertex{ glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2(0.0f, 0.0f) });
			vertices.push_back(Vertex{ glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2(1.0f, 0.0f) });
			
			// Cara derecha (x = 0.5)
			vertices.push_back(Vertex{ glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 1.0f) });
			vertices.push_back(Vertex{ glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 0.0f) });
			vertices.push_back(Vertex{ glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 0.0f) });
			vertices.push_back(Vertex{ glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 1.0f) });
			
			// Cara trasera (z = -0.5)
			vertices.push_back(Vertex{ glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2(1.0f, 1.0f) });
			vertices.push_back(Vertex{ glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2(1.0f, 0.0f) });
			vertices.push_back(Vertex{ glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2(0.0f, 0.0f) });
			vertices.push_back(Vertex{ glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2(0.0f, 1.0f) });

			// Vamos cara a cara dibujando los triángulos en orden CCW
			indices.push_back(0);indices.push_back(1);indices.push_back(2);
			indices.push_back(0);indices.push_back(2);indices.push_back(3);

			indices.push_back(4);indices.push_back(5);indices.push_back(6);
			indices.push_back(4);indices.push_back(6);indices.push_back(7);
			
			indices.push_back(8);indices.push_back(9);indices.push_back(10);
			indices.push_back(8);indices.push_back(10);indices.push_back(11);
			
			indices.push_back(12);indices.push_back(13);indices.push_back(14);
			indices.push_back(12);indices.push_back(14);indices.push_back(15);
			
			indices.push_back(16);indices.push_back(17);indices.push_back(18);
			indices.push_back(16);indices.push_back(18);indices.push_back(19);
			
			indices.push_back(20);indices.push_back(21);indices.push_back(22);
			indices.push_back(20);indices.push_back(22);indices.push_back(23);

			return R_Mesh(vertices, indices, textures);
		}

		// Por cada una de las mallas del nodo del modelo actualiza el vector de mallas y recursivamente hace lo mismo con los hijos de la mismo
		void processNode(aiNode* node, const aiScene* scene) {
			for (unsigned int i{}; i < node->mNumMeshes; i++) {
				// Usamos solo un puntero a la malla para procesar dicha malla
				aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
				// Actualizamos el vector de mallas con la malla procesada
				meshes.push_back(processMesh(mesh, scene));
			}
			// Lo mismo para cada hijo de este nodo
			for (unsigned int i{}; i < node->mNumChildren; i++) {
				processNode(node->mChildren[i], scene);
			}
		}

		// Procesa la informacion de la malla y devuelve un tipo malla propio
		R_Mesh processMesh(aiMesh* mesh, const aiScene* scene) {
			// Nuestro tipo malla propio puede tener n vertices, n indices y n texturas, asi que creamos unos vectores para almacenarlos
			std::vector<Vertex> vertices{};
			std::vector<unsigned int> indices{};
			std::vector<Texture> textures{};
			// Por cada vertice, lo va procesando
			for (unsigned int i{}; i < mesh->mNumVertices; i++) {
				// Creamos el vertex
				Vertex vertex{};
				// Le anadimos la posicion del vertice
				vertex.position.x = mesh->mVertices[i].x;
				vertex.position.y = mesh->mVertices[i].y;
				vertex.position.z = mesh->mVertices[i].z;
				// SI tiene normales, las asigna
				if (mesh->HasNormals()) {
					vertex.normal.x = mesh->mNormals[i].x;
					vertex.normal.y = mesh->mNormals[i].y;
					vertex.normal.z = mesh->mNormals[i].z;
				}
				// Si tiene coordenadas de textura, los asigna
				if (mesh->mTextureCoords[0]) {
					vertex.texCoord.x = mesh->mTextureCoords[0][i].x;
					vertex.texCoord.y = mesh->mTextureCoords[0][i].y;
				}
				// Ahora anade al vector de vertices el vertex
				vertices.push_back(vertex);
			}
			// Por cada cara aprovechamos y le sacamos los indices
			for (unsigned int i{}; i < mesh->mNumFaces; i++) {
				// Recuperamos cada cara
				aiFace face{ mesh->mFaces[i] };
				// Por cada indice lo va metiendo en el vector de indices
				for (unsigned int j{}; j < face.mNumIndices; j++)
					indices.push_back(face.mIndices[j]);
			}
			// Recuperamos el material desde la escena con el indice que guarda la malla
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			// Hacemos un vector temporal con todos los mapas de difusion
			std::vector<Texture> diffuseMaps{ loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse") };
			// Insertamos al final de textures los mapas difusos
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			// Hacemos lo mismo con las texturas especulares
			std::vector<Texture> specularMaps{ loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular") };
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
			// Devolvemos un mesh propio con todo
			return R_Mesh(vertices, indices, textures);
		}

		// Carga las texturas de los materiales
		std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName) {
			// Como podria tener muchas texturas, hacemos un vector
			std::vector<Texture> textures{};
			if (mat->GetTextureCount(type) != 0) {
				// Como hay textura, el valor de mexcla sera 0 y no cuenta el color
				mixValue = 0.0f;
				// Por cada tectura, comprobamos si existe en las texturas cargadas, si no la creamos
				for (unsigned int i{}; i < mat->GetTextureCount(type); i++) {
					aiString str{};
					mat->GetTexture(type, i, &str);
					bool skip{};
					for (unsigned int j{}; j < texturesLoaded->size(); j++) {
						if (std::strcmp(texturesLoaded->at(j).path.data(), str.C_Str()) == 0) {
							textures.push_back(texturesLoaded->at(j));
							skip = true;
							break;
						}
					}
					// Si no lo ha encontrado, la crea
					if (!skip) {
						Texture texture{};
						texture.id = textureFromFile(str.C_Str(), this->directory);
						texture.type = typeName;
						texture.path = str.C_Str();
						textures.push_back(texture);
						// Actualizamos el vector de texturas cargadas
						texturesLoaded->push_back(texture);
					}
				}
			}
			return textures;
		}

		// Metodo para recuperar la textura segun un archivo y un directorio
		unsigned int textureFromFile(const char* path, const std::string& directory, [[maybe_unused]] bool gamma = false) {
			// Ajustamos el path. Esto es feo pero Assimp lo necesita asi asi que bueno
			std::string filename{ std::string(path) };
			filename = directory + '/' + filename;
			// Generamos la nueva textura
			unsigned int textureID{};
			glGenTextures(1, &textureID);
			// Cargamos con stbi la informacion de la imagen
			int width{}, height{}, nrComponents{};
			unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
			// Si ha recuperado datos, lo cargamos en la textura si no mostramos un error por terminal
			if (data) {
				// Creamos un formato base, segun el numero de canales de color le ponemos uno u otro
				GLenum format{};
				if (nrComponents == 1)
					// Si solo tiene un canal, solo "rojo" porque OpenGL lo llama asi
					format = GL_RED;
				else if (nrComponents == 3)
					// Si tiene tres, RGB
					format = GL_RGB;
				else if (nrComponents == 4)
					// Si tiene cuatro, RGB y un canal alpha
					format = GL_RGBA;
				// Anidamos la textura
				glBindTexture(GL_TEXTURE_2D, textureID);
				// Cargamos la imagen
				glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

				// Ponemos los parametros para los mimaps y las texturas
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				// Generamos los mipmaps
				glGenerateMipmap(GL_TEXTURE_2D);

				// Liberamos la informacion de los datos
				stbi_image_free(data);
			}
			else {
				std::cout << "_Texture failed to load in: " << path << " (A.K.A '" << directory << "')" << std::endl;
				stbi_image_free(data);
			}
			// Devolvemos la ID que le ha asignado OpenGL a la textura 
			return textureID;
		}

		// Tipo del recurso
		RType type{};
		// Vector de mallas
		std::vector<R_Mesh> meshes{};
		std::vector<Texture>* texturesLoaded{};
		// La carpeta contenedora del modelo o textura
		std::string directory{};
		// Puntero al Shader
		Shader* shader{};
		// Correccion del valor gamma y el valor de mezcla
		bool gammaCorrection{};
		float mixValue{ 1.0f };
};