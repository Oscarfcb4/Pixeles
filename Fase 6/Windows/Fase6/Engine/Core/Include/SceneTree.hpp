#pragma once
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>
#include <Shader.hpp>
#include <E_AllEntities.hpp>

// Clase nodo, con todo lo necesario para gestionar el arbol

struct Node{
    // Metodo para la gestion de los hijos 
    uint32_t addChild(Node* newNode);
    bool removeChild(Node* remNode);
    std::vector<Node*>& getChildren(){return childs;};
    Node* getDad(){return dad;};

    // Cada nodo puede tener un tipo de entidad asignada, estos metodos ayudan a gestionarlo
    bool setEntity(E_Entity* newEntity);
    E_Entity* getEntity(){return entity;};

    // Metodos para la transformacion del nodo
    void translate(glm::vec3 translate);
    void rotate(glm::vec4 rotation);
    void scale(glm::vec3 scaleM);
    void setTransMatrix(glm::mat4 transMatrix){this->transMatrix = transMatrix;};
    glm::mat4 getTransMatrix(){return transMatrix;};

    // Metodo que recorre el arbol y dibuja todos sus hijos teniendo en cuenta una camara, normalmente de uso exlusivo de la raiz
    void traversal(glm::mat4 parentMatrix, E_Camera* principalCamera);

    void lookAt(glm::vec3 target, glm::vec4 baseRotation, float yawOffset = 0.0f);

    // Metodo para recuperar y asignar una camara principal, de nuevo solo util en la raiz
    void setPrincipalCamera(E_Camera* newPCamera){principalCamera = newPCamera;};
    E_Camera* getPrincipalCamera(){return principalCamera;};

    private:
        // Almacenamiento de la estructura del arbol y sus entidades
        E_Entity* entity{};
        std::vector<Node*> childs{};
        Node* dad{};
        E_Camera* principalCamera{};
        // Su transformaci�n local (empieza por la identidad)
        glm::mat4 transMatrix{1.0f};
};
