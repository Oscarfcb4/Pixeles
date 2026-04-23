#pragma once

struct R_Model;
struct R_Shader;
struct E_Camera;

struct E_Model: virtual public E_Entity{
	// Constructor del modelo base
	E_Model(){};
	// Constructor con un gestor de recurso, necesario para el dibujado, pero sin un modelo
	E_Model(ResourceManager* RM){this->RM = RM;};
	// Constructor con un gestor de recurso y un recurso asociado, ademas de un tipo de recurso. La ruta igual al nombre
	E_Model(ResourceManager* RM, std::string name, RType type){ this->RM = RM; model = RM->getModel(name, name, type);};
	// Constructor con un gestor de recurso y un recurso asociado, ademas de un tipo de recurso. La ruta diferente al nombre
	E_Model(ResourceManager* RM, std::string name, std::string src, RType type) { this->RM = RM; model = RM->getModel(name, src, type); };
	// Metodo de dibujado, comunes para todos los modelos
	void draw(glm::mat4 matrix = glm::mat4(1.0f), E_Camera* principalCamera = nullptr) override{
		// Si hay un shader, un modelo y una camara principal se dibuja, si no no se podrá
		if(shader && model && principalCamera){
			// Le asigna el shader al modelo
			model->setShader(&shader->shader);
			// Creo una referencia a ese shader, para que sea mas facil de usar
			Shader& actualShader = shader->shader;
			// Activo el shader
			actualShader.use();
			// La mtriz modelo es la matriz acumulaba
			glm::mat4 modelMatrix = matrix;
			// La matriz vista usando el metodo de la camara, ya que es a partir de ella
			// desde donde vamos a visualizar la escena
			glm::mat4 viewMatrix = principalCamera->getViewMatrix();
			// Para simplificar, la matriz proyeccion va a ser siempre perspectiva (Puedes hacer que la informe el
			// usuario si quieres). Usamos el fov de la camara (zoom), la relacion de aspecto que ha marcado el usuario 
			// y un z_near y z_far que puedes hacer también que sea modular si quieres
			glm::mat4 projectionMatrix = glm::perspective(glm::radians(principalCamera->zoom), aspect, 0.1f, 200.0f);
			// Calculamos la modelViewProjection en CPU porque aunque es mejor calcularlo una vez lento que N veces rapido 
			// en el vertex segun el numero de vertices del modelo 
			glm::mat4 mvp = projectionMatrix * viewMatrix * modelMatrix;
			actualShader.setMat4("modelViewProjection", mvp);
			// Para el calculo de la normale necesitamos pasarle el modelo 
			actualShader.setMat4("model", modelMatrix);
			actualShader.setVec3("cameraPos", glm::vec3(principalCamera->position.x, principalCamera->position.y, principalCamera->position.z));
			// Le pasamos un material base para el color, si ya hay una textura el mixValue sera 0 por lo que este material no hara nada, 
			// pero es util cuando no hay texturas en el modelo
			actualShader.setInt("material.diffuse", 0);
			actualShader.setInt("material.specular", 1);
			actualShader.setFloat("material.shininess", 64.0f);
			// El color definido por el usuario, de base es el amarillo
			actualShader.setVec3("material.color", glm::vec3(color.r, color.g, color.b));
			// El mixValue del modelo, que indicara cuanta fuerza tiene el color sobre el resultado final
			actualShader.setFloat("mix_value", model->getMixValue());
			// Ahora que hemos seteado el shader llamamos al renderizado del modelo
			model->drawWithShader(actualShader);
		}
	};

	// Busca el shader dentro del gesto de recursos
	void setShader(std::string name){shader = RM->getShader(name, name, name);};
	// Setea la relacion de aspecto
	void setAspect(float asp){aspect = asp;};
	// Asignamos el modelo, el nombre igual a la ruta
	void setModel(std::string name, RType type) { model = RM->getModel(name, name, type); };
	// Asignamos el modelo, el nombre y la ruta diferentes
	void setModel(std::string name, std::string src, RType type) { model = RM->getModel(name, src, type); };
	// Busca y asigna una textura al modelo
	void setTexture(std::string path) { if (model) model->addTexture(path, "texture_diffuse", RM->texturesLoaded); };

	// Un puntero al modelo y al shader
	R_Model* model{};
	R_Shader* shader{};
	// La relación de aspecto
	float aspect{};
	// El color, por defecto es el amarillo
	Color color{1.0f, 1.0f, 0.0f, 1.0f};
};