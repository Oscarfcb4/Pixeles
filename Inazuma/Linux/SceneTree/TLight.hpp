#pragma once

struct TCamera;

// Los distintos tipos de luces soportadas por el motor
enum LightType{
    directional, pointLight, spotLight
};

// Clase luz
struct TLight: virtual public TEntity{
    // Constructor por defecto por si solo quiere instanciar una luz
    TLight() = default;
    // Constructor con un shader para inicializar el shader
    TLight(_Shader* shader_){shader = shader_;};

    // Sete una luz direccional con el tipo
    void setDirectionalLight(Vec3 dir, Vec3 amb, Vec3 dif, Vec3 spc){
        type = LightType::directional;
        direction = dir;
        ambient = amb;
        diffuse = dif;
        specular = spc;
    }

    // Sete una luz puntual con el tipo
    void setPointLight(Vec3 pos, Vec3 amb, Vec3 dif, Vec3 spc, Vec3 att){
        type = LightType::pointLight;
        position = pos;
        ambient = amb;
        diffuse = dif;
        specular = spc;
        attenuationValues = att;
    }

    // Sete una luz focal con el tipo
    void setSpotLight(Vec3 pos, Vec3 dir, Vec3 amb, Vec3 dif, Vec3 spc, Vec3 att, Vec2 cut){
        type = LightType::spotLight;
        position = pos;
        direction = dir;
        ambient = amb;
        diffuse = dif;
        specular = spc;
        attenuationValues = att;
        spotAngles = cut;
    }

    static constexpr glm::vec3 shadowDirectionalPos{6.8f, 15.0f, 4.7f};

