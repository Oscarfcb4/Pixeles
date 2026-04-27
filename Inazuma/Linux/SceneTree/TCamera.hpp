#pragma once

// Para mayor usabilidad y permitir que el movimiento sea igual en todos los dispositivos, ponemos a disposicion del usuario unos movimientos
// genericos que funcionan con el deltatime del mismo
enum cameraMovement{
    FORWARD, BACKWARD, LEFT, RIGHT
};

struct TCamera: virtual public TEntity{
    // Constructor de la camara, inicializa todo lo necesario para que la camara 3D pueda usarse para renderizar
    TCamera(Vec3 position_ = Vec3(0.0f, 0.0f, 0.0f), Vec3 up_ = Vec3(0.0f, 1.0f, 0.0f), float yaw_ = -90.0f, float pitch_ = 0.0f)
        : front(Vec3(0.0f, 0.0f, -1.0f)), movementSpeed(0.1f), mouseSensitivity(0.1f), zoom(45.0f){
        position = position_;
        worldUp = up_;
        yaw = yaw_;
        pitch = pitch_;
        updateCameraVectors();
    };

    // Metodo que calcula la matriz vista usando el metodo lookAt
    glm::mat4 getViewMatrix(){
        glm::vec3 pos{position.x, position.y, position.z};
        Vec3 aux{position + front};
        glm::vec3 right{aux.x, aux.y, aux.z};
        glm::vec3 up_{up.x, up.y, up.z};
        return glm::lookAt(pos, right, up_);
    };

    // Si el usuario cambia el Front o el Up de la camara, este metodo actualizara el resto de datos
    void changed(){
        updateCameraVectors();
    }

    // Posiciona la camara mirando a una posicion en especifico
    void toTarget(Vec3 target){
        front = Vec3(target.x - position.x, target.y - position.y, target.z - position.z);
    }

    // Aqui el usuario podra mover la camara segun un delta time, asegurando un movimiento estable respecto al framerate
    void processKeyboard(cameraMovement direction, float deltaTime){
        float velocity{movementSpeed * deltaTime};
        if (direction == FORWARD)
            position += front * velocity;
        if (direction == BACKWARD)
            position -= front * velocity;
        if (direction == LEFT)
            position -= right * velocity;
        if (direction == RIGHT)
            position += right * velocity;
    };

    // Metodos de usabilidad al usuario. Son un poco especificos pero mira, ya que aprendi a hacerlos los implemento.

    // Si el usuario quiere que la camara se mueva con el raton, este metodo se lo permitira
    void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true){
        xoffset *= mouseSensitivity;
        yoffset *= mouseSensitivity;
        yaw   += xoffset;
        pitch += yoffset;

        if (constrainPitch){
            if (pitch > 89.0f)
                pitch = 89.0f;
            if (pitch < -89.0f)
                pitch = -89.0f;
        }

        updateCameraVectors();
    };

    // Este metodo permitira al usuario el controlar el zoom de la camara con la rueda del raton
    void processMouseScroll(float yoffset){
        zoom -= static_cast<float>(yoffset);
        if (zoom < 1.0f)
            zoom = 1.0f;
        if (zoom > 45.0f)
            zoom = 45.0f;
    };

    // Metodo Draw obligatorio por ser una entidad, pero una camara no lo necesita
    void draw([[maybe_unused]] glm::mat4 matrix = glm::mat4(1.0f), [[maybe_unused]] TCamera* principalCamera = nullptr) override{};
    void drawShadowArray([[maybe_unused]] _Shader* shader, [[maybe_unused]] uint16_t& shadowCont, [[maybe_unused]] glm::mat4 matrix = glm::mat4(1.0f)){};
    void drawShadowCubes([[maybe_unused]] _Shader* shader, [[maybe_unused]] uint16_t& shadowCont, [[maybe_unused]] glm::mat4 matrix = glm::mat4(1.0f)){};

    // Datos de la camara
    // La posicion en el mundo
    Vec3 position{};
    // Vector que apunta hacia donde mira la camara
    Vec3 front{};
    // Vector que apunta hacia arriba desde la camara
    Vec3 up{};
    // Vector que apunta hacia la derecha desde la camara
    Vec3 right{};
    // Vector que apunta hacia arriba desde la camara en respecto al mundo
    Vec3 worldUp{};
    // Valores de Euler
    float yaw{};
    float pitch{};
    // Velocidad de movimiento, sensibilidad del raton y zoom (fov)
    float movementSpeed{};
    float mouseSensitivity{};
    float zoom{};

    private:
        // Actualiza los valores de up y right segun los valores de Euler y el front
        void updateCameraVectors(){
            glm::vec3 frontVec{};
            frontVec.x = static_cast<float>(cos(glm::radians(yaw)) * cos(glm::radians(pitch)));
            frontVec.y = static_cast<float>(sin(glm::radians(pitch)));
            frontVec.z = static_cast<float>(sin(glm::radians(yaw)) * cos(glm::radians(pitch)));

            glm::vec3 aux{glm::normalize(frontVec)};
            front = Vec3(aux.x, aux.y, aux.z);
            glm::vec3 aux2{glm::normalize(glm::cross(aux, glm::vec3(worldUp.x, worldUp.y, worldUp.z)))};
            right = Vec3(aux2.x, aux2.y, aux2.z);
            aux = glm::normalize(glm::cross(aux2, aux));
            up = Vec3(aux.x, aux.y, aux.z);
        };
};