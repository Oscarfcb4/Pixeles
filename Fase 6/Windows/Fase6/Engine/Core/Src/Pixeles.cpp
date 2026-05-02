#include <Pixeles.hpp>
#include <SceneTree.hpp>
#include <ResourceManager.hpp>
#include <ShadersDefault.hpp>

// Variables generales para la ventana, la relacion de aspecto y el ultimo color de limpieza de pantalla
GLFWwindow* window{};
float aspect{};
Color cleanColor{};
unsigned int WIDTH{}, HEIGHT{};
bool fullscreen{};

// Importante para manejar nosotros la memoria y que no haya fugas
Pixeles::~Pixeles() = default;

// Constructor que arranca Pixeles
Pixeles::Pixeles(uint16_t width, uint16_t height, std::string name) {
    // Inicializamos la pantalla principal con OpenGL
    mainWindow = setupOpenGL(width, height, name);
    // Guardamos nuestras variables locales y globales del motor
    WIDTH = width;
    HEIGHT = height;
    SCREEN_WIDTH = width;
    SCREEN_HEIGHT = height;
    // Calculamos el aspect ratio y lo guardamos
    aspect = static_cast<float>(width) / static_cast<float>(height);

    // Si esta pantalla no existe es que OpenGL ha fallado por algun motivo, abortamos y pintamos por pantalla
    if (mainWindow == NULL) {
        std::cout << "ERROR::OPENGL::FAILED TO CREATE WINDOW" << std::endl;
        return;
    }
    // Le asignamos a la global window la ventana
    window = mainWindow;
    // Creamos el nodo raiz del arbol
    sceneRoot = std::make_unique<Node>();
    // Le decimos al resource manager que esta es la raiz de nuestra escena
    RM.setScene(sceneRoot.get());
    // Precargamos el shader base
    loadDefaultShaders();
    // Creamos el espacio para el skybox en OpenGL
    setupSkybox();
}

void Pixeles::loadDefaultShaders(){
    defaultShader = &RM.getShader("default", defaultVertexShader, defaultFragmentShader, false)->shader;
    skyboxShader = &RM.getShader("skybox", skyboxVertexShader, skyboxFragmentShader, false)->shader;
}

void Pixeles::closeWindow() {
    glfwDestroyWindow(window);  
    glfwTerminate();
}

// Inicializador de la ventana
void Pixeles::startWindow(uint16_t width, uint16_t height, std::string name) {
    // Igual que el constructor
    if (!window) {
        mainWindow = setupOpenGL(width, height, name);

        if (mainWindow == NULL) {
            std::cout << "ERROR::OPENGL::FAILED TO CREATE WINDOW" << std::endl;
            return;
        }

        window = mainWindow;
    }
    sceneRoot = std::make_unique<Node>();
    WIDTH = width;
    HEIGHT = height;
    SCREEN_WIDTH = width;
    SCREEN_HEIGHT = height;
    RM.setScene(sceneRoot.get());
    loadDefaultShaders();
    aspect = static_cast<float>(width) / static_cast<float>(height);
    setupSkybox();
}

// Asignamos una ventana ya existente
void Pixeles::setWindow(void* win, uint16_t width, uint16_t height) {
    // Comprobamos que exista la ventana
    if (win == NULL) {
        std::cout << "ERROR::OPENGL::WINDOW NOT VALID" << std::endl;
        return;
    }
    // Casteamos la ventana principal a una de OpenGL
    window = static_cast<GLFWwindow*>(win);
    mainWindow = window;
    WIDTH = width;
    HEIGHT = height;
    SCREEN_WIDTH = width;
    SCREEN_HEIGHT = height;
    sceneRoot = std::make_unique<Node>();
    RM.setScene(sceneRoot.get());
    loadDefaultShaders();
    aspect = static_cast<float>(width) / static_cast<float>(height);
    stbi_set_flip_vertically_on_load(true);
    // Activamos el Face Culling
    glEnable(GL_CULL_FACE);
    // Aunque viene por defecto, por si acaso, le decimos que solo lo haga con las caras traseras
    glCullFace(GL_BACK);
    setupSkybox();
}

// Inicio del dibujado, de momento solo nos aseguramos de que siempre haya una camara principal
void Pixeles::start() {
    if (!principalCamera)
        createCamera();
}

// Final del dibujado, se encarga de renderizar
void Pixeles::end() {
    // Si el usuario no ha creado luces, crea una luz direccional para que haya al menos una
    if (lights.size() == 0)
        createDirectionalLight();

    // Llamamos al metodo de dibujado global
    draw3D();

    // Metodos de GLFW para controlar la ventana y su buffer de OpenGL
    glfwSwapBuffers(mainWindow);
    glfwPollEvents();
}

