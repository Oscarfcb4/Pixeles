#pragma once
#include <GLM/glm.hpp>
#include <PixelesTypes.hpp>

struct ResourceManager;
struct E_Camera;

struct E_Entity{
    // Clase Draw que tienen heredaran todas su entidades
    virtual void draw(glm::mat4 matrix = glm::mat4(1.0f), E_Camera* principalCamera = nullptr) = 0;

    Node* node{};

    // Actualiza la matriz de transformacion con la funcion translate de GLM y marca que lo ha actualizado
    void translate(Vec3 toTranslate);

    // Actualiza la matriz de transformacion con la funcion rotate de GLM y marca que lo ha actualizado
    void rotate(Vec4 toRotate);

    // Actualiza la matriz de transformacion con la funcion scale de GLM y marca que lo ha actualizado
    void scale(Vec3 toScale);

    // Método que rota el nodo hacia el objetivo, extrayendo la posicion y escala de la matriz actual
    void lookAt(Vec3 target, Vec4 baseRotation, float yawOffset);

    // Puntero al Resource Manager, solo lo tendran las entidades renderizables por el metodo de recorrido
    ResourceManager* RM{};
    bool skybox{};
};