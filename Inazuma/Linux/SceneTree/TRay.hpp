#pragma once

struct TR_Model;
struct TR_Shader;
struct TCamera;

struct TRay: virtual public TEntity{
	TRay(){};
	TRay(TResourceManager* RM_, Vec3 p, Vec3 d){RM = RM_; pos = p; dir = d;};
	void draw([[maybe_unused]] glm::mat4 matrix = glm::mat4(1.0f), TCamera* principalCamera = nullptr) override{
		if(shader && principalCamera){
			_Shader& actualShader = shader->shader;
			actualShader.use();

			glm::mat4 modelM = matrix;
			glm::mat4 view = principalCamera->getViewMatrix();
			glm::mat4 projection = glm::perspective(glm::radians(principalCamera->zoom), aspect, 0.1f, 20000.0f);
			actualShader.setMat4("mvp", projection * view * modelM);
			actualShader.setVec4("color", glm::vec4(color.r, color.g, color.b, color.a));
			
			float points[6];

			if(is3D){
				points[0] = pos.x;
				points[1] = pos.y;
				points[2] = pos.z;
				points[3] = dir.x;
				points[4] = dir.y;
				points[5] = dir.z;
			}else{
				float scale = 10000.0f;

				points[0] = pos.x;
				points[1] = pos.y;
				points[2] = pos.z;
				points[3] = pos.x + (dir.x * scale);
				points[4] = pos.y + (dir.y * scale);
				points[5] = pos.z + (dir.z * scale);
			}			

			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), &points[0], GL_STATIC_DRAW);
			glDrawArrays(GL_LINES, 0, 2);
			glBindVertexArray(0);
		}
	};

    void drawShadowArray([[maybe_unused]] _Shader* shader, [[maybe_unused]] uint16_t& shadowCont, [[maybe_unused]] glm::mat4 matrix = glm::mat4(1.0f)){};
    void drawShadowCubes([[maybe_unused]] _Shader* shader, [[maybe_unused]] uint16_t& shadowCont, [[maybe_unused]] glm::mat4 matrix = glm::mat4(1.0f)){};

	// Busca el shader dentro del gesto de recurso y asigna el resultado.
	void setShader(std::string name){shader = RM->getShader(name, name, name);};
	// Setea la relacion de aspecto
	void setAspect(float asp){aspect = asp;};
	void setBuffers(unsigned int vao, unsigned int vbo){VAO = vao; VBO = vbo;};

	// Un puntero al modelo, el shader y la relacion de aspect
	TR_Shader* shader{};
	float aspect{};
	bool is3D{};
	unsigned int VAO{}, VBO{};
	// El color, por defecto es el amarillo
	_Color color{WHITE};
    Vec3 dir{0.0f, 0.0f, 0.0f};
	Vec3 pos{0.0f, 0.0f, 0.0f};
};