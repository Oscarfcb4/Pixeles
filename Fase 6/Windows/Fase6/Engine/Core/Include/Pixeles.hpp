#pragma once
#include <GLAD/glad.h>
#include <GLFW/glfw3.h>
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <cstdint>
#include <Shader.hpp>
#include <ResourceManager.hpp>
#include <PixelesTypes.hpp>
#include <E_AllEntities.hpp>
#include <SceneTree.hpp>

constexpr unsigned int ANCHO{ 1280 };
constexpr unsigned int ALTO{ 720 };
constexpr const char* NOMBRE{ "Pixeles" };

struct Pixeles {
    // Constructor por defecto, por si el usuario quiere instanciar pero no empezar a correr el motor.
    Pixeles() {};
    // Constructo de arranque, con este ya te abre una ventana del motor grafico e inicia el arbol.
    Pixeles(uint16_t width, uint16_t height, std::string name);
    // El destructor por defecto y los operadores de copia y asignacion destruidos, importante para evitar
    // posibles fugas de memoria.
    ~Pixeles();
    Pixeles(const Pixeles&) = delete;
    Pixeles(Pixeles&&) = delete;
    Pixeles& operator=(const Pixeles&) = delete;
    Pixeles& operator=(Pixeles&&) = delete;

    // Si el usuario ha instanciado el motor con el constructor por defecto, esta funcion arrancara todo cuando el usuario quiera
    void startWindow(uint16_t width, uint16_t height, std::string name);
    void closeWindow();
    // Tu motor no tiene que ser celoso, si ya has abierto una ventana de otra manera, puedes iniciar este motor 
    // con el handler de la ventana de OpenGL que hayas inciado tú
    void setWindow(void* win, uint16_t width, uint16_t height);

    // Incio del renderizado, Asegura unas calidades minimas para que el renderizado mas basico sea visible
    void start();
    // Una vez has creado todo lo que querias renderizar, recorre el arbol y dibuja todo..
    void end();
    // Por si no quiere cambiar los buffers front y back, que no sé pa qué pero mejor dar opciones
    void endNoSwap();
    // Renderiza el arbol, no es mas que una llamada a start y end, pero asi el usuario puede controlar el renderizado o solo llamar a renderizar y olvidarse
    void render() { start(); end(); };
    // Limpia el arbol de la escena entero, permite el tratamiendo del arbol dentro de un bucle de renderizado.
    void clearScene();

    // Para controlar el bucle de renderizado sin necesidad de llamar directamente a GLFW
    bool rendering() { return !glfwWindowShouldClose(mainWindow); };
    void stopRendering(bool cierre) { glfwSetWindowShouldClose(mainWindow, cierre); };

    // La función de limpiar el fondo
    void cleanBackground(Color c);

    Shader* createShader(std::string name, std::string vs, std::string fs);

    // Metodos de creacion de entidades renderizables, con valores por defecto para las versiones "default"
    E_Camera* createCamera(
        Vec3 position = Vec3(0.0f, 0.0f, 0.0f),
        Vec3 up = Vec3(0.0f, 1.0f, 0.0f),
        float yaw = -90.0f,
        float pitch = 0.0f
    );

    E_Light* createDirectionalLight(
        Vec3 amb = Vec3(0.2f, 0.2f, 0.2f),
        Vec3 dir = Vec3(-0.2f, -1.0f, -0.3f),
        Vec3 dif = Vec3(0.3f, 0.3f, 0.3f),
        Vec3 spc = Vec3(1.0f, 1.0f, 1.0f),
        std::string shader = "default"
    );

    E_Light* createPointLight(
        Vec3 pos = Vec3(1.2f, 1.0f, 2.0f),
        Vec3 amb = Vec3(0.2f, 0.2f, 0.2f),
        Vec3 dif = Vec3(0.5f, 0.5f, 0.5f),
        Vec3 spc = Vec3(1.0f, 1.0f, 1.0f),
        Vec3 att = Vec3(1.0f, 0.09f, 0.032f),
        std::string shader = "default"
    );