void Pixeles::endNoSwap() {
    // Si el usuario no ha creado luces, crea una luz direccional para que haya al menos una
    if (lights.size() == 0)
        createDirectionalLight();

    // Llamamos al metodo de dibujado global
    draw3D();
}

// Limpia el arbol de la escena
void Pixeles::clearScene() {
    // Limpiamos los hijos de la raiz
    sceneRoot->getChildren().clear();
    // Limpiamos el vector de nodos
    nodes.clear();
    // Limpiamos el vector de camaras menos la camara principal
    cameras.erase(std::remove_if(
        cameras.begin(), cameras.end(), [this](const std::unique_ptr<E_Camera>& p) {
            return p.get() != principalCamera;
        }), cameras.end()
            );
    // Limpiamos las luces
    lights.clear();
    // Limpaimos los modelos
    models.clear();
    // Reseteamos el nuemro de luces
    numLights = glm::vec3(0.0f, 0.0f, 0.0f);
}

// Metodos de creacion de entidades

Shader* Pixeles::createShader(std::string name, std::string vs, std::string fs) {
    return &RM.getShader(name, vs, fs)->shader;
}

E_Camera* Pixeles::createCamera(Vec3 position, Vec3 up, float yaw, float pitch) {
    // Creamos un nodo como un puntero unico
    auto nodeCam = std::make_unique<Node>();
    // Lo pusheamos al vector de nodos y le pasamos la responsabilidad de memoria
    nodes.push_back(std::move(nodeCam));
    // Lo recuperamos como una referencia para trabajar con el
    auto& nCam = nodes[nodes.size() - 1];
    // Creamos la entidad con los parámetros
    auto entityCam = std::make_unique<E_Camera>(parseGLM(position), parseGLM(up), yaw, pitch);
    // Lo pusheamos y le pasamos la responsabilidad de memoria
    cameras.push_back(std::move(entityCam));
    // La recuperamos como referencia
    auto& eCam = cameras[cameras.size() - 1];
    // Si no existe ya una, la asigna como camara principal
    if(sceneRoot->getPrincipalCamera() == NULL){
        sceneRoot->setPrincipalCamera(eCam.get());
        principalCamera = eCam.get();
    }
    // Le asigna al nodo la entidad
    nCam->setEntity(eCam.get());
    // Guardamos el nodo en la entidad
    eCam->node = nCam.get();
    // Lo anadimos a la raiz como hijo, para que forme parte de la escena
    sceneRoot->addChild(nCam.get());
    // Devuelve un puntero al nodo, para que el usuario pueda jugar con él
    return dynamic_cast<E_Camera*>(eCam.get());
}

E_Light* Pixeles::createDirectionalLight(Vec3 amb, Vec3 dir, Vec3 dif, Vec3 spc, std::string shader) {
    auto nodeLight = std::make_unique<Node>();
    nodes.push_back(std::move(nodeLight));
    auto& nLight = nodes[nodes.size() - 1];
    auto entityLight = std::make_unique<E_Light>();
    lights.push_back(std::move(entityLight));
    auto& eLight = lights[lights.size() - 1];
    if(shader != "default")
        eLight->shader = &RM.getShader(shader, shader, shader)->shader;
    else
        eLight->shader = defaultShader;
    // Crea una luz direccional en la entidad luz con los parametros
    eLight->setDirectionalLight(parseGLM(dir), parseGLM(amb), parseGLM(dif), parseGLM(spc));
    // Marcamos la id como el numero actual de luces direccionales
    eLight->id = static_cast<int>(numLights.x);
    nLight->setEntity(std::move(eLight.get()));
    sceneRoot->addChild(std::move(nLight.get()));
    eLight->node = nLight.get();
    // Aumenta el contador de luces direccionales 
    numLights.x += 1;
    return dynamic_cast<E_Light*>(eLight.get());
}

E_Light* Pixeles::createPointLight(Vec3 pos, Vec3 amb, Vec3 dif, Vec3 spc, Vec3 att, std::string shader) {
    auto nodeLight = std::make_unique<Node>();
    nodes.push_back(std::move(nodeLight));
    auto& nLight = nodes[nodes.size() - 1];
    auto entityLight = std::make_unique<E_Light>();
    lights.push_back(std::move(entityLight));
    auto& eLight = lights[lights.size() - 1];
    if(shader != "default")
        eLight->shader = &RM.getShader(shader, shader, shader)->shader;
    else
        eLight->shader = defaultShader;
    // Crea una luz puntual en la entidad luz con los parametros
    eLight->setPointLight(parseGLM(pos), parseGLM(amb), parseGLM(dif), parseGLM(spc), parseGLM(att));
    // Esta vez usamos el numero de luces puntuales
    eLight->id = static_cast<int>(numLights.y);
    nLight->setEntity(std::move(eLight.get()));
    sceneRoot->addChild(std::move(nLight.get()));
    eLight->node = nLight.get();
    // Esta vez aumentamos el numero de luces puntuales
    numLights.y += 1;
    return dynamic_cast<E_Light*>(eLight.get());
}

