#pragma once

struct ResourceManager;
struct E_Camera;

struct E_Entity{
    // Clase Draw que tienen heredaran todas su entidades
    virtual void draw(glm::mat4 matrix = glm::mat4(1.0f), E_Camera* principalCamera = nullptr) = 0;

    // Puntero al Resource Manager, solo lo tendran las entidades renderizables por el metodo de recorrido
    ResourceManager* RM{};
    bool skybox{};
};