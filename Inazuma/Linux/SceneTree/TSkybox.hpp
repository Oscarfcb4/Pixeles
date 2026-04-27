#pragma once

struct TR_Model;
struct TR_Shader;
struct TCamera;

struct TSkybox: virtual public TEntity{
	// Constructor del modelo base
	TSkybox(){skybox = true;};
	// Constructor con un gestor de recurso, necesario para el dibujado, pero sin un modelo
	TSkybox(TResourceManager* RM_){RM = RM_;skybox = true;};
	// COnstructor con un gestor de recurso y un recurso asociado, ademas de un tipo de recurso
	TSkybox(TResourceManager* RM_, std::string name, RType type){RM = RM_; model = RM->getModel(name, name, type);skybox = true;};
	// Metodo de dibujado, comunes para todos los modelos
	void draw([[maybe_unused]] glm::mat4 matrix = glm::mat4(1.0f), TCamera* principalCamera = nullptr) override{
		// Si hay un shader, un modelo y una camara principal se dibuja si no no se podra
		if(shader && model && principalCamera){
            glDisable(GL_CULL_FACE);
            glDisable(GL_DEPTH_TEST);
            glEnable(GL_DEPTH_CLAMP);
            glDepthFunc(GL_LEQUAL);
            model->setShader(&shader->shader);
            _Shader& actualShader = shader->shader;
            actualShader.use();
            glm::mat4 view = glm::mat4(glm::mat3(principalCamera->getViewMatrix()));
            glm::mat4 projection = glm::perspective(glm::radians(principalCamera->zoom), aspect, 0.1f, 200.0f);
            actualShader.setMat4("projectionView", projection * view);
            actualShader.setInt("skybox", 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, 3);
            model->drawWithShader(actualShader);
            glEnable(GL_CULL_FACE);
            glEnable(GL_DEPTH_TEST);
            glDisable(GL_DEPTH_CLAMP);
            glDepthFunc(GL_LESS);
		}
	};

    void drawShadowArray([[maybe_unused]] _Shader* shader, [[maybe_unused]] uint16_t& shadowCont, [[maybe_unused]] glm::mat4 matrix = glm::mat4(1.0f)){};
    void drawShadowCubes([[maybe_unused]] _Shader* shader, [[maybe_unused]] uint16_t& shadowCont, [[maybe_unused]] glm::mat4 matrix = glm::mat4(1.0f)){};

	// Busca el shader dentro del gesto de recurso y asigna el resultado.
	void setShader(std::string name){shader = RM->getShader(name, name, name);};
	// Setea la relacion de aspecto
	void setAspect(float asp){aspect = asp;};

	// Un puntero al modelo, el shader y la relacion de aspect
	TR_Model* model{};
	TR_Shader* shader{};
	float aspect{};
	// El color, por defecto es el amarillo
	glm::vec3 color{253.0f, 249.0f, 0.0f};
};