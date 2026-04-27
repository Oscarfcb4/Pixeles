#pragma once

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
	// Metodo de dibujado, comunes para todos los modelos
	void draw(glm::mat4 matrix = glm::mat4(1.0f), E_Camera* principalCamera = nullptr) override {
		// Si hay un shader, un modelo y una camara principal se dibuja si no no se podra
		if (shader && model && principalCamera) {
			model->setShader(&shader->shader);
			Shader& actualShader = shader->shader;
			actualShader.use();
			glDisable(GL_CULL_FACE);
			glDisable(GL_DEPTH_TEST);
			glEnable(GL_DEPTH_CLAMP);
			glDepthFunc(GL_LEQUAL);
			glm::mat4 view = glm::mat4(glm::mat3(principalCamera->getViewMatrix()));
			glm::mat4 projection = glm::perspective(glm::radians(principalCamera->zoom), aspect, 0.1f, 200.0f);
			actualShader.setMat4("projectionView", projection * view);
			actualShader.setInt("skybox", 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, 3);
			model->drawWithShader(actualShader);
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