E_Light* Pixeles::createSpotLight(Vec3 pos, Vec3 dir, Vec3 amb, Vec3 dif, Vec3 spc, Vec3 att, Vec2 cut, std::string shader) {
    auto nodeLight = std::make_unique<Node>();
    nodes.push_back(std::move(nodeLight));
    auto& nLight = nodes[nodes.size() - 1];
    auto entityLight = std::make_unique<E_Light>();
    lights.push_back(std::move(entityLight));
    auto& eLight = lights[lights.size() - 1];
    if(shader != "default")
        eLight->shader = &RM.getShader(shader, shader, shader)->shader;
    else
        eLight->shader = defaultShader;
    glm::vec2 cones = glm::vec2(glm::cos(glm::radians(cut.x)), glm::cos(glm::radians(cut.y)));
    // Crea una luz focal en la entidad luz con los parametros
    eLight->setSpotLight(parseGLM(pos), parseGLM(dir), parseGLM(amb), parseGLM(dif), parseGLM(spc), parseGLM(att), cones);
    // Esta vez aumentamos el numero de luces focales
    eLight->id = static_cast<int>(numLights.z);
    nLight->setEntity(std::move(eLight.get()));
    sceneRoot->addChild(std::move(nLight.get()));
    eLight->node = nLight.get();
    // Esta vez aumentamos el numero de luces focales
    numLights.z += 1;
    return dynamic_cast<E_Light*>(eLight.get());
}

E_Model* Pixeles::createModel(std::string path, Vec3 pos, Vec3 size, float rotAngle, Vec3 rotAxis, std::string shader) {
    auto nodeModel = std::make_unique<Node>();
    nodes.push_back(std::move(nodeModel));
    auto& nModel = nodes[nodes.size() - 1];
    auto entityModel = std::make_unique<E_Model>(&RM, path, RType::RModel);
    models.push_back(std::move(entityModel));
    auto& eModel = models[models.size() - 1];
    eModel->setShader(shader);
    // Translada el modelo a la posicion por parametro
    nModel->translate(parseGLM(pos));
    // Luego lo rota
    nModel->rotate(glm::vec4(rotAxis.x, rotAxis.y, rotAxis.z, rotAngle));
    // Y luego lo escala
    nModel->scale(parseGLM(size));
    nModel->setEntity(std::move(eModel.get()));
    // Le ajusta la relacion de aspecto
    eModel->setAspect(aspect);
    sceneRoot->addChild(std::move(nModel.get()));
    eModel->node = nModel.get();
    return dynamic_cast<E_Model*>(eModel.get());
}

E_Model* Pixeles::createCube(Vec3 pos, Vec3 size, Color color, float rotAngle, Vec3 rotAxis, bool texture) {
	// Creamos el nodo
	auto nodeCube = std::make_unique<Node>();
	// Lo metemos en el vector de nodos
	nodes.push_back(std::move(nodeCube));
	// Lo recuperamos como referencia
	auto& nCube = nodes[nodes.size() - 1];
	if (texture) {
		// Creamos la entidad con un nombre unico para evitar compartir texturas entre cubos
		auto entityModel = std::make_unique<E_Model>(&RM, "Cubo_" + models.size(), RType::RCube);
		// Lo metemos en el vector de modelos
		models.push_back(std::move(entityModel));
	} else {
		// Creamos la entidad con un nombre compartido para evitar recargar el modelo
		auto entityModel = std::make_unique<E_Model>(&RM, "Cubo", RType::RCube);
		// Lo metemos en el vector de modelos
		models.push_back(std::move(entityModel));
	}
	// Lo recuperamos como referencia
	auto& eCube = models[models.size() - 1];
	// Le metemos nuestro shader, con el mismo nombre que le pusimos
	eCube->setShader("default");
	// Aplicamos transformaciones para colocarlo como lo quiere el usuario
	nCube->translate(parseGLM(pos));
	nCube->rotate(glm::vec4(rotAxis.x, rotAxis.y, rotAxis.z, rotAngle));
	nCube->scale(parseGLM(size));
	// Le ponemos el color y el aspect ratio
	eCube->color = color;
	eCube->setAspect(aspect);
	// Guardamos la entidad dentro del nodo
	nCube->setEntity(eCube.get());
	// Lo añadimos a la escena
	sceneRoot->addChild(std::move(nCube.get()));
    eCube->node = nCube.get();
	// Recuperamos el nodo para el usuario
	return dynamic_cast<E_Model*>(eCube.get());
}

