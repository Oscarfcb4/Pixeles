#include <string>
#include <vector>
#include <ResourceManager.hpp>
#include <Shader.hpp>  
#include "R_Model.hpp"

// STB necesita incluirse en un cpp y compilarse para funcionar, asi que lo incluyo asi. Ademas tambien necesita que le  
// defina que necesito la parte de el tratamiento de imagenes
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// Implementacion de addTexture para R_Model
void R_Model::addTexture(const std::string& path, const std::string& type) {
	// Generamos la nueva textura
	unsigned int textureID{};
	glGenTextures(1, &textureID);
	// Cargamos con stbi la informacion de la imagen
	int width{}, height{}, nrComponents{};
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
	// Si ha recuperado datos, lo cargamos en la textura
	if (data) {
		// Creamos un formato base, segun el numero de canales de color le ponemos uno u otro
		GLenum format{};
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;
		// Anidamos la textura
		glBindTexture(GL_TEXTURE_2D, textureID);
		// Cargamos la imagen
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		// Parametros para mimaps y texturas
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Generamos los mipmaps
		glGenerateMipmap(GL_TEXTURE_2D);
		// Liberamos los datos
		stbi_image_free(data);
	}
	else {
		std::cout << "Texture failed to load: " << path << std::endl;
	}
	// Creamos la estructura de textura y la anadimos a todas las mallas
	Texture texture{};
	texture.id = textureID;
	texture.type = type;
	texture.path = path;
	for (auto& mesh : meshes) {
		mesh.textures.push_back(texture);
	}
	// El color ya no cuenta, la textura es la que importa
	mixValue = 0.0f;
}

// Llamada a la funcion de recoger recurso segun un modelo
R_Model* ResourceManager::getModel(std::string name, std::string src, RType rt) {
    return getResource<R_Model>(name, src, rt);
}

// Llamada a la funcion de recoger recurso segun un shader
R_Shader* ResourceManager::getShader(std::string name, std::string vs, std::string fs) {
    return getResource<R_Shader>(name, vs, fs);
}