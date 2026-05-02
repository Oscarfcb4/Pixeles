#pragma once
#include <GLM/glm.hpp>

struct E_Camera;

// Los distintos tipos de luces soportadas por el motor
enum LightType {
    directional, pointLight, spotLight
};

// Clase luz
struct E_Light : virtual public E_Entity {
    // Constructor por defecto por si solo quiere instanciar una luz
    E_Light() = default;
    // Constructor con un shader para inicializar el shader
    E_Light(Shader* shader_) { shader = shader_; };

    // Sete una luz direccional con el tipo
    void setDirectionalLight(glm::vec3 dir, glm::vec3 amb, glm::vec3 dif, glm::vec3 spc) {
        type = LightType::directional;
        direction = dir;
        ambient = amb;
        diffuse = dif;
        specular = spc;
    }

    // Sete una luz puntual con el tipo
    void setPointLight(glm::vec3 pos, glm::vec3 amb, glm::vec3 dif, glm::vec3 spc, glm::vec3 att) {
        type = LightType::pointLight;
        position = pos;
        ambient = amb;
        diffuse = dif;
        specular = spc;
        attenuationValues = att;
    }

    // Sete una luz focal con el tipo
    void setSpotLight(glm::vec3 pos, glm::vec3 dir, glm::vec3 amb, glm::vec3 dif, glm::vec3 spc, glm::vec3 att, glm::vec2 cut) {
        type = LightType::spotLight;
        position = pos;
        direction = dir;
        ambient = amb;
        diffuse = dif;
        specular = spc;
        attenuationValues = att;
        spotAngles = cut;
    }

    // Metodo de dibujado, en verdad no necesita ni la matriz acumulada ni la camara en este caso
    void draw(glm::mat4 matrix = glm::mat4(1.0f), E_Camera* principalCamera = nullptr) override {
        // Si hay un shader, hace el dibujado si no pues pa que
        if (shader) {
            // Nos aseguramos que se use el shader
            shader->use();
            // Preparamos las cadenas para el sistema de nombres dinámico
            std::stringstream confg{};
            std::string light{};
            // Segun el tipo, manda una informacion al shader u otra.
            switch (type) {
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
                break;
            }
        }
    };

    Vec3 getPosition() { return Vec3(position); };
    Vec3 getDirection() { return Vec3(direction); };
    Vec3 getAmbient() { return Vec3(ambient); };
    Vec3 getDiffuse() { return Vec3(diffuse); };
    Vec3 getSpecular() { return Vec3(specular); };

    void setPosition(Vec3 position) { this->position = glm::vec3(position.x, position.y, position.z); };
    void setDirection(Vec3 direction) { this->direction = glm::vec3(direction.x, direction.y, direction.z); };
    void setAmbient(Vec3 ambient) { this->ambient = glm::vec3(ambient.x, ambient.y, ambient.z); };
    void setDiffuse(Vec3 diffuse) { this->diffuse = glm::vec3(diffuse.x, diffuse.y, diffuse.z); };
    void setSpecular(Vec3 specular) { this->specular = glm::vec3(specular.x, specular.y, specular.z); };

    // El tipo de luz
    LightType type{};
    // Un puntero al shader
    Shader* shader{};
    // La id de la luz en el shader, depende del numero de luces en el arbol.
    int id{};
    // Datos de las luces, la informacion es generica aunque solo se le daran uso a algunos dependiendo de la luz
    glm::vec3 position{};
    glm::vec3 direction{};
    glm::vec3 ambient{};
    glm::vec3 diffuse{};
    glm::vec3 specular{};
    // Constante, lineal y cuadratica
    glm::vec3 attenuationValues{};
    // El radio interior y exterior de la zona focal
    glm::vec2 spotAngles{};
};