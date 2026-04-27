#include "SceneTree.hpp"

// Anade el hijo pasado como parametro al nodo actual y devuelve la cantidad de hijos
uint32_t Node::addChild(Node* newNode) {
    // Construye un nuevo nodo al final del vector
    childs.emplace_back(newNode);
    return static_cast<uint32_t>(childs.size());
}

// Elimina del array de hijos el hijo pasado como parametro
// y devuelve si lo ha borrado
bool Node::removeChild(Node* remNode) {
    // Itera para comprobar si el nodo es igual al recibido por parámetro
    auto it = std::find(childs.begin(), childs.end(), remNode);
    if (it != childs.end()) {
        // Si lo es, lo borra y devuelve donde estaba
        childs.erase(it);
        return true;
    }
    return false;
}

// Asigna una entidad y devuelve true o false si lo ha hecho o no
bool Node::setEntity(E_Entity* newEntity) {
    bool res{};
    if (newEntity != nullptr && entity == nullptr) {
        entity = std::move(newEntity);
        res = true;
    }
    return res;
}

// Actualiza la matriz de transformacion con la funcion translate de GLM y marca que lo ha actualizado
void Node::translate(glm::vec3 toTranslate) {
    transMatrix = glm::translate(transMatrix, glm::vec3(toTranslate.x, toTranslate.y, toTranslate.z));
}

// Actualiza la matriz de transformacion con la funcion rotate de GLM y marca que lo ha actualizado
void Node::rotate(glm::vec4 toRotate) {
    // Aquí aprovecho el cuarto elemento del vector para el ángulo, pero si quieres puede separarlo
    transMatrix = glm::rotate(transMatrix, glm::radians(toRotate.w), glm::vec3{ toRotate.x, toRotate.y, toRotate.z });
}

// Actualiza la matriz de transformacion con la funcion scale de GLM y marca que lo ha actualizado
void Node::scale(glm::vec3 toScale) {
    transMatrix = glm::scale(transMatrix, glm::vec3(toScale.x, toScale.y, toScale.z));
}

void Node::lookAt(glm::vec3 target, glm::vec4 baseRotation, float yawOffset) {
    glm::vec3 position = glm::vec3(transMatrix[3]);
    glm::vec3 scale = glm::vec3(
        glm::length(glm::vec3(transMatrix[0])),
        glm::length(glm::vec3(transMatrix[1])),
        glm::length(glm::vec3(transMatrix[2]))
    );
    glm::vec3 direction = glm::normalize(target - position);
    float yaw = atan2(direction.x, direction.z) + glm::radians(yawOffset);
    float pitch = asin(direction.y);
    transMatrix = glm::mat4(1.0f);
    transMatrix = glm::translate(transMatrix, position);
    transMatrix = glm::rotate(transMatrix, yaw, glm::vec3(0.0f, 1.0f, 0.0f));
    transMatrix = glm::rotate(transMatrix, pitch, glm::vec3(1.0f, 0.0f, 0.0f));
    transMatrix = glm::rotate(transMatrix, glm::radians(baseRotation.w),
        glm::vec3(baseRotation.x, baseRotation.y, baseRotation.z));
    transMatrix = glm::scale(transMatrix, scale);
}

// Recorrido del arbol
void Node::traversal(glm::mat4 parentMatrix, E_Camera* prCamera) {
    // Le pasa la matriz del padre y recalcula la matriz de transformación, volviéndose la acumulada
    glm::mat4 cumulativeMatrix = parentMatrix * transMatrix;

    // Si tiene entidad y esta entidad tiene un puntero a un ResourceManager que existe, 
    // le manda a dibujar. Esto se hace para evitar dibujar las camaras aqui.
    if (entity && entity->RM && !entity->skybox)
        entity->draw(cumulativeMatrix, prCamera);
    // Recursivamente llamamos a los hijos
    for (auto& c : childs)
        c->traversal(cumulativeMatrix, prCamera);
}