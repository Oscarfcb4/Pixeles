#pragma once

// Para mayor usabilidad y permitir que el movimiento sea igual en todos los dispositivos, ponemos a 
// disposicion del usuario unos movimientos genericos que funcionan con el deltatime que nos pasen
enum CameraMovement{
    FORWARD, BACKWARD, LEFT, RIGHT
};

struct E_Camera: virtual public E_Entity{
    // Constructor de la camara, inicializa todo lo necesario para que la camara 3D pueda usarse para renderizar
    E_Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), 
        float yaw = -90.0f, float pitch = 0.0f)
        : front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(2.0f), mouseSensitivity(0.1f), zoom(45.0f){
        this->position = position;
        // El up de verdad lo calulo después, pero le llamo up en el parámetro porque va a 
        // ser más fácil de entender así para el usuario, nos aseguramos de que esté normalizado
        this->worldUp = glm::normalize(up);
        this->yaw = yaw;
        this->pitch = pitch;
        updateCameraVectors();
    };

    // Metodo que calcula la matriz vista usando la matriz lookAt
    glm::mat4 getViewMatrix() const{
        // Le pasamos la posición, punto al que mira y el vector up. Con esto GLM construye la matriz.
        return glm::lookAt(position, position + front, up);
    };

    // Si el usuario cambia el Front o el Up de la camara, este metodo actualizara el resto de datos
    void changed(){
        updateCameraVectors();
    }

    // Posiciona la camara mirando a una posicion en especifico. Nos aseguramos de normalizar después el vector
    void toTarget(glm::vec3 target){
        front = glm::normalize(glm::vec3(target.x - position.x, target.y - position.y, target.z - position.z));
    }

    // Aqui el usuario podra mover la camara segun un delta time, asegurando un movimiento estable respecto al framerate
    void processKeyboard(CameraMovement direction, float deltaTime){
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

    // Si el usuario quiere que la camara se mueva con el raton, este metodo se lo permitira
    void processMouseMovement(float xoffset, float yoffset){
        // Multiplicamos los offsets (el movimiento puro del ratón) por la sensibilidad del 
        // raton para que el movimiento sea mas lento o mas rapido
        xoffset *= mouseSensitivity;
        yoffset *= mouseSensitivity;
        // Añadimos los offsets a los angulos de Euler
        yaw   += xoffset;
        pitch += yoffset;

        // Limitamos el pitch para evitar el gimbal lock
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        // Actualizamos los vectores de la camara con los nuevos angulos
        updateCameraVectors();
    };

    // Este metodo permitira al usuario controlar el zoom modificando el FOV de la camara con la rueda del raton
    void processMouseScroll(float yoffset){
        // Restamos el offset al zoom (si el offset es positivo, restamos y si es negativo, sumamos)
        // porque es lo que espera el usuario (hacia abajo = alejar, hacia arriba = acercar)
        zoom -= static_cast<float>(yoffset);
        // Limitamos el zoom minimo a 1 grado para que no se de la vuelta la matriz de proyeccion
        if (zoom < 1.0f)
            zoom = 1.0f;
        // Limitamos el zoom maximo a 100 grados
        if (zoom > 100.0f)
            zoom = 100.0f;
    };

    // Metodo Draw obligatorio por ser una entidad, pero una camara no lo necesita
    void draw(glm::mat4 matrix = glm::mat4(1.0f), E_Camera* principalCamera = nullptr) override{};

    // Datos de la camara
    // La posicion en el mundo
    glm::vec3 position{};
    // Vector que apunta hacia donde mira la camara
    glm::vec3 front{};
    // Vector que apunta hacia arriba desde la camara
    glm::vec3 up{};
    // Vector que apunta hacia la derecha desde la camara
    glm::vec3 right{};
    // Vector que apunta hacia arriba desde la camara en respecto al mundo
    glm::vec3 worldUp{};
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
            // Variable auxiliar para calcular el vector front antes de normalizarlo
            glm::vec3 frontVec{};

            // Calculamos la componente X, Y y Z del vector front usando los angulos yaw y pitch
            frontVec.x = static_cast<float>(cos(glm::radians(yaw)) * cos(glm::radians(pitch)));
            frontVec.y = static_cast<float>(sin(glm::radians(pitch)));
            frontVec.z = static_cast<float>(sin(glm::radians(yaw)) * cos(glm::radians(pitch)));

            // Normalizamos el vector front para que tenga longitud 1,
            // esto es necesario porque los vectores deben ser vectores
            // unitarios de dirección para usarse con LookAt
            front = glm::normalize(frontVec);

            // Calculamos el vector right haciendo producto cruz entre front y worldUp, esto nos da un 
            // vector perpendicular a ambos que apunta a la derecha de la camara.
            right = glm::normalize(glm::cross(front, worldUp));

            // Calculamos el vector up haciendo producto cruz entre right y front, esto nos da un vector 
            // perpendicular a ambos que apunta hacia arriba desde la perspectiva de la camara
            up = glm::normalize(glm::cross(right, front));
        };
};