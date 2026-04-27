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
			_Shader& actualShader = shader->shader;
			// Activo el shader
			actualShader.use();
			// La mtriz modelo es la matriz acumulaba
			glm::mat4 modelM = matrix;
			// La matriz vista usando el metodo de la camara
			glm::mat4 view = principalCamera->getViewMatrix();
			// La matriz proyeccion en este casi va a ser siempre perspectiva (nunca he visto a nadie usar una ortogonal mas alla de blender), usamos el
			// fov de la camara (zoom), la relacion de aspecto que ha marcado el usuario y un z_near y z_far que en un futuro hare que sea modular 
			glm::mat4 projection = glm::perspective(glm::radians(principalCamera->zoom), aspect, 0.1f, 20000.0f);
			// Calculamos la modelViewProjection en CPU porque aunque es mejor calcularlo una vez lento que N veces rapido en el vertex segun el nummero
			// de vertices del modelo 
			actualShader.setMat4("modelViewProjection", projection * view * modelM);
			// Para el calculo de la normale necesitamos pasarle el modelo 
			actualShader.setMat4("model", modelM);
			actualShader.setMat4("view", view);
			actualShader.setVec3("cameraPos", glm::vec3(principalCamera->position.x, principalCamera->position.y, principalCamera->position.z));
			actualShader.setVec2("imageScale", imageSize);
			actualShader.setVec2("imagePos", imagePos);
			// Le pasamos un material base para el color, si ya hay una textura el mixValue sera 0 por lo que este material no hara nada, pero es util 
			// cuando no hay texturas en el modelo
			actualShader.setInt("material.diffuse", 0);
			actualShader.setInt("material.specular", 1);
			actualShader.setFloat("material.shininess", 64.0f);
			// El color definido por el usuario, de base es el amarillo
			actualShader.setVec3("material.color", glm::vec3(color.x/255, color.y/255, color.z/255));
			actualShader.setFloat("material.alpha", color.w);
			// El mixValue del modelo, que indicara cuanta fuerza tiene el color sobre el resultado final
			actualShader.setFloat("mixValue", model->getMixValue());
			actualShader.setBool("anim", false);
			// Ahora que hemos seteado el shader llamamos al renderizado del modelo
			model->drawWithShader(actualShader);
		}
	};

	void drawShadowArray(_Shader* shader, [[maybe_unused]] uint16_t& shadowCont, glm::mat4 matrix = glm::mat4(1.0f)) override {
		model->setShader(shader);
		_Shader& actualShader = *shader;
		glm::mat4 modelM = matrix;
		actualShader.setMat4("model", modelM);
		model->drawShadows(actualShader);
	};

	void drawShadowCubes(_Shader* shader, [[maybe_unused]] uint16_t& shadowCont, glm::mat4 matrix = glm::mat4(1.0f)) override {
		model->setShader(shader);
		_Shader& actualShader = *shader;
		glm::mat4 modelM = matrix;
		actualShader.setMat4("model", modelM);
		model->drawShadows(actualShader);
	};

	// Busca el shader dentro del gesto de recurso y asigna el resultado.
	void setShader(std::string name){shader = RM->getShader(name, name, name);};
	// Setea la relacion de aspecto
	void setAspect(float asp){aspect = asp;};

	void drawBoundingBoxes(bool aabbs = true){model->setBoundingBoxes(aabbs);};

	void setImage(Vec2 size, Vec2 pos){
		imageSize.x = size.x; 
		imageSize.y = size.y; 
		imagePos.x = pos.x;
		imagePos.y = pos.y;
	};

	void setAlpha(float alpha){
		if(alpha > 1.0f)
			alpha = 1.0f;
		if(alpha < 0.0f)
			alpha = 0.0f;

		color.w = alpha;
	};

	// Un puntero al modelo, el shader y la relacion de aspect
	TR_Model* model{};
	TR_Shader* shader{};
	float aspect{};
	// El color, por defecto es el amarillo
	glm::vec4 color{253.0f, 249.0f, 0.0f, 1.0f};
	glm::vec2 imageSize{}, imagePos{};
};