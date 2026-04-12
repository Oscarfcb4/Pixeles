#pragma once

struct TResourceManager;
struct TCamera;

struct TEntity{
    // Clase Draw que tienen heredaran todas su entidades
    virtual void draw(glm::mat4 matrix = glm::mat4(1.0f), TCamera* principalCamera = nullptr) = 0;

    // Vector al Resource Manager, solo lo tendran las entidades renderizables por el metodo de recorrido
    TResourceManager* RM{};
};