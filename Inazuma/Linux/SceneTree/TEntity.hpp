#pragma once
#include "./sceneTree.hpp"

struct TResourceManager;
struct TCamera;
struct _Shader;
struct TNode;

struct TEntity{
    // Clase Draw que tienen heredaran todas su entidades
    virtual void draw(glm::mat4 matrix = glm::mat4(1.0f), TCamera* principalCamera = nullptr) = 0;
    virtual void drawShadowArray(_Shader* shader, uint16_t& shadowCont, glm::mat4 matrix = glm::mat4(1.0f)) = 0;
    virtual void drawShadowCubes(_Shader* shader, uint16_t& shadowCont, glm::mat4 matrix = glm::mat4(1.0f)) = 0;

    TNode* node{};

    // Vector al Resource Manager, solo lo tendran las entidades renderizables por el metodo de recorrido
    TResourceManager* RM{};
    bool skybox{};
};