    E_Light* createSpotLight(
        Vec3 pos = Vec3(0.0f, 0.0f, 0.0f),
        Vec3 dir = Vec3(-4.3f, 0.0f, -1.0f),
        Vec3 amb = Vec3(0.0f, 0.0f, 0.0f),
        Vec3 dif = Vec3(1.0f, 1.0f, 1.0f),
        Vec3 spc = Vec3(1.0f, 1.0f, 1.0f),
        Vec3 att = Vec3(1.0f, 0.09f, 0.032f),
        Vec2 cut = Vec2(12.5f, 15.0f),
        std::string shader = "default"
    );

    E_Model* createModel(
        std::string path,
        Vec3 pos = Vec3(0.0f, 0.0f, 0.0f),
        Vec3 size = Vec3(1.0f, 1.0f, 1.0f),
        float rotAngle = 0.0f,
        Vec3 rotAxis = Vec3(0.0f, 1.0f, 0.0f),
        std::string shader = "default"
    );

    E_Model* createCube(
        Vec3 pos = Vec3(0.0f, 0.0f, 0.0f),
        Vec3 size = Vec3(1.0f, 1.0f, 1.0f),
        Color color = YELLOW,
        float rotAngle = 0.0f,
        Vec3 rotAxis = Vec3(0.0f, 1.0f, 0.0f),
        bool texture = false
    );

    E_Skybox* createSkybox(std::vector<std::string> faces);

    void setTexture(E_Model* image, std::string path);
    void setTexture(Node* image, std::string path);

    // Getters para los valores privados 
    GLFWwindow* getMainWindow() { return mainWindow; };
    Shader* getDefaultShader() { return defaultShader; };
    Shader* getSkyboxShader() { return skyboxShader; };
    E_Camera* getPrincipalCamera() { return principalCamera; };
    // Si quiere acceder al árbol se lo permitimos, pero es opcional completamente
    Node* getSceneRoot() { return sceneRoot.get(); };
    uint16_t getScreenWidth() { return SCREEN_WIDTH; };
    uint16_t getScreenHeight() { return SCREEN_WIDTH; };

    // Funcionalidades varias como si hay pulsada una tecla, recuperar el tiempo del programa...
    // Cuando mejores tu motor podrás ir añadiendolas
    bool isKeyPressed(uint16_t key);
    float getTime() { return static_cast<float>(glfwGetTime()); };
    void switchFullscreen();

private:
    // Funcion interna para el dibujado 3D, ayuda a encapsular el renderizado
    // básicamente recorre el árbol e informa los uniform
    void draw3D();

    // Carga el espacio en OpenGL del Skybox
    void setupSkybox();

    // Carga los shaders default
    void loadDefaultShaders();

    // Arranca la ventana de OpenGL y todo lo necesario para nuestro motor
    GLFWwindow* setupOpenGL(uint16_t width, uint16_t height, std::string name);

    // Carga las imágenes de un skybox con OpenGL
    void loadSkybox(std::vector<std::string> faces);

    // Métodos para pasar de nuestras clases propias a GLM
    glm::vec2 parseGLM(Vec2 vector) { return glm::vec2(vector.x, vector.y); }
    glm::vec3 parseGLM(Vec3 vector) { return glm::vec3(vector.x, vector.y, vector.z); }
    glm::vec4 parseGLM(Vec4 vector) { return glm::vec4(vector.x, vector.y, vector.z, vector.w); }

    // El puntero unico de la raiz del nodo
    std::unique_ptr<Node> sceneRoot{};
    // Punteros al shader basico, a la camara principal y a la ventana principal
    Shader* defaultShader{};
    Shader* skyboxShader{};
    E_Camera* principalCamera{};
    GLFWwindow* mainWindow{};
    // Instancia del gestor de recursos
    ResourceManager RM{};
    // Vectores para almacenar los nodos, necesario para gestionar y encapsular las responsabilidades de la destruccion de la memoria
    std::vector<std::unique_ptr<Node>> nodes{};
    std::vector<std::unique_ptr<E_Camera>> cameras{};
    std::vector<std::unique_ptr<E_Light>> lights{};
    std::vector<std::unique_ptr<E_Model>> models{};
    std::unique_ptr<E_Skybox> skyboxModel{};
    // Numero de luces
    // X -> Luces direcionales
    // Y -> Luces puntuales
    // Z -> Luces especulares
    glm::vec3 numLights{};
    // Booleano para ver si esta activo el motor o no
    bool active{}, skyLoaded{};
    uint16_t SCREEN_WIDTH{}, SCREEN_HEIGHT{};
    unsigned int skybox{};
};