E_Skybox* Pixeles::createSkybox(std::vector<std::string> faces) {
    loadSkybox(faces);
    auto nodeCube = std::make_unique<Node>();
    nodes.push_back(std::move(nodeCube));
    auto& nCube = nodes[nodes.size() - 1];
    // Es un cubo básico así que lo enviamos con el mismo nombre que el cubo sin textura
    auto entityModel = std::make_unique<E_Skybox>(&RM, "Cubo", RType::RCube);
    skyboxModel = std::move(entityModel);
    auto& eCube = skyboxModel;
    // El skybox necesita su shader
    eCube->setShader("skybox");
    eCube->setAspect(aspect);
    nCube->setEntity(std::move(eCube.get()));
    sceneRoot->addChild(std::move(nCube.get()));
    eCube->node = nCube.get();
    return dynamic_cast<E_Skybox*>(eCube.get());
}

// Metodo de renderizado global
void Pixeles::draw3D() {
    // Por si acaso
    glEnable(GL_DEPTH_TEST);
    // Renderizamos primero las luces, asi da igual el orden en los que las haya metido el usuario en el arbol
    for (auto& l : lights)
        l->draw(glm::mat4{ 1.0f }, sceneRoot->getPrincipalCamera());
    // Le decimos al shader cuantas luces hay de cada tipo
    defaultShader->setInt("n_directionalLights", static_cast<int>(numLights.x));
    defaultShader->setInt("n_pointLights", static_cast<int>(numLights.y));
    defaultShader->setInt("n_spotLights", static_cast<int>(numLights.z));

    if (skyboxModel)
        skyboxModel->draw(glm::mat4{ 1.0f }, sceneRoot->getPrincipalCamera());

    // Recorremos el arbol
    sceneRoot->traversal(glm::mat4{ 1.0f }, sceneRoot->getPrincipalCamera());
}

// Metodo para arrancar OpenGL
GLFWwindow* Pixeles::setupOpenGL(uint16_t width, uint16_t height, std::string name) {
    // Inicio GLFW
    if (!glfwInit())
        std::cout << "Failed to initiate GLFW" << std::endl;
    // Le decimos a GLFW las veriones de OpenGL compatibles y el perfil que vamos a usar, el core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Creamos la ventana con GLFW
    GLFWwindow* window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
    // Si no se crea la ventana, abortamos y lo decimos por terminal
    if (window == NULL) {
        std::cout << "Failed to create window" << std::endl;
        glfwTerminate();
        return nullptr;
    }

    // Le decimos a GLFW que use la ventana creada
    glfwMakeContextCurrent(window);

    // Inicializamos GLAD con el contexto de GLFW
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initializa GLAD" << std::endl;
        return nullptr;
    }
    // Iniciamos el viewport de OpenGL
    glViewport(0, 0, width, height);
    // Le decimos a OpenGL que maneje las profundidades con el Z-Buffer
    glEnable(GL_DEPTH_TEST);
    // Activamos el Face Culling
    glEnable(GL_CULL_FACE);
    // Devolvemos la ventana
    return window;
}

void Pixeles::setupSkybox() {
    glGenTextures(1, &skybox);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox); glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void Pixeles::loadSkybox(std::vector<std::string> faces) {
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox);
    int width{}, height{}, nrChannels{};
    for (uint16_t i{}; i < faces.size(); i++) {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        else {
            std::cout << "ERROR::SKYBOX::Source skybox images cannot load" << std::endl;
        }
        stbi_image_free(data);
    }
    skyLoaded = true;
}

bool Pixeles::isKeyPressed(uint16_t key){
    if(glfwGetKey(window, key) == GLFW_PRESS){
        return true;
    }
    return false;
}

// Limpia la pantalla ocn un color, como OpenGL lo tiene todo normalizado entre 0 y 1 toca dividir para conventir los valores RGBA
void Pixeles::cleanBackground(Color c) {
    // Como las divisiones son costosas, guardamos una copia del color y solo dividimos si el color que le pasas es diferente al ultimo
    if (c != cleanColor) {
        // reasignamos los colores al cleanColor global
        cleanColor = Color(c.r / 255.0f, c.g / 255.0f, c.b / 255.0f, c.a / 255.0f);
    }

    // Limpiamos la pantalla con el color de limpieza
    glClearColor(cleanColor.r, cleanColor.g, cleanColor.b, cleanColor.a);
    // Limpiamos tambien el buffer de color y el buffer de profundidad
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Pixeles::switchFullscreen() {
    if (fullscreen)
        glfwSetWindowMonitor(window, NULL, 0, 0, WIDTH, HEIGHT, 60);
    else
        glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, WIDTH, HEIGHT, 60);
    fullscreen = !fullscreen;
}