#pragma once

struct TR_Model;
struct TR_Shader;
struct TCamera;

struct T2DElement: virtual public TEntity{
	T2DElement(){};
	T2DElement(TResourceManager* RM_){RM = RM_;};
	T2DElement(TResourceManager* RM_, std::string name, RType type){RM = RM_; model = RM->getModel(name, name, type);};

	void draw(glm::mat4 matrix = glm::mat4(1.0f), TCamera* principalCamera = nullptr) override{
		if(shader && model && principalCamera){
			glDisable(GL_DEPTH_TEST);
			model->setShader(&shader->shader);
			_Shader& actualShader = shader->shader;
			actualShader.use();
			glm::mat4 modelM = matrix;
			actualShader.setMat4("model", modelM);
			actualShader.setVec2("imageScale", imageSize);
			actualShader.setVec2("imagePos", imagePos);
			actualShader.setInt("material.diffuse", 0);
			actualShader.setInt("material.specular", 1);
			actualShader.setFloat("material.shininess", 64.0f);
			actualShader.setVec3("material.color", glm::vec3(color.x/255, color.y/255, color.z/255));
			actualShader.setFloat("material.alpha", color.w);
			actualShader.setFloat("mixValue", model->getMixValue());
            if(wired)
                glPolygonMode( GL_FRONT_AND_BACK, GL_LINE);
			if(model->getMeshes().at(0).textures.size() > 0){
				model->getMeshes().at(0).textures[0].id = difusse.id;
				model->getMeshes().at(0).textures[0].type = difusse.type;
				model->getMeshes().at(0).textures[0].path = difusse.path;
			}
			model->drawWithShader(actualShader);
            glPolygonMode( GL_FRONT_AND_BACK, GL_FILL);
			glEnable(GL_DEPTH_TEST);
		}
	};

	void drawShadowArray([[maybe_unused]]_Shader* shader, [[maybe_unused]] uint16_t& shadowCont, [[maybe_unused]] glm::mat4 matrix = glm::mat4(1.0f)) override {};

	void drawShadowCubes([[maybe_unused]]_Shader* shader, [[maybe_unused]] uint16_t& shadowCont, [[maybe_unused]] glm::mat4 matrix = glm::mat4(1.0f)) override {};

	// Busca el shader dentro del gesto de recurso y asigna el resultado.
	void setShader(std::string name){shader = RM->getShader(name, name, name);};
	// Setea la relacion de aspecto
	void setAspect(float asp){aspect = asp;};

	void setColor(Vec4 col){
		color.x = col.x;
		color.y = col.y;
		color.z = col.z;
		color.w = col.w;
	}

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
	_Texture difusse{};
	float aspect{};
	// El color, por defecto es el amarillo
	glm::vec4 color{253.0f, 249.0f, 0.0f, 1.0f};
	glm::vec2 imageSize{}, imagePos{};
    bool wired{};
};