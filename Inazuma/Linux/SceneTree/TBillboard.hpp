#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

struct TR_Model;
struct TR_Shader;
struct TCamera;

struct TBillboard: virtual public TEntity{
	// Constructor del modelo base
	TBillboard(){};
	// Constructor con un gestor de recurso, necesario para el dibujado, pero sin un modelo
	TBillboard(TResourceManager* RM_){RM = RM_;};
	// COnstructor con un gestor de recurso y un recurso asociado, ademas de un tipo de recurso
	TBillboard(TResourceManager* RM_, std::string name, RType type){RM = RM_; model = RM->getModel(name, name, type);};
	// Metodo de dibujado, comunes para todos los modelos
	void draw([[maybe_unused]] glm::mat4 matrix = glm::mat4(1.0f), TCamera* principalCamera = nullptr) override{
		// Si hay un shader, un modelo y una camara principal se dibuja si no no se podra
		if(shader && model && principalCamera){
            // Le asigna el shader al modelo
			model->setShader(&shader->shader);
			// Creo una referencia a ese shader, para que sea mas facil de usar
			_Shader& actualShader = shader->shader;
			// Activo el shader
			actualShader.use();
            //modelM = modelM * glm::eulerAngleXYZ(0.0f, phi, 0.0f) * glm::eulerAngleXYZ(0.0f, 0.0f, theta) ; 
            // La matriz vista usando el metodo de la camara
			glm::mat4 view = principalCamera->getViewMatrix();
			// La matriz proyeccion en este casi va a ser siempre perspectiva (nunca he visto a nadie usar una ortogonal mas alla de blender), usamos el
			// fov de la camara (zoom), la relacion de aspecto que ha marcado el usuario y un z_near y z_far que en un futuro hare que sea modular 
			glm::mat4 projection = glm::perspective(glm::radians(principalCamera->zoom), aspect, 0.1f, 200.0f);
			// Calculamos la modelViewProjection en CPU porque aunque es mejor calcularlo una vez lento que N veces rapido en el vertex segun el nummero
			// de vertices del modelo 
			glm::mat4 viewModel{view * matrix};
			viewModel[0][0] = 1.0f;
			viewModel[1][0] = 0.0f;
			viewModel[2][0] = 0.0f;
			viewModel[0][2] = 0.0f;
			viewModel[1][2] = 0.0f;
			viewModel[2][2] = 1.0f;
			glm::mat4 modelM = glm::mat4(1.0f);
			actualShader.setMat4("modelViewProjection", projection * viewModel * modelM);
			// Para el calculo de la normale necesitamos pasarle el modelo 
			actualShader.setMat4("model", modelM);
			actualShader.setMat4("view", viewModel);
			actualShader.setVec3("cameraPos", glm::vec3(principalCamera->position.x, principalCamera->position.y, principalCamera->position.z));
			// Le pasamos un material base para el color, si ya hay una textura el mixValue sera 0 por lo que este material no hara nada, pero es util 
			// cuando no hay texturas en el modelo
			actualShader.setInt("material.diffuse", 0);
			actualShader.setInt("material.specular", 1);
			actualShader.setFloat("material.shininess", 64.0f);
			// El color definido por el usuario, de base es el amarillo
			actualShader.setVec3("material.color", glm::vec3(color.x/255, color.y/255, color.z/255));
			// El mixValue del modelo, que indicara cuanta fuerza tiene el color sobre el resultado final
			actualShader.setFloat("mixValue", model->getMixValue());
			// Ahora que hemos seteado el shader llamamos al renderizado del modelo
			model->drawWithShader(actualShader);
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
	glm::vec3 pos{0.0f, 0.0f, 0.0f};
};