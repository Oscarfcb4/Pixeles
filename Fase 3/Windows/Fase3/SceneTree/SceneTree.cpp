#include "sceneTree.hpp"

// Anade el hijo pasado como parametro al nodo actual y devuelve la cantidad de hijos
uint32_t TNode::addChild(TNode* newNode) {
    childs.emplace_back();
    childs[childs.size() - 1] = std::move(newNode);
    return static_cast<uint32_t>(childs.size());
}

// Elimina del array de hhijos el hijo pasado como parametro al nodo actual y devuelve la posicion donde estaba y -1 si no ha borrado
uint32_t TNode::removeChild(TNode* remNode) {
    // Comprueba por cada hijo si el puntero a entidad es igual
    for (uint32_t i{}; i <= childs.size(); i++) {
        if (childs[i]->getEntity() == remNode->getEntity()) {
            childs.erase(childs.begin() + i);
            return i;
        }
    }
    return -1;
}

// Asigna una entidad y devuelve true o false si lo ha hecho o no
bool TNode::setEntity(TEntity* newEntity) {
    bool res{};
    if (newEntity != nullptr && entity == nullptr) {
        entity = std::move(newEntity);
        res = true;
    }
    return res;
}

// Actualiza la matriz de transformacion con la funcion translate de GLM y marca que lo ha actualizado
void TNode::translate(glm::vec3 toTranslate) {
    transMatrix = glm::translate(transMatrix, glm::vec3(toTranslate.x, toTranslate.y, toTranslate.z));
    updated = true;
}

// Actualiza la matriz de transformacion con la funcion rotate de GLM y marca que lo ha actualizado
void TNode::rotate(glm::vec4 toRotate) {
    transMatrix = glm::rotate(transMatrix, glm::radians(toRotate.w), glm::vec3{ toRotate.x, toRotate.y, toRotate.z });
    updated = true;
}

// Actualiza la matriz de transformacion con la funcion scale de GLM y marca que lo ha actualizado
void TNode::scale(glm::vec3 toScale) {
    transMatrix = glm::scale(transMatrix, glm::vec3(toScale.x, toScale.y, toScale.z));
    updated = true;
}

// Recorrido del arbol
void TNode::traversal(glm::mat4 cumulativeMatrix, TCamera* prCamera) {
    // Si se ha actualizado, le pasa la matriz acumulada y recalcula la matriz de transformacion
    if (updated) {
        transMatrix = cumulativeMatrix * transMatrix;
        updated = false;
    }
    // Si tiene entidad y esta entidad tiene un puntero a un ResourceManager que existe, le manda a dibujar. Esto se hace
    // para evitar dibujar las luces y las camaras aqui.
    if (entity && entity->RM)
        entity->draw(transMatrix, prCamera);
    // Recursivamente llamamos a los hijos
    for (auto& c : childs)
        c->traversal(transMatrix, prCamera);
}