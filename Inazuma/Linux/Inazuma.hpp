#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <cstdint>
#include <memory>
#include <iostream>
#include <vector>
#include "./SceneTree/sceneTree.hpp"
#include "./Animator/animator.hpp"
#include "InazumaTypes.hpp"

// Bienvenido al motor grafico Inazuma, es pequenito pero peleon. Ya veras.

struct character{
    unsigned int TextureID{}, Advance{};
    glm::ivec2   Size{}, Bearing{}; 
};

struct textToRender{
    std::string text{};
    float x{}, y{}, scale{};
    _Color color{};
};


struct Inazuma{
    // Constructor por defecto, por si el usuario quiere instanciar pero no empezar a correr el motor.
    Inazuma(){};
    // Constructo de arranque, con este ya te abre una ventana del motor grafico e inicia el arbol.
    Inazuma(uint16_t width, uint16_t height, std::string name);
    // El destructor por defecto y los operadores de copia y asignacion destruidos, importante para evitar
    // posibles fugas de memoria.
    ~Inazuma() = default;
    Inazuma(const Inazuma&) = delete;
    Inazuma(Inazuma&&) = delete;
    Inazuma& operator=(const Inazuma&) = delete;
    Inazuma& operator=(Inazuma&&) = delete;

    // Si el usuario ha instanciado el motor con el constructor por defecto, esta funcion arrancara todo cuando el usuario quiera
    void startWindow(uint16_t width, uint16_t height, std::string name);
    void closeWindow();
    // Inazuma no es celoso! Si ya has abierto una ventana con otro motor grafico, puedes iniciar este motor con el handler de la ventana
    // de OpenGL que haya inciado tu otro motor.
    void setWindow(void* win, uint16_t width, uint16_t height);

    // Incio del renderizado, Asegura unas calidades minimas para que el renderizado mas basico sea visible
    void start();
    // Una vez has creado todo lo que querias renderizar, recorre el arbol y dibuja todo..
    void end();
    void endNoSwap();
    // Renderiza el arbol, no es mas que una llamada a start y finish, pero asi el usuario puede controlar el renderizado o solo llamar a renderizar y olvidarse
    void render(){start(); end();};
    // Limpia el arbol de la escena entero, permite el tratamiendo del arbol dentro de un bucle de renderizado.
    void clearScene();

    void shadowRender();

    // Metodos de creacion de entidades renderizables, con valores por defecto para las versiones "base"
    TCamera* createCamera(
        Vec3 position_ = Vec3(0.0f, 0.0f, 0.0f), 
        Vec3 up_ = Vec3(0.0f, 1.0f, 0.0f), 
        float yaw_ = -90.0f, 
        float pitch_ = 0.0f
    );

    TLight* createDirectionalLight(
        Vec3 amb = Vec3( 0.2f,  0.2f,  0.2f),
        Vec3 dir = Vec3(-0.2f, -1.0f, -0.3f),
        Vec3 dif = Vec3( 0.3f,  0.3f,  0.3f),
        Vec3 spc = Vec3( 1.0f,  1.0f,  1.0f),
        std::string shader = "base"
    );

    TLight* createPointLight(
        Vec3 pos = Vec3(1.2f, 1.0f, 2.0f), 
        Vec3 amb = Vec3(0.2f, 0.2f, 0.2f), 
        Vec3 dif = Vec3(0.5f, 0.5f, 0.5f), 
        Vec3 spc = Vec3(1.0f, 1.0f, 1.0f), 
        Vec3 att = Vec3(1.0f, 0.09f, 0.032f),
        std::string shader = "base"
    );

