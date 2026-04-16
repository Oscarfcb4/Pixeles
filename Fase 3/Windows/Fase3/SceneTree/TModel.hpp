#pragma once

struct TR_Model;
struct TR_Shader;
struct TCamera;

struct TModel: virtual public TEntity{
	// Constructor del modelo base
	TModel(){};
	// Constructor con un gestor de recurso, necesario para el dibujado, pero sin un modelo
	TModel(TResourceManager* RM_){RM = RM_;};
	// COnstructor con un gestor de recurso y un recurso asociado, ademas de un tipo de recurso
	TModel(TResourceManager* RM_, std::string name, RType type){RM = RM_; model = RM->getModel(name, name, type);};
	// Metodo de dibujado, comunes para todos los modelos
	void draw(glm::mat4 matrix = glm::mat4(1.0f), TCamera* principalCamera = nullptr) override{
		// Si hay un shader, un modelo y una camara principal se dibuja si no no se podra
		if(shader && model && principalCamera){
			// Le asigna el shader al modelo
			model->setShader(&shader->shader);
			// Creo una referencia a ese shader, para que sea mas facil de usar
			Shader& actualShader = shader->shader;
			// Activo el shader
			actualShader.use();
			// La mtriz modelo es la matriz acumulaba
			glm::mat4 modelM = matrix;
			// La matriz vista usando el metodo de la camara
			glm::mat4 view = principalCamera->getViewMatrix();
			// La matriz proyeccion en este casi va a ser siempre perspectiva (nunca he visto a nadie usar una ortogonal mas alla de blender), usamos el
			// fov de la camara (zoom), la relacion de aspecto que ha marcado el usuario y un z_near y z_far que en un futuro hare que sea modular 
			glm::mat4 projection = glm::perspective(glm::radians(principalCamera->zoom), aspect, 0.1f, 200.0f);
			// Calculamos la modelViewProjection en CPU porque aunque es mejor calcularlo una vez lento que N veces rapido en el vertex segun el nummero
			// de vertices del modelo 
			glm::mat4 mvp = projection * view * modelM;
			actualShader.setMat4("modelViewProjection", projection * view * modelM);
			actualShader.setVec4("ourColor", color.x, color.y, color.z, 1.0f);
			// Ahora que hemos seteado el shader llamamos al renderizado del modelo
			model->drawWithShader(actualShader);
		}
	};

	// Busca el shader dentro del gesto de recurso y asigna el resultado.
	void setShader(std::string name){shader = RM->getShader(name, name, name);};
	// Setea la relacion de aspecto
	void setAspect(float asp){aspect = asp;};

	// Un puntero al modelo, el shader y la relacion de aspect
	TR_Model* model{};
	TR_Shader* shader{};
	float aspect{};
	// El color, por defecto es el amarillo
	glm::vec3 color{1.0f, 1.0f, 0.0f};
};