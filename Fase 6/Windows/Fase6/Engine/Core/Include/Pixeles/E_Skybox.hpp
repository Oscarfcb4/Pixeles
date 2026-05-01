#pragma once
#include <GLM/glm.hpp>
#include <Pixeles/PixelesTypes.hpp>
#include <Pixeles/ResourceManager.hpp> 

struct R_Model;
struct R_Shader;
struct E_Camera;

struct E_Skybox : virtual public E_Entity {
	// Constructor del modelo base
	E_Skybox() { skybox = true; };
	// Constructor con un gestor de recurso, necesario para el dibujado, pero sin un modelo
	E_Skybox(ResourceManager* RM_) { RM = RM_; skybox = true; };
	// COnstructor con un gestor de recurso y un recurso asociado, ademas de un tipo de recurso
	E_Skybox(ResourceManager* RM_, std::string name, RType type) { RM = RM_; model = RM->getModel(name, name, type);skybox = true; };
	// Metodo de dibujado del skybox, renderiza el cubo mapa siempre detras de los objetos de la escena
	void draw(glm::mat4 matrix = glm::mat4(1.0f), E_Camera* principalCamera = nullptr) override {
		// Si hay un shader, un modelo y una camara principal se dibuja, si no no se podra
		if (shader && model && principalCamera) {
			// Asigna el shader al modelo y obtiene la referencia
			model->setShader(&shader->shader);
			Shader& actualShader = shader->shader;
			// Activamos el shader, el del skybox
			actualShader.use();
			// Desactiva el culling de caras para que el skybox se renderice con todas sus caras
			glDisable(GL_CULL_FACE);
			// Desactiva el depth test tradicional para gestionar la profundidad manualmente
			glDisable(GL_DEPTH_TEST);
			// Activa el clamping de profundidad para evitar problemas con la profundidad 1.0f
			glEnable(GL_DEPTH_CLAMP);
			// Usa LEQUAL para que el skybox compita con otros objetos a profundidad maxima
			glDepthFunc(GL_LEQUAL);
			// Extraemos la matriz de vista eliminando la modelo (el skybox sigue a la camara)
			glm::mat4 view = glm::mat4(glm::mat3(principalCamera->getViewMatrix()));
			// Calculamos la matriz de proyeccion usando el zoom de la camara y la relacion de aspecto
			glm::mat4 projection = glm::perspective(glm::radians(principalCamera->zoom), aspect, 0.1f, 200.0f);
			// Le pasa la combinacion de proyeccion y vista al shader (sin modelo)
			actualShader.setMat4("projectionView", projection * view);
			// Indica al shader que la textura del cubo mapa esta en la unidad 0
			actualShader.setInt("skybox", 0);
			// Activa la unidad de textura 0 y vincula el cubo mapa 
			// (ID 3 es el skybox precargado, lo pongo a mano por agilidad pero puedes hacerlo dinámico)
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, 3);
			// Dibuja el modelo usando el shader configurado
			model->drawWithShader(actualShader);
			// Restaura el estado de OpenGL a los valores por defecto
			glEnable(GL_CULL_FACE);
			glEnable(GL_DEPTH_TEST);
			glDisable(GL_DEPTH_CLAMP);
			glDepthFunc(GL_LESS);
		}
	};

	// Busca el shader dentro del gesto de recurso y asigna el resultado.
	void setShader(std::string name) { shader = RM->getShader(name, name, name); };
	// Setea la relacion de aspecto
	void setAspect(float asp) { aspect = asp; };

	// Un puntero al modelo, el shader y la relacion de aspect
	R_Model* model{};
	R_Shader* shader{};
	float aspect{};
	// El color, por defecto es el amarillo
	glm::vec3 color{ 253.0f, 249.0f, 0.0f };
};