    TLight* createSpotLight(
        Vec3 pos = Vec3(0.0f, 0.0f, 0.0f), 
        Vec3 dir = Vec3(-4.3f, 0.0f, -1.0f), 
        Vec3 amb = Vec3(0.0f, 0.0f, 0.0f), 
        Vec3 dif = Vec3(1.0f, 1.0f, 1.0f), 
        Vec3 spc = Vec3(1.0f, 1.0f, 1.0f), 
        Vec3 att = Vec3(1.0f, 0.09f, 0.032f), 
        Vec2 cut = Vec2(glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(15.0f))),
        std::string shader = "base"
    );

    TModel* createModel(
        std::string path, 
        Vec3 pos = Vec3(0.0f, 0.0f, 0.0f), 
        Vec3 size = Vec3(1.0f, 1.0f, 1.0f), 
        float rotAngle = 0.0f, 
        Vec3 rotAxis   = Vec3(0.0f, 1.0f, 0.0f),
        std::string shader = "base"
    );

    TAnimationModel* createAnimationModel(
        std::string path, 
        Vec3 pos = Vec3(0.0f, 0.0f, 0.0f), 
        Vec3 size = Vec3(1.0f, 1.0f, 1.0f), 
        float rotAngle = 0.0f, 
        Vec3 rotAxis   = Vec3(0.0f, 1.0f, 0.0f),
        std::string shader = "base"
    );
 
    TModel* createCube(
        Vec3 pos       = Vec3(0.0f, 0.0f, 0.0f), 
        Vec3 size      = Vec3(1.0f, 1.0f, 1.0f), 
        _Color color   = YELLOW, 
        float rotAngle = 0.0f, 
        Vec3 rotAxis   = Vec3(0.0f, 1.0f, 0.0f)
    );

    T2DElement* createRectangle(
        Vec2 pos       = Vec2(0.0f, 0.0f), 
        Vec2 size      = Vec2(1.0f, 1.0f), 
        _Color color   = YELLOW, 
        float rotAngle = 0.0f, 
        Vec3 rotAxis   = Vec3(0.0f, 1.0f, 0.0f)
    );

    T2DElement* createRectangleWired(
        Vec2 pos       = Vec2(0.0f, 0.0f), 
        Vec2 size      = Vec2(1.0f, 1.0f), 
        _Color color   = YELLOW, 
        float rotAngle = 0.0f, 
        Vec3 rotAxis   = Vec3(0.0f, 1.0f, 0.0f)
    );

    T2DElement* createImage(
        std::string path = "none",
        Vec2 pos = Vec2(0.0f, 0.0f), 
        Vec2 size = Vec2(1.0f, 1.0f), 
        float rotAngle = 0.0f, 
        Vec3 rotAxis = Vec3(0.0f, 1.0f, 0.0f)
    );

    T2DElement* createImage(
        Vec2 pos = Vec2(0.0f, 0.0f), 
        Vec2 size = Vec2(1.0f, 1.0f), 
        float rotAngle = 0.0f, 
        Vec3 rotAxis = Vec3(0.0f, 1.0f, 0.0f)
    );


    void setTexture(T2DElement* image, _Texture2D texture);
    void setTexture(TModel* image, _Texture2D texture, uint16_t maya);
    void setTexture(TAnimationModel* image, _Texture2D texture, uint16_t maya);

    TBillboard* createBillboard(
        std::string path,
        Vec3 pos = Vec3(0.0f, 0.0f, 0.0f), 
        Vec3 size = Vec3(1.0f, 1.0f, 1.0f)
    );

    TRay* drawRay(_Ray ray, _Color color);
    TRay* drawRay3D(Vec3 origen, Vec3 destino, _Color color);

    void renderAllText(
        std::string text = "Hello World", 
        float x = 0.0f, 
        float y = 0.0f, 
        float scale = 48.0f, 
        _Color color = YELLOW
    );

    void renderText(
        std::string text = "Hello World", 
        float x = 0.0f, 
        float y = 0.0f, 
        float scale = 48.0f, 
        _Color color = YELLOW
    ){texts.push_back(textToRender(text, x, y, scale, color));};

    // Inazuma tambien puede activarse o desactivarse a deseo del usuario. De moemnto la funcionalidad depende del usuario
    // pero es util cuando trabajas con varios motores. Ten cuidado de no hacerlo en medio del bucle de renderizado!
    void switchActive(){active = !active;};
    bool isActive(){return active;};

    void enableShadows(){shadows = true;};
    void disableShadows(){shadows = false;};

    void enableLights(){light = true;};
    void disableLights(){light = false;};

    void printFPS(){showFPS = true;};
    void hideFPS(){showFPS = false;};

    // Getters para los valores privados 
    GLFWwindow* getMainWindow(){return mainWindow;};
    _Shader* getBasicShader(){return basicShader;};
    _Shader* getBasicShadowShader(){return basicShadowShader;};
    TCamera* getPrincipalCamera(){return principalCamera;};
    TNode* getSceneRoot(){return sceneRoot.get();};
    uint16_t getScreenWidth(){return SCREEN_WIDTH;};
    uint16_t getScreenHeight(){return SCREEN_WIDTH;};

    void loadSkybox(std::vector<std::string> faces);

    _Texture2D loadTexture(std::string path, TextureTypes type = TextureTypes::difuse, bool flip = true);

    private:
        // Funcion interna para el dibujado 3D, ayuda a encapsular el renderizado
        void draw3D();

        void setupShadow();
        void setupSkybox();
        void setupText();
        void setupRay();
        void setupInputs();
        
        TSkybox* createSkybox();

        _Texture loadTextureToMesh(std::string path, TextureTypes type);
        
        // El puntero unico de la raiz del nodo
        std::unique_ptr<TNode> sceneRoot{};
        // Punteros al shader basico, a la camara principal y a la ventana principal
        _Shader* basicShader{};
        _Shader* basicShadowShader{};
        _Shader* basicShadowShaderCubes{};
        _Shader* basicSkyboxShader{};
        _Shader* basic2DShader{};
        _Shader* basicTextShader{};
        _Shader* basicRayShader{};
        TCamera* principalCamera{};
        GLFWwindow* mainWindow{};
        // Instancia del gestor de recursos
        TResourceManager RM{};
        // Vectores para almacenar los nodos, necesario para gestionar y encapsular las responsabilidades de la destruccion de la memoria
        std::vector<std::unique_ptr<TNode>> nodes{};
        std::vector<std::unique_ptr<TCamera>> cameras{};
        std::vector<std::unique_ptr<TLight>> lights{};
        std::vector<std::unique_ptr<TModel>> models{};
        std::vector<std::unique_ptr<T2DElement>> elements2D{};
        std::vector<std::unique_ptr<TBillboard>> billboards{};
        std::vector<std::unique_ptr<TAnimationModel>> animations{};
        std::vector<std::unique_ptr<TRay>> rays{};
        std::vector<textToRender> texts{};
        std::unique_ptr<TSkybox> skyboxModel{};
        // Numero de luces
        // X -> Luces direcionales
        // Y -> Luces puntuales
        // Z -> Luces especulares
        Vec3 numLights{};
        // Booleano para ver si esta activo el motor o no
        bool active{}, skyLoaded{}, shadows{}, showFPS{}, light{};
        uint16_t FPS{}, SCREEN_WIDTH{}, SCREEN_HEIGHT{};
        unsigned int depthMapFBO{}, depthMap{}, matricesUBO{}, depthMapFBOCubes{}, depthCubemaps{},
                     skybox{}, textVAO{}, textVBO{}, rayVAO{}, rayVBO{};
};

// Metodos generales de utilidad para el motor 

// Arranca la ventana de OpenGL y todo lo necesario para nuestro motor
GLFWwindow* setupOpenGL(uint16_t width, uint16_t height, std::string name);

// Permite hacer resize dinamico de la ventana de OpenGL
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

// Comprueba el input de teclado con las facilidades de OpenGL
bool isKeyPressed(uint16_t key);

bool isKeyDown(uint16_t key);

bool isAnyKeyPressed();

// Limpia el fondo en un color especifico
void cleanBackground(_Color c);

_Color fade(_Color c, float alpha);

void switchFullscreen();

_RayCollision checkRaysCollisions(_Ray ray, _BoundingBox box);