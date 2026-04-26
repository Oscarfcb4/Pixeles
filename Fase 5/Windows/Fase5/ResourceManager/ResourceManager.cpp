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
void R_Model::addTexture(const std::string& path, const std::string& type, std::vector<Texture>& texturesLoaded) {
	// Comprobamos si la textura ya esta cargada
	for (auto& t : texturesLoaded) {
		if (t.path == path) {
			for (auto& mesh : meshes) {
				mesh.textures.push_back(t);
			}
			// Ponemos el mixValue a 0 para que se vea sólo la textura
			mixValue = 0.0f;
			return;
		}
	}

	// No existe, la cargamos, Primero generamos una id, como con los buffers
	unsigned int textureID{};
	// Le decimos a OpenGL que genere el espacio para la textura
	glGenTextures(1, &textureID);
	// Creamos unas variables para la altura, la anchura y los componentes de la imagen
	int width{}, height{}, nrComponents{};
	// Cargamos la imagen con STB
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
	// Si la carga ha sido exitosa, entonces continuamos
	if (data) {
		// Ahora comprobamos el formato de la imagen según sus componentes
		GLenum format{};
		// Un componente, será sólo roja la imagen
		if (nrComponents == 1) format = GL_RED;
		// Con tres roja, verde y azul (RGB)
		else if (nrComponents == 3) format = GL_RGB;
		// Y si tiene canal alpha para transparencias, cuatro
		else if (nrComponents == 4) format = GL_RGBA;

		// Enlazamos la id que generamos antes
		glBindTexture(GL_TEXTURE_2D, textureID);
		// Le decimos a OpenGL que cargue la textura con una imagen
		glTexImage2D(
			GL_TEXTURE_2D,    // El tipo de textura objetivo
 			0,                // Nivel de imagen, por defecto es 0
			format,           // Formato de la imagen
			width,            // Ancho
			height,           // Alto
			0,                // El borde de la imagen
			format,           // Formato de los píxeles, podría ser diferente
			GL_UNSIGNED_BYTE, // El tipo de los datos de los píxeles 
			data              // Puntero a la imagen cargada con STB
		);
		// Añadimos los parámetros del Wrapping, el Filtering y generamos los MinMaps
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);
		// Liberamos de memoria RAM la imagen que ya esta cargada en VRAM
		stbi_image_free(data);
	} else {
		// Si no ha cargado, debugeamos
		std::cout << "Texture failed to load: " << path << std::endl;
	}

	// Generamos un objeto textura, le asignamos los valores recuperados 
	// y lo metemos en el array de texturas cargadas.
	Texture texture{};
	texture.id = textureID;
	texture.type = type;
	texture.path = path;
	texturesLoaded.push_back(texture);

	// Actualizamos las mallas del modelo con la nueva textura
	for (auto& mesh : meshes) {
		mesh.textures.push_back(texture);
	}
	// Ponemos el mixValue a 0 para que se vea sólo la textura
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