#pragma once
#include "../InazumaTypes.hpp"
#include "../Shaders/shader.hpp"
#include "../ResourceManager/TResourceManager.hpp"
#include "TEntities.hpp"

// Clase nodo, con todo lo necesario para gestionar el arbol

struct TNode{
    // Metodo para la gestion de los hijos 
    uint32_t addChild(TNode* newNode);
    uint32_t removeChild(TNode* remNode);
    std::vector<TNode*>& getChildren(){return childs;};
    TNode* getDad(){return dad;};

    // Cada nodo puede tener un tipo de entidad asignada, estos metodos ayudan a gestionarlo
    bool setEntity(TEntity* newEntity);
    TEntity* getEntity(){return entity;};

    // Metodos para la transformacion del nodo
    void translate(Vec3 translate);
    void rotate(Vec4 rotation);
    void scale(Vec3 scaleM);
    void setTransMatrix(glm::mat4 transMatrix_){transMatrix = transMatrix_;};
    glm::mat4 getTransMatrix(){return transMatrix;};

    // Metodo que recorre el arbol y dibuja todos sus hijos teniendo en cuenta una camara, normalmente de uso exlusivo de la raiz
    void traversal(glm::mat4 cumulativeMatrix, TCamera* prCamera);

    // Metodo para recuperar y asignar una camara principal, de nuevo solo util en la raiz
    void setPrincipalCamera(TCamera* newPCamera){pCamera = newPCamera;};
    TCamera* getPrincipalCamera(){return pCamera;};

    private:
        // Almacenamiento de la estructura del arbol y sus entidades
        TEntity* entity{};
        std::vector<TNode*> childs{};
        TNode* dad{};
        TCamera* pCamera{};
        glm::mat4 transMatrix{1.0f};
        // Solo hace el calculo de la matriz acumulada si este nodo ha sido cambiado
        bool updated{};
};
