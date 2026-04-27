#pragma once
#include <map>
#include "TR_Mesh.hpp"
#include "../Shaders/shader.hpp"
#include "../Animator/animator.hpp"

// Clase modelo, generica. En un futuro separare los distintos tipos de modelos en distintas clases especificas, pero de momento se hace todo aqui.

struct TR_Model: virtual public TResource{
	// Constructor por defecto
	TR_Model(){};

	// Metodo de dibujado, requiere de un shader que se le pasara desde TModel
	void drawWithShader(_Shader& shader){
		// Si es una imagen, le pasas al vertex shader. De momento es inutil pero en un futuro
		// sera crucial para el billboard
		if(type == RType::TRImage || type == RType::TRRectangle){
			glDisable(GL_CULL_FACE);
			shader.setBool("image", true);
		}else{
			shader.setBool("image", false);
		}
		// Para cada una de sus mallas, llama a su meto de deibujado con el mismo shader
		for(unsigned int i{}; i<meshes.size(); i++)
			meshes[i].draw(shader);

		glEnable(GL_CULL_FACE);
	};

	void drawShadows(_Shader& shader){
		for(unsigned int i{}; i<meshes.size(); i++)
			meshes[i].drawShadows(shader);
	};

	// Metodo de cargado, con la ruta del archivo, el tipo de archivo y una referencia a las texturas cargadas, mas util en el futuro para los billboards
	void load(const std::string& path, std::vector<_Texture>& textLoaded, RType rt) override{
		// Asignamos el tipo de recurso
		type = rt;
		// Segun el tipo de recursos hace una cosa u otra
		if(rt == RType::TRCube)
			// Metemos en una mesh el cubo
			meshes.push_back(processCube());
		else if(rt == RType::TRImage){
    		stbi_set_flip_vertically_on_load(true);
			// Actualizamos la informacion de las texturas cargadas, para no repetirlas
			texturesLoaded = &textLoaded;
			if(path=="none"){
				meshes.push_back(processImage());
			}else{
				// Recuperamos el directorio del path
				directory = path.substr(0, path.find_last_of('/'));
				// Metemos en una mesh la imagen recuperada con el path
				meshes.push_back(processImage(path.substr(path.find_last_of('/')+1, path.size()), textLoaded));
			}
    		stbi_set_flip_vertically_on_load(false);
		}else if(rt == RType::TRRectangle){
			meshes.push_back(processRectangle());
		}else if(rt == RType::TRRecWired){
			meshes.push_back(processRectangleWired());
			meshes[0].mode = GL_LINES;
		}else{
			texturesLoaded = &textLoaded;
			// Iniciamos el importador de Assimp
			Assimp::Importer importer{};
			// Con el lector de Assimp, leemos el modelo y recuperamos la escena
			const aiScene* scene = importer.ReadFile(path, aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_GenSmoothNormals 
								| aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_GenBoundingBoxes);
			// Si no ha habido escena, esta vacia o Assimp nos marcaa un error, lo mostramos por consola
			if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
				std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
				return;
			}
			directory = path.substr(0, path.find_last_of('/'));
			// Procesamos la informacion de Assimp
			processNode(scene->mRootNode, scene);
		}
	};

	// Seteamos el shader del modelo
	void setShader(_Shader* sh){shader = sh;};

	// Recupera las mallas del modelo
	std::vector<TR_Mesh>& getMeshes(){return meshes;};

	// Recupera el valor de mezcla entre el color y la textura
	float getMixValue(){return mixValue;};
	void setMixValue(float mix){mixValue = mix;};

	void setBoundingBoxes(bool aabbs){for(auto& m : meshes){m.boundiboxesActived = aabbs;}};

	auto& getBoneInfoMap(){return aBoneInfoMap;};
	auto& getAnimator(){return animator;};
	void setBoneInforMap(const std::map<std::string, _BoneInfo>& aBones){aBoneInfoMap = aBones;};
	int& getNumBones(){return aNumBones;};
	void setNumbones(int b){aNumBones = b;};
	bool anim{};

	private:
		// Por cada una de las mallas del nodo del modelo actualiza el vector de mallas y recursivamente hace lo mismo con los hijos de la mismo
		void processNode(aiNode* node, const aiScene* scene){
			for(unsigned int i{}; i<node->mNumMeshes; i++){
				// Usamos solo un puntero a la malla para procesar dicha malla
				aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
				// Actualizamos el vector de mallas con la malla procesada
				meshes.push_back(processMesh(mesh, scene));
			}
			// Lo mismo para cada hijo de este nodo
			for(unsigned int i{}; i<node->mNumChildren; i++){
				processNode(node->mChildren[i], scene);
			}
		}

		// Procesa la informacion de la malla y devuelve un tipo malla propio
		TR_Mesh processMesh(aiMesh* mesh, const aiScene* scene){
			// Nuestro tipo malla propio puede tener n vertices, n indices y n texturas, asi que creamos unos vectores para almacenarlos
			std::vector<Vertex> vertices{};
			std::vector<unsigned int> indices{};
			std::vector<_Texture> textures{};
			// Por cada vertice, lo va procesando
			for(unsigned int i{}; i<mesh->mNumVertices; i++){
				// Creamos el vertex
				Vertex vertex{};
				setVertexBoneDefault(vertex);
				// Le anadimos la posicion del vertice
				vertex.position.x = mesh->mVertices[i].x;
				vertex.position.y = mesh->mVertices[i].y;
				vertex.position.z = mesh->mVertices[i].z;
				// SI tiene normales, las asigna
				if(mesh->HasNormals()){
					vertex.normal.x = mesh->mNormals[i].x;
					vertex.normal.y = mesh->mNormals[i].y;
					vertex.normal.z = mesh->mNormals[i].z;
				}
				// Si tiene coordenadas de textura, los asigna
				if(mesh->mTextureCoords[0]){
					vertex.texCord.x = mesh->mTextureCoords[0][i].x;
					vertex.texCord.y = mesh->mTextureCoords[0][i].y;
				}
				// Ahora anade al vector de vertices el vertex
				vertices.push_back(vertex);
			}
			// Por cada cara aprovechamos y le sacamos los indices
			for(unsigned int i{}; i<mesh->mNumFaces; i++){
				// Recuperamos cada cara
				aiFace face{mesh->mFaces[i]};
				// Por cada indice lo va metiendo en el vector de indices
				for(unsigned int j{}; j<face.mNumIndices; j++)
					indices.push_back(face.mIndices[j]);
			}
			// Recuperamos el material desde la escena con el indice que guarda la malla
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			// Hacemos un vector temporal con todos los mapas de difusion
			std::vector<_Texture> diffuseMaps{loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse")};
			// Insertamos al final de textures los mapas difusos
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			// Hacemos lo mismo con las texturas especulares
			std::vector<_Texture> specularMaps{loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular")};
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
			// Devolvemos un mesh propio con todo
			_BoundingBox aabb{};
			glm::vec3 aabbVec{};
			aabbVec.x = mesh->mAABB.mMin.x;
			aabbVec.y = mesh->mAABB.mMin.y;
			aabbVec.z = mesh->mAABB.mMin.z;
			aabb.min = aabbVec;
			aabbVec.x = mesh->mAABB.mMax.x;
			aabbVec.y = mesh->mAABB.mMax.y;
			aabbVec.z = mesh->mAABB.mMax.z;
			aabb.max = aabbVec;

			if(mesh->HasBones())
				extractBoneWeight(vertices, mesh, scene);

			return TR_Mesh(vertices, indices, textures, aabb);		
		}

		// Procesa el cubo
		TR_Mesh processCube(){
			std::vector<Vertex> vertices{};
			std::vector<unsigned int> indices{};
			std::vector<_Texture> textures{};

			// La forma mas optima de hacerlo es metiendole yo los datos a mano, el cubo basico no tiene textura

			vertices.push_back(Vertex{glm::vec3(  1.0f,  1.0f, -1.0f), glm::vec3( -0.0f,  1.0f, -0.0f), glm::vec2(  0.625f,  0.5f)});
			vertices.push_back(Vertex{glm::vec3( -1.0f,  1.0f, -1.0f), glm::vec3( -0.0f,  1.0f, -0.0f), glm::vec2(  0.875f,  0.5f)});
			vertices.push_back(Vertex{glm::vec3( -1.0f,  1.0f,  1.0f), glm::vec3( -0.0f,  1.0f, -0.0f), glm::vec2(  0.875f,  0.25f)});
			vertices.push_back(Vertex{glm::vec3(  1.0f,  1.0f,  1.0f), glm::vec3( -0.0f,  1.0f, -0.0f), glm::vec2(  0.625f,  0.25f)});
			vertices.push_back(Vertex{glm::vec3(  1.0f, -1.0f,  1.0f), glm::vec3( -0.0f, -0.0f,  1.0f), glm::vec2(  0.375f,  0.25f)});
			vertices.push_back(Vertex{glm::vec3(  1.0f,  1.0f,  1.0f), glm::vec3( -0.0f, -0.0f,  1.0f), glm::vec2(  0.625f,  0.25f)});
			vertices.push_back(Vertex{glm::vec3( -1.0f,  1.0f,  1.0f), glm::vec3( -0.0f, -0.0f,  1.0f), glm::vec2(  0.625f,  0.0f)});
			vertices.push_back(Vertex{glm::vec3( -1.0f, -1.0f,  1.0f), glm::vec3( -0.0f, -0.0f,  1.0f), glm::vec2(  0.375f,  0.0f)});
			vertices.push_back(Vertex{glm::vec3( -1.0f, -1.0f,  1.0f), glm::vec3( -1.0f, -0.0f, -0.0f), glm::vec2(  0.375f,  1.0f)});
			vertices.push_back(Vertex{glm::vec3( -1.0f,  1.0f,  1.0f), glm::vec3( -1.0f, -0.0f, -0.0f), glm::vec2(  0.625f,  1.0f)});
			vertices.push_back(Vertex{glm::vec3( -1.0f,  1.0f, -1.0f), glm::vec3( -1.0f, -0.0f, -0.0f), glm::vec2(  0.625f,  0.75f)});
			vertices.push_back(Vertex{glm::vec3( -1.0f, -1.0f, -1.0f), glm::vec3( -1.0f, -0.0f, -0.0f), glm::vec2(  0.375f,  0.75f)});
			vertices.push_back(Vertex{glm::vec3( -1.0f, -1.0f, -1.0f), glm::vec3( -0.0f, -1.0f, -0.0f), glm::vec2(  0.125f,  0.5f)});
			vertices.push_back(Vertex{glm::vec3(  1.0f, -1.0f, -1.0f), glm::vec3( -0.0f, -1.0f, -0.0f), glm::vec2(  0.375f,  0.5f)});
			vertices.push_back(Vertex{glm::vec3(  1.0f, -1.0f,  1.0f), glm::vec3( -0.0f, -1.0f, -0.0f), glm::vec2(  0.375f,  0.25f)});
			vertices.push_back(Vertex{glm::vec3( -1.0f, -1.0f,  1.0f), glm::vec3( -0.0f, -1.0f, -0.0f), glm::vec2(  0.125f,  0.25f)});
			vertices.push_back(Vertex{glm::vec3(  1.0f, -1.0f, -1.0f), glm::vec3(  1.0f, -0.0f, -0.0f), glm::vec2(  0.375f,  0.5f)});
			vertices.push_back(Vertex{glm::vec3(  1.0f,  1.0f, -1.0f), glm::vec3(  1.0f, -0.0f, -0.0f), glm::vec2(  0.625f,  0.5f)});
			vertices.push_back(Vertex{glm::vec3(  1.0f,  1.0f,  1.0f), glm::vec3(  1.0f, -0.0f, -0.0f), glm::vec2(  0.625f,  0.25f)});
			vertices.push_back(Vertex{glm::vec3(  1.0f, -1.0f,  1.0f), glm::vec3(  1.0f, -0.0f, -0.0f), glm::vec2(  0.375f,  0.25f)});
			vertices.push_back(Vertex{glm::vec3( -1.0f, -1.0f, -1.0f), glm::vec3( -0.0f, -0.0f, -1.0f), glm::vec2(  0.375f,  0.75f)});
			vertices.push_back(Vertex{glm::vec3( -1.0f,  1.0f, -1.0f), glm::vec3( -0.0f, -0.0f, -1.0f), glm::vec2(  0.625f,  0.75f)});
			vertices.push_back(Vertex{glm::vec3(  1.0f,  1.0f, -1.0f), glm::vec3( -0.0f, -0.0f, -1.0f), glm::vec2(  0.625f,  0.5f)});
			vertices.push_back(Vertex{glm::vec3(  1.0f, -1.0f, -1.0f), glm::vec3( -0.0f, -0.0f, -1.0f), glm::vec2(  0.375f,  0.5f)});

			for(auto& vert : vertices){
				setVertexBoneDefault(vert);
			}

			indices.push_back(0);
			indices.push_back(1);
			indices.push_back(2);
			indices.push_back(0);
			indices.push_back(2);
			indices.push_back(3);
			indices.push_back(4);
			indices.push_back(5);
			indices.push_back(6);
			indices.push_back(4);
			indices.push_back(6);
			indices.push_back(7);
			indices.push_back(8);
			indices.push_back(9);
			indices.push_back(10);
			indices.push_back(8);
			indices.push_back(10);
			indices.push_back(11);
			indices.push_back(12);
			indices.push_back(13);
			indices.push_back(14);
			indices.push_back(12);
			indices.push_back(14);
			indices.push_back(15);
			indices.push_back(16);
			indices.push_back(17);
			indices.push_back(18);
			indices.push_back(16);
			indices.push_back(18);
			indices.push_back(19);
			indices.push_back(20);
			indices.push_back(21);
			indices.push_back(22);
			indices.push_back(20);
			indices.push_back(22);
			indices.push_back(23);

			// Devuelve la mesh
			return TR_Mesh(vertices, indices, textures);
		}

		// Procesa la imagen
		TR_Mesh processImage(std::string path, [[maybe_unused]] std::vector<_Texture>& textLoaded){
			std::vector<Vertex> vertices{};
			std::vector<unsigned int> indices{};
			std::vector<_Texture> textures{};

			// De nuevo, metemos los datos a mano
			vertices.push_back(Vertex{glm::vec3(  1.0f,  1.0f,  0.0f), glm::vec3(  1.0f,  1.0f,  0.0f), glm::vec2(  1.0f,  1.0f)});
			vertices.push_back(Vertex{glm::vec3(  1.0f, -1.0f,  0.0f), glm::vec3(  1.0f,  1.0f,  0.0f), glm::vec2(  1.0f,  0.0f)});
			vertices.push_back(Vertex{glm::vec3( -1.0f, -1.0f,  0.0f), glm::vec3(  1.0f,  1.0f,  0.0f), glm::vec2(  0.0f,  0.0f)});
			vertices.push_back(Vertex{glm::vec3( -1.0f,  1.0f,  0.0f), glm::vec3(  1.0f,  1.0f,  0.0f), glm::vec2(  0.0f,  1.0f)});

			for(auto& vert : vertices){
				setVertexBoneDefault(vert);
			}

			indices.push_back(0);
			indices.push_back(1);
			indices.push_back(3);
			indices.push_back(2);
			indices.push_back(3);
			indices.push_back(1);

			// Comprobamos que la textura no exista ya, asi evitamos repetir el procesado de la imagen
			bool skip{};
			for(unsigned int j{}; j<texturesLoaded->size(); j++){
				if(std::strcmp(texturesLoaded->at(j).path.data(), path.c_str())==0){
					textures.push_back(texturesLoaded->at(j));
					skip = true;
					break;
				}
			}
			if(!skip){
				// Si no existe, cargamos los datos de la imagen del path como texturas difusas y especulares
				_Texture texture{};
				texture.id = textureFromFile(path.c_str(), this->directory);
				texture.type = "texture_diffuse";
				texture.path = path.c_str();
				textures.push_back(texture);
				texture.type = "texture_spècular";
				textures.push_back(texture);
				// Actualizamos la texturas cargadas
				texturesLoaded->push_back(texture);
			}
			
			// Como hay tectura, el color no lo tiene en cuenta
			mixValue = 0.0f;

			return TR_Mesh(vertices, indices, textures);
		}

		// Procesa la imagen sin texturas
		TR_Mesh processImage(){
			std::vector<Vertex> vertices{};
			std::vector<unsigned int> indices{};
			std::vector<_Texture> textures{};

			// De nuevo, metemos los datos a mano
			vertices.push_back(Vertex{glm::vec3(  1.0f,  1.0f,  0.0f), glm::vec3(  1.0f,  1.0f,  0.0f), glm::vec2(  1.0f,  1.0f)});
			vertices.push_back(Vertex{glm::vec3(  1.0f, -1.0f,  0.0f), glm::vec3(  1.0f,  1.0f,  0.0f), glm::vec2(  1.0f,  0.0f)});
			vertices.push_back(Vertex{glm::vec3( -1.0f, -1.0f,  0.0f), glm::vec3(  1.0f,  1.0f,  0.0f), glm::vec2(  0.0f,  0.0f)});
			vertices.push_back(Vertex{glm::vec3( -1.0f,  1.0f,  0.0f), glm::vec3(  1.0f,  1.0f,  0.0f), glm::vec2(  0.0f,  1.0f)});

			for(auto& vert : vertices){
				setVertexBoneDefault(vert);
			}

			indices.push_back(0);
			indices.push_back(1);
			indices.push_back(3);
			indices.push_back(2);
			indices.push_back(3);
			indices.push_back(1);

			textures.push_back(_Texture{});
			
			// Como hay tectura, el color no lo tiene en cuenta
			mixValue = 0.0f;

			return TR_Mesh(vertices, indices, textures);
		}


		TR_Mesh processRectangle(){
			std::vector<Vertex> vertices{};
			std::vector<unsigned int> indices{};
			std::vector<_Texture> textures{};

			// De nuevo, metemos los datos a mano
			vertices.push_back(Vertex{glm::vec3(  1.0f,  1.0f,  0.0f), glm::vec3(  1.0f,  1.0f,  0.0f), glm::vec2(  1.0f,  1.0f)});
			vertices.push_back(Vertex{glm::vec3(  1.0f, -1.0f,  0.0f), glm::vec3(  1.0f,  1.0f,  0.0f), glm::vec2(  1.0f,  0.0f)});
			vertices.push_back(Vertex{glm::vec3( -1.0f, -1.0f,  0.0f), glm::vec3(  1.0f,  1.0f,  0.0f), glm::vec2(  0.0f,  0.0f)});
			vertices.push_back(Vertex{glm::vec3( -1.0f,  1.0f,  0.0f), glm::vec3(  1.0f,  1.0f,  0.0f), glm::vec2(  0.0f,  1.0f)});

			for(auto& vert : vertices){
				setVertexBoneDefault(vert);
			}

			indices.push_back(0);
			indices.push_back(1);
			indices.push_back(3);
			indices.push_back(2);
			indices.push_back(3);
			indices.push_back(1);
			
			// Como no hay tectura, solo tiene en cuenta el color
			mixValue = 1.0f;

			return TR_Mesh(vertices, indices, textures);
		}

		TR_Mesh processRectangleWired(){
			std::vector<Vertex> vertices{};
			std::vector<unsigned int> indices{};
			std::vector<_Texture> textures{};

			// De nuevo, metemos los datos a mano
			vertices.push_back(Vertex{glm::vec3(  1.0f,  1.0f,  0.0f), glm::vec3(  1.0f,  1.0f,  0.0f), glm::vec2(  1.0f,  1.0f)});
			vertices.push_back(Vertex{glm::vec3(  1.0f, -1.0f,  0.0f), glm::vec3(  1.0f,  1.0f,  0.0f), glm::vec2(  1.0f,  0.0f)});
			vertices.push_back(Vertex{glm::vec3( -1.0f, -1.0f,  0.0f), glm::vec3(  1.0f,  1.0f,  0.0f), glm::vec2(  0.0f,  0.0f)});
			vertices.push_back(Vertex{glm::vec3( -1.0f,  1.0f,  0.0f), glm::vec3(  1.0f,  1.0f,  0.0f), glm::vec2(  0.0f,  1.0f)});

			for(auto& vert : vertices){
				setVertexBoneDefault(vert);
			}

			indices.push_back(0);
			indices.push_back(1);
			indices.push_back(1);
			indices.push_back(2);
			indices.push_back(2);
			indices.push_back(3);
			indices.push_back(3);
			indices.push_back(0);
			
			// Como no hay tectura, solo tiene en cuenta el color
			mixValue = 1.0f;

			return TR_Mesh(vertices, indices, textures);
		}

		// Carga las texturas de los materiales
		std::vector<_Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName){
			// Como podria tener muchas texturas, hacemos un vector
			std::vector<_Texture> textures{};
			if(mat->GetTextureCount(type) != 0){
				// Como hay textura, el valor de mexcla sera 0 y no cuenta el color
				mixValue = 0.0f;
				// Por cada tectura, comprobamos si existe en las texturas cargadas, si no la creamos
				for(unsigned int i{}; i<mat->GetTextureCount(type); i++){
					aiString str{};
					mat->GetTexture(type, i, &str);
					bool skip{};
					for(unsigned int j{}; j<texturesLoaded->size(); j++){
						if(std::strcmp(texturesLoaded->at(j).path.data(), str.C_Str())==0){
							textures.push_back(texturesLoaded->at(j));
							skip = true;
							break;
						}
					}
					// Si no lo ha encontrado, la crea
					if(!skip){
						_Texture texture{};
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
		unsigned int textureFromFile(const char* path, const std::string &directory, [[maybe_unused]] bool gamma = false){
			// Ajustamos el path. Esto es feo pero Assimp lo necesita asi asi que bueno
			std::string filename{std::string(path)};
			filename = directory + '/' + filename;
			// Generamos la nueva textura
			unsigned int textureID{};
			glGenTextures(1, &textureID);
			// Cargamos con stbi la informacion de la imagen
			int width{}, height{}, nrComponents{};
			replaceString(filename, "\\", '/');
			unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
			// Si ha recuperado datos, lo cargamos en la textura si no mostramos un error por terminal
			if(data){
				// Creamos un formato base, segun el numero de canales de color le ponemos uno u otro
				GLenum format{};
				if(nrComponents == 1)
					// Si solo tiene un canal, solo "rojo" porque OpenGL lo llama asi
					format = GL_RED;
				else if(nrComponents == 3)
					// Si tiene tres, RGB
					format = GL_RGB;
				else if(nrComponents == 4)
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
			}else{
				std::cout << "_Texture failed to load in: " << path << " (A.K.A '" << directory << "/" << path << "')" << std::endl;
				stbi_image_free(data);
			}
			// Devolvemos la ID que le ha asignado OpenGL a la textura 
			return textureID;
		}

		void setVertexBoneDefault(Vertex& vertex){
			for(int i{}; i<MAX_BONE_INFLUENCE; i++){
				vertex.aBoneIDs[i] = -1;
				vertex.aWeights[i] = 0.0f;
			}
		}

		void setVertexBoneData(Vertex& vertex, int boneID, float weight){
			for(int i{}; i<MAX_BONE_INFLUENCE; ++i){
				if(vertex.aBoneIDs[i] < 0){
					vertex.aBoneIDs[i] = boneID;
					vertex.aWeights[i] = weight;
					break;
				}
			}
		}

		void extractBoneWeight(std::vector<Vertex>& vertices, aiMesh* mesh, [[maybe_unused]] const aiScene* scene){
			auto& boneInfoMap = aBoneInfoMap;
			int& numBones = aNumBones;

			for(unsigned int idx{}; idx < mesh->mNumBones; idx+=1){
				int boneID{-1};
				std::string boneName{mesh->mBones[idx]->mName.C_Str()};
				if(boneInfoMap.find(boneName) == boneInfoMap.end()){
					_BoneInfo newBone{};
					newBone.id = numBones;
					newBone.offset = AssimpConverter::matToGLM(mesh->mBones[idx]->mOffsetMatrix);
					boneInfoMap[boneName] = newBone;
					boneID = numBones;
					numBones += 1;
				}else{
					boneID = boneInfoMap[boneName].id;
				}
				assert(boneID != -1);
				auto weights = mesh->mBones[idx]->mWeights;
				unsigned int numWeights = mesh->mBones[idx]->mNumWeights;
				for(unsigned int weightIdx{}; weightIdx < numWeights; weightIdx += 1){
					unsigned int vertexID = weights[weightIdx].mVertexId;
					float weight = weights[weightIdx].mWeight;
					assert(vertexID <= vertices.size());
					setVertexBoneData(vertices[vertexID], boneID, weight);
				}
			}
		}
		
		// Tipo del recurso
		RType type{};
		// Vectores de meses y texturas
		std::vector<TR_Mesh> meshes{};
		std::vector<_Texture>* texturesLoaded{};
		std::map<std::string, _BoneInfo> aBoneInfoMap{};
		int aNumBones{}; 
		// La carpeta contenedora del modelo o textura
		std::string directory{};
		// Puntero al Shader
		_Shader* shader{};
    	Animator animator{};
		// Correccion del valor gamma y el valor de mezcla
		bool gammaCorrection{};
		float mixValue{0.99f};
};