    // Metodo de dibujado, en verdad no necesita ni la matriz ni la camara asi que se lo indico al compilador
    void draw([[maybe_unused]] glm::mat4 matrix = glm::mat4(1.0f), [[maybe_unused]] TCamera* principalCamera = nullptr) override{
        // Si hay un shader, hace el dibujado si no pues pa que
        if(shader){
            // Nos aseguramos que se use el shader
            shader->use();
            // Preparamos unos strings para el sistema de luces
            std::stringstream confg{};
            std::string light{};

            float near{1.0f}, far{100.0f};
            glm::mat4 lightProjection{}, lightView{}, lightSpaceMatrix{};
            std::vector<glm::mat4> shadowTransforms{};
            glm::vec3 lightPos{position.x, position.y, position.z};
            // Segun el tipo, manda una informacion al shader u otra. El vector de luces del shader basico tiene espacio para 10, por lo que la id
            // marca en que posicion de este esta.
            switch(type){
                case directional:
                    confg.str("");
                    light = "dLight[";
                    confg << light << id << "].direction";
                    shader->setVec3(confg.str(), glm::vec3(direction.x, direction.y, direction.z));
                    confg.str("");
                    confg << light << id << "].ambient";
                    shader->setVec3(confg.str(), ambient.x, ambient.y, ambient.z);
                    confg.str("");
                    confg << light << id << "].diffuse";
                    shader->setVec3(confg.str(), diffuse.x, diffuse.y, diffuse.z);
                    confg.str("");
                    confg << light << id << "].specular";
                    shader->setVec3(confg.str(), specular.x, specular.y, specular.z);

                    confg.str("");
                    confg << light << id << "].lightSpaceMatrix";
                    lightProjection = glm::ortho(-30.0f, 30.0f, -30.0f, 30.0f, near, far);
                    lightView = glm::lookAt(shadowDirectionalPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                    lightSpaceMatrix = lightProjection * lightView;
                    shader->setMat4(confg.str(), lightSpaceMatrix);
                    confg.str("");
                    confg << light << id << "].lightPos";
                    shader->setVec3(confg.str(), shadowDirectionalPos);
                    break;
                case pointLight:
                    confg.str("");
                    light = "pLight[";
                    confg << light << id << "].position";
                    shader->setVec3(confg.str(), glm::vec3(position.x, position.y, position.z));
                    confg.str("");
                    confg << light << id << "].ambient";
                    shader->setVec3(confg.str(), ambient.x, ambient.y, ambient.z);
                    confg.str("");
                    confg << light << id << "].diffuse";
                    shader->setVec3(confg.str(), diffuse.x, diffuse.y, diffuse.z);
                    confg.str("");
                    confg << light << id << "].specular";
                    shader->setVec3(confg.str(), specular.x, specular.y, specular.z);
                    confg.str("");
                    confg << light << id << "].constant";
                    shader->setFloat(confg.str(), attenuationValues.x);
                    confg.str("");
                    confg << light << id << "].linear";
                    shader->setFloat(confg.str(), attenuationValues.y);
                    confg.str("");
                    confg << light << id << "].cuadratic";
                    shader->setFloat(confg.str(), attenuationValues.z);
                    confg.str("");
                    confg << light << id << "].farPlane";
                    shader->setFloat(confg.str(), far);
                    break;
                case spotLight:
                    confg.str("");
                    light = "sLight[";
                    confg << light << id << "].position";
                    shader->setVec3(confg.str(), glm::vec3(position.x, position.y, position.z));
                    confg.str("");
                    confg << light << id << "].direction";
                    shader->setVec3(confg.str(), glm::vec3(direction.x, direction.y, direction.z));
                    confg.str("");
                    confg << light << id << "].ambient";
                    shader->setVec3(confg.str(), ambient.x, ambient.y, ambient.z);
                    confg.str("");
                    confg << light << id << "].diffuse";
                    shader->setVec3(confg.str(), diffuse.x, diffuse.y, diffuse.z);
                    confg.str("");
                    confg << light << id << "].specular";
                    shader->setVec3(confg.str(), specular.x, specular.y, specular.z);
                    confg.str("");
                    confg << light << id << "].constant";
                    shader->setFloat(confg.str(), attenuationValues.x);
                    confg.str("");
                    confg << light << id << "].linear";
                    shader->setFloat(confg.str(), attenuationValues.y);
                    confg.str("");
                    confg << light << id << "].cuadratic";
                    shader->setFloat(confg.str(), attenuationValues.z);
                    confg.str("");
                    confg << light << id << "].cutOff";
                    shader->setFloat(confg.str(), spotAngles.x);
                    confg.str("");
                    confg << light << id << "].outerCutOff";
                    shader->setFloat(confg.str(), spotAngles.y);
                    
                    confg.str("");
                    confg << light << id << "].lightSpaceMatrix";
                    lightProjection = glm::perspective(glm::radians(90.0f), (1280.0f/720.0f), near, far);
                    lightView = glm::lookAt(glm::vec3(position.x, position.y, position.z), glm::vec3{position.x+direction.x, position.y+direction.y, position.z+direction.z}, glm::vec3(0.0f, 1.0f, 0.0f));
                    lightSpaceMatrix = lightProjection * lightView;
                    shader->setMat4(confg.str(), lightSpaceMatrix);
                    confg.str("");
                    confg << light << id << "].lightPos";
                    shader->setVec3(confg.str(), glm::vec3(position.x, position.y, position.z));
                    break;
            }
        }
    };

    void drawShadowArray([[maybe_unused]]_Shader* shader, uint16_t& shadowCont, [[maybe_unused]] glm::mat4 matrix = glm::mat4(1.0f)) override {
        float near{1.0f}, far{100.0f};
        glm::mat4 lightProjection{}, lightView{}, lightSpaceMatrix{};
        std::vector<glm::mat4> shadowTransforms{};
        glm::vec3 lightPos{position.x, position.y, position.z};
        if(type == LightType::directional){
            lightProjection = glm::ortho(-30.0f, 30.0f, -30.0f, 30.0f, near, far);
            lightView = glm::lookAt(shadowDirectionalPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            lightSpaceMatrix = lightProjection * lightView;
            glBufferSubData(GL_UNIFORM_BUFFER, shadowCont * sizeof(glm::mat4x4), sizeof(glm::mat4x4), &lightSpaceMatrix);
            shadowCont += 1;
        }else if(type == LightType::spotLight){
            lightProjection = glm::perspective(glm::radians(90.0f), (1280.0f/720.0f), near, far);
            lightView = glm::lookAt(lightPos, glm::vec3{position.x+direction.x, position.y+direction.y, position.z+direction.z}, glm::vec3(0.0f, 1.0f, 0.0f));
            lightSpaceMatrix = lightProjection * lightView;
            glBufferSubData(GL_UNIFORM_BUFFER, shadowCont * sizeof(glm::mat4x4), sizeof(glm::mat4x4), &lightSpaceMatrix);
            shadowCont += 1;
        }
    };

    void drawShadowCubes([[maybe_unused]]_Shader* shader, [[maybe_unused]] uint16_t& shadowCont, [[maybe_unused]] glm::mat4 matrix = glm::mat4(1.0f)) override {
        if(type == LightType::pointLight){
            float near{1.0f}, far{100.0f};
            glm::mat4 lightProjection{};
            std::vector<glm::mat4> shadowTransforms{};
            glm::vec3 lightPos{position.x, position.y, position.z};

            lightProjection = glm::perspective(glm::radians(90.0f), static_cast<float>(1024)/static_cast<float>(1024), near, far);
            shadowTransforms.push_back(lightProjection * glm::lookAt(lightPos, lightPos + glm::vec3{ 1.0f, 0.0f, 0.0f}, glm::vec3{ 0.0f,-1.0f, 0.0f}));
            shadowTransforms.push_back(lightProjection * glm::lookAt(lightPos, lightPos + glm::vec3{-1.0f, 0.0f, 0.0f}, glm::vec3{ 0.0f,-1.0f, 0.0f}));
            shadowTransforms.push_back(lightProjection * glm::lookAt(lightPos, lightPos + glm::vec3{ 0.0f, 1.0f, 0.0f}, glm::vec3{ 0.0f, 0.0f, 1.0f}));
            shadowTransforms.push_back(lightProjection * glm::lookAt(lightPos, lightPos + glm::vec3{ 0.0f,-1.0f, 0.0f}, glm::vec3{ 0.0f, 0.0f,-1.0f}));
            shadowTransforms.push_back(lightProjection * glm::lookAt(lightPos, lightPos + glm::vec3{ 0.0f, 0.0f, 1.0f}, glm::vec3{ 0.0f,-1.0f, 0.0f}));
            shadowTransforms.push_back(lightProjection * glm::lookAt(lightPos, lightPos + glm::vec3{ 0.0f, 0.0f,-1.0f}, glm::vec3{ 0.0f,-1.0f, 0.0f}));

            for(unsigned int i{0}; i<6; ++i){
                shader->setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
            }
            
            shader->setFloat("farPlane", far);
            shader->setVec3("lightPos", lightPos);
        }
    };

    // El tipo de luz
    LightType type{};
    // Un puntero al shader
    _Shader* shader{};
    // La id de la luz en el shader, depende del numero de luces en el arbol. Me da igual que sea un int porque aunque
    // se meta negativo o se pase mal, OpenGL se lo tragara
    int id{};
    // Datos de las luces, la informacion es generica aunque solo se le daran uso a algunos dependiendo de la luz
    Vec3 position{};
    Vec3 direction{};
    Vec3 ambient{};
    Vec3 diffuse{};
    Vec3 specular{};
    // Constante, lineal y cuadratica
    Vec3 attenuationValues{};
    // El radio interior y exterior de la zona focal
    Vec2 spotAngles{};
};