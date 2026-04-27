#include "Inazuma.hpp"
#include "defaultFontData.hpp"

/////////////////////////////////////////////////////////////////
//                        INAZUMA.HPP                          //
/////////////////////////////////////////////////////////////////

// Variables generales para la ventana, la relacion de aspecto y el ultimo color de limpieza de pantalla
GLFWwindow* window{};
float aspect{};
_Color cleanColor{};
const unsigned int SHADOW_WIDTH{1024}, SHADOW_HEIGHT{1024};
unsigned int WIDTH{}, HEIGHT{};
std::map<char, character> characters{};
bool fullscreen{};
double lastTime{glfwGetTime()};
int nbFrames = 0;
int oldState = GLFW_RELEASE;
std::map<uint16_t, InputStates> inputs{};

// Constructor que arranca Inazuma
Inazuma::Inazuma(uint16_t width, uint16_t height, std::string name){
    // Inicializamos la pantalla principal con OpenGL
    mainWindow = setupOpenGL(width, height, name);
    WIDTH = width;
    HEIGHT = height;
    SCREEN_WIDTH = width;
    SCREEN_HEIGHT = height;

    // Si esta pantalla no existe es que OpenGL ha fallado por algun motivo, abortamos y pintamos por pantalla
    if(mainWindow == NULL){
        std::cout << "ERROR::OPENGL::FAILED TO CREATE WINDOW" << std::endl;
        return;
    }

    // Creamos el nodo raiz del arbol
    sceneRoot = std::make_unique<TNode>();
    // Le decimos al resource manager que esta es la raiz de nuestra escena
    RM.setScene(sceneRoot.get());
    // Precargamos el shader base
    basicShader = &RM.loadBasicShader("base")->shader;
    basicShadowShader = &RM.loadBasicShader("baseShadow")->shader;
    basicShadowShaderCubes = &RM.loadBasicShader("baseShadowCubes")->shader;
    basicSkyboxShader = &RM.loadBasicShader("basicSkybox")->shader;
    basic2DShader = &RM.loadBasicShader("basic2D")->shader;
    basicTextShader = &RM.loadBasicShader("basicText")->shader;
    basicRayShader = &RM.loadBasicShader("basicRay")->shader;
    // Creamos la camara principal, obligatoria
    createCamera();
    // Le asignamos a la global window la ventana
    window = mainWindow;
    // Calculamos el aspect ratio y lo guardamos
    aspect = static_cast<float>(width)/static_cast<float>(height);
    setupShadow();
    setupSkybox();
    setupText();
    setupRay();
    setupInputs();
}

void Inazuma::closeWindow(){
	glfwTerminate();
}

// Inicializador de la ventana
void Inazuma::startWindow(uint16_t width, uint16_t height, std::string name){
    // Igual que el constructor
    if(!window){
        mainWindow = setupOpenGL(width, height, name);

        if(mainWindow == NULL){
            std::cout << "ERROR::OPENGL::FAILED TO CREATE WINDOW" << std::endl;
            return;
        }

        window = mainWindow;
    }
    sceneRoot = std::make_unique<TNode>();
    WIDTH = width;
    HEIGHT = height;
    SCREEN_WIDTH = width;
    SCREEN_HEIGHT = height;
    RM.setScene(sceneRoot.get());
    basicShader = &RM.loadBasicShader("base")->shader;
    basicShadowShader = &RM.loadBasicShader("baseShadow")->shader;
    basicShadowShaderCubes = &RM.loadBasicShader("baseShadowCubes")->shader;
    basicSkyboxShader = &RM.loadBasicShader("basicSkybox")->shader;
    basic2DShader = &RM.loadBasicShader("basic2D")->shader;
    basicTextShader = &RM.loadBasicShader("basicText")->shader;
    basicRayShader = &RM.loadBasicShader("basicRay")->shader;
    createCamera();
    aspect = static_cast<float>(width)/static_cast<float>(height);
    setupShadow();
    setupSkybox();
    setupText();
    setupRay();
    setupInputs();
}

// Asignamos una ventana ya existente
void Inazuma::setWindow(void* win, uint16_t width, uint16_t height){
    // Comprobamos que exista la ventana
    if(win == NULL){
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
    sceneRoot = std::make_unique<TNode>();
    RM.setScene(sceneRoot.get());
    basicShader = &RM.loadBasicShader("base")->shader;
    basicShadowShader = &RM.loadBasicShader("baseShadow")->shader;
    basicShadowShaderCubes = &RM.loadBasicShader("baseShadowCubes")->shader;
    basicSkyboxShader = &RM.loadBasicShader("basicSkybox")->shader;
    basic2DShader = &RM.loadBasicShader("basic2D")->shader;
    basicTextShader = &RM.loadBasicShader("basicText")->shader;
    basicRayShader = &RM.loadBasicShader("basicRay")->shader;
    createCamera();
    aspect = static_cast<float>(width)/static_cast<float>(height);
    // Como no pasamos por setupOpenGL nos aseguramos de que esto este activo
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    stbi_set_flip_vertically_on_load(true);
    // Activamos el Face Culling
    glEnable(GL_CULL_FACE);
    // Aunque viene por defecto, por si acaso, le decimos que solo lo haga con las caras traseras
    glCullFace(GL_BACK);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
    setupShadow();
    setupSkybox();
    setupText();
    setupRay();
    setupInputs();
}

// Inicio del dibujado, de momento solo nos aseguramos de que siempre haya una camara principal
void Inazuma::start(){
    if(!principalCamera)
        createCamera();
    double currentTime = glfwGetTime();
    nbFrames+=1;
    if(currentTime-lastTime >= 1.0){
        FPS = static_cast<uint16_t>(nbFrames);
        nbFrames = 0;
        lastTime += 1.0;
    }
    if(showFPS)
        renderText("FPS " + std::to_string(FPS), 20, 40, 30);
}

// Final del dibujado, se encarga de renderizar
void Inazuma::end(){
    // Si el usuario no ha creado luces, crea una luz direccional para que haya al menos una
    if(lights.size() == 0 && light)
        createDirectionalLight();

    if(shadows){
        shadowRender();
    }

    // Llamamos al metodo de dibujado global
    draw3D(); 

    // Metodos de GLFW para controlar la ventana y su buffer de OpenGL
	glfwSwapBuffers(mainWindow);
    glfwPollEvents();
}

void Inazuma::endNoSwap(){
    // Si el usuario no ha creado luces, crea una luz direccional para que haya al menos una
    if(lights.size() == 0 && light)
        createDirectionalLight();

    if(shadows){
        shadowRender();
    }

    // Llamamos al metodo de dibujado global
    draw3D();
}

// Limpia el arbol de la escena
void Inazuma::clearScene(){
    // Limpiamos los hijos de la raiz
    sceneRoot->getChildren().clear();
    // Limpiamos el vector de nodos
    nodes.clear();
    // Limpiamos el vector de camaras menos la camara principal
    cameras.erase(std::remove_if(
        cameras.begin(), cameras.end(), [this](const std::unique_ptr<TCamera>& p){
            return p.get() != principalCamera;
        }), cameras.end()
    );
    // Limpiamos las luces
    lights.clear();
    // Limpaimos los modelos
    models.clear();
    // Reseteamos el nuemro de luces
    numLights = Vec3(0.0f, 0.0f, 0.0f);
}

void Inazuma::shadowRender(){ 
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    glDisable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    basicShadowShader->use();
    
    glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO);
    uint16_t shadowCont{};
    for(auto& l : lights){
		l->drawShadowArray(basicShadowShader, shadowCont, glm::mat4{1.0f});
    }

    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    sceneRoot->renderShadows(glm::mat4(1.0f), basicShadowShader);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBOCubes);
    glClear(GL_DEPTH_BUFFER_BIT);

    basicShadowShaderCubes->use();

    shadowCont = 1;
    for(auto& l : lights){
		l->drawShadowCubes(basicShadowShaderCubes, shadowCont, glm::mat4{1.0f});
    }

    sceneRoot->renderShadows(glm::mat4(1.0f), basicShadowShaderCubes);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glViewport(0, 0, WIDTH, HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

// Metodos de creacion de entidades

TCamera* Inazuma::createCamera(Vec3 position_, Vec3 up_, float yaw_, float pitch_){
    // Creamos un nodo como un puntero unico
    auto nodeCam = std::make_unique<TNode>();
    // Lo pusheamos al vector de nodos y le pasamos la responsabilidad de memoria
    nodes.push_back(std::move(nodeCam));
    // Lo recuperamos como una referencia para trabajar con el
    auto& nCam = nodes[nodes.size()-1];
    // Creamos la entidad 
    auto entityCam = std::make_unique<TCamera>(position_, up_, yaw_, pitch_);
    // Lo pusheamos y le pasamos la responsabilidad de memoria
    cameras.push_back(std::move(entityCam));
    // La recuperamos como referencia
    auto& eCam = cameras[cameras.size()-1];
    // Si no existe una camara, la asigna como camara principal
    if(!principalCamera){
        principalCamera = eCam.get();
        sceneRoot->setPrincipalCamera(eCam.get());
    }
    // Le asigna al nodo la entidad
    nCam->setEntity(eCam.get());
    eCam->node = nCam.get();
    // Le anadimos a la raiz como hijo
    sceneRoot->addChild(nCam.get());
    // Devuelve un puntero al nodo
    return eCam.get();
}

TLight* Inazuma::createDirectionalLight(Vec3 amb, Vec3 dir, Vec3 dif, Vec3 spc, std::string shader){
    auto nodeLight = std::make_unique<TNode>();
    nodes.push_back(std::move(nodeLight));
    auto& nLight = nodes[nodes.size()-1];
    auto entityLight = std::make_unique<TLight>();
    lights.push_back(std::move(entityLight));
    auto& eLight = lights[lights.size()-1];
    // Si no le pasas un shader, usa el base. Si no, busca el que le has pasado en el gestor de recursos
    if(strcmp(shader.c_str(), "base") != 0)
        eLight->shader = &RM.getShader(shader, shader, shader)->shader;
    else
        eLight->shader = basicShader;
    // Crea una luz direccional en la entidad luz con los parametros
    eLight->setDirectionalLight(dir, amb, dif, spc);
    // Marcamos la id como el numero actual de luces direccionales (la id empieza en 0, nuestro contador no)
    eLight->id = static_cast<int>(numLights.x);
    eLight->node = nLight.get();
    nLight->setEntity(std::move(eLight.get()));
    sceneRoot->addChild(std::move(nLight.get()));
    // Aumenta el numero de luces direccionales en 1
    numLights.x += 1;
    return eLight.get();
}

TLight* Inazuma::createPointLight(Vec3 pos, Vec3 amb, Vec3 dif, Vec3 spc, Vec3 att, std::string shader){
    auto nodeLight = std::make_unique<TNode>();
    nodes.push_back(std::move(nodeLight));
    auto& nLight = nodes[nodes.size()-1];
    auto entityLight = std::make_unique<TLight>();
    lights.push_back(std::move(entityLight));
    auto& eLight = lights[lights.size()-1];
    if(shader != "base")
        eLight->shader = &RM.getShader(shader, shader, shader)->shader;
    else
        eLight->shader = basicShader;
    // Crea una luz puntual en la entidad luz con los parametros
    eLight->setPointLight(pos, amb, dif, spc, att);
    // Esta vez usamos el numero de luces puntuales
    eLight->id = static_cast<int>(numLights.y);
    eLight->node = nLight.get();
    nLight->setEntity(std::move(eLight.get()));
    sceneRoot->addChild(std::move(nLight.get()));
    // Esta vez aumentamos el numero de luces puntuales
    numLights.y += 1;
    return eLight.get();
}

TLight* Inazuma::createSpotLight(Vec3 pos, Vec3 dir, Vec3 amb, Vec3 dif, Vec3 spc, Vec3 att, Vec2 cut, std::string shader){
    auto nodeLight = std::make_unique<TNode>();
    nodes.push_back(std::move(nodeLight));
    auto& nLight = nodes[nodes.size()-1];
    auto entityLight = std::make_unique<TLight>();
    lights.push_back(std::move(entityLight));
    auto& eLight = lights[lights.size()-1];
    if(shader != "base")
        eLight->shader = &RM.getShader(shader, shader, shader)->shader;
    else
        eLight->shader = basicShader;
    // Crea una luz focal en la entidad luz con los parametros
    eLight->setSpotLight(pos, dir, amb, dif, spc, att, cut);
    // Esta vez aumentamos el numero de luces focales
    eLight->id = static_cast<int>(numLights.z);
    eLight->node = nLight.get();
    nLight->setEntity(std::move(eLight.get()));
    sceneRoot->addChild(std::move(nLight.get()));
    // Esta vez aumentamos el numero de luces focales
    numLights.z += 1;
    return eLight.get();
}   

TModel* Inazuma::createModel(std::string path, Vec3 pos, Vec3 size, float rotAngle, Vec3 rotAxis, std::string shader){
    auto nodeModel = std::make_unique<TNode>();
    nodes.push_back(std::move(nodeModel));
    auto& nModel = nodes[nodes.size()-1];
    auto entityModel = std::make_unique<TModel>(&RM, path, RType::TRAnim);
    models.push_back(std::move(entityModel));
    auto& eModel = models[models.size()-1];
    // Si el shader no es el basico, lo setea (este metodo lo buscara en el resource manager)
    if(shader != "base")
        eModel->setShader(shader);
    else
        eModel->setShader("base");
    // Translada el modelo a la posicion por parametro
    nModel->translate(pos);
    // Luego lo rota
    nModel->rotate(Vec4(rotAxis.x, rotAxis.y, rotAxis.z, rotAngle));
    // Y luego lo escala
    nModel->scale(size);
    eModel->node = nModel.get();
    nModel->setEntity(std::move(eModel.get()));
    // Le ajusta la relacion de aspecto
    eModel->setAspect(aspect);
    sceneRoot->addChild(std::move(nModel.get()));
    return eModel.get();
}

TAnimationModel* Inazuma::createAnimationModel(std::string path, Vec3 pos, Vec3 size, float rotAngle, Vec3 rotAxis, std::string shader){
    auto nodeModel = std::make_unique<TNode>();
    nodes.push_back(std::move(nodeModel));
    auto& nModel = nodes[nodes.size()-1];
    auto entityModel = std::make_unique<TAnimationModel>(&RM, path, RType::TRModel);
    animations.push_back(std::move(entityModel));
    auto& eModel = animations[animations.size()-1];
    // Si el shader no es el basico, lo setea (este metodo lo buscara en el resource manager)
    if(shader != "base")
        eModel->setShader(shader);
    else
        eModel->setShader("base");
    // Translada el modelo a la posicion por parametro
    nModel->translate(pos);
    // Luego lo rota
    nModel->rotate(Vec4(rotAxis.x, rotAxis.y, rotAxis.z, rotAngle));
    // Y luego lo escala
    nModel->scale(size);
    eModel->node = nModel.get();
    nModel->setEntity(std::move(eModel.get()));
    // Le ajusta la relacion de aspecto
    eModel->setAspect(aspect);
    sceneRoot->addChild(std::move(nModel.get()));
    return eModel.get();
}

TModel* Inazuma::createCube(Vec3 pos, Vec3 size, _Color color, float rotAngle, Vec3 rotAxis){
    auto nodeCube = std::make_unique<TNode>();
    nodes.push_back(std::move(nodeCube));
    auto& nCube = nodes[nodes.size()-1];
    // Como siempre va a ser igual le pasamos ya las cosas por el constructor
    auto entityModel = std::make_unique<TModel>(&RM, "Inazuma_Cube", RType::TRCube);
    models.push_back(std::move(entityModel));
    auto& eCube = models[models.size()-1];
    // El cubo siempre va a tener solo un shader, el base
    eCube->setShader("base");
    // Aplicamos transformaciones
    nCube->translate(Vec3(pos.x, pos.y, pos.z));
    nCube->rotate(Vec4(rotAxis.x, rotAxis.y, rotAxis.z, rotAngle));
    nCube->scale(Vec3(size.x/2, size.y/2, size.z/2));
    // Le ponemos el color
    eCube->color.x = color.r;
    eCube->color.y = color.g;
    eCube->color.z = color.b;
    eCube->color.w = color.a;
    eCube->setAspect(aspect);
    eCube->node = nCube.get();
    nCube->setEntity(std::move(eCube.get()));
    sceneRoot->addChild(std::move(nCube.get()));
    return eCube.get();
}

TSkybox* Inazuma::createSkybox(){
    auto nodeCube = std::make_unique<TNode>();
    nodes.push_back(std::move(nodeCube));
    auto& nCube = nodes[nodes.size()-1];
    // Como siempre va a ser igual le pasamos ya las cosas por el constructor
    auto entityModel = std::make_unique<TSkybox>(&RM, "Inazuma_Cube", RType::TRCube);
    skyboxModel = std::move(entityModel);
    auto& eCube = skyboxModel;
    // El cubo siempre va a tener solo un shader, el base
    eCube->setShader("basicSkybox");
    eCube->setAspect(aspect);
    eCube->node = nCube.get();
    nCube->setEntity(std::move(eCube.get()));
    sceneRoot->addChild(std::move(nCube.get()));
    return eCube.get();
}

T2DElement* Inazuma::createRectangle(Vec2 pos, Vec2 size, _Color color, float rotAngle, Vec3 rotAxis){
    auto nodeRec = std::make_unique<TNode>();
    nodes.push_back(std::move(nodeRec));
    auto& nRec = nodes[nodes.size()-1];
    auto entityRec = std::make_unique<T2DElement>(&RM, "Inazuma_Rec", RType::TRRectangle);
    elements2D.push_back(std::move(entityRec));
    auto& eRec = elements2D[elements2D.size()-1];
    eRec->setShader("basic2D");
    pos.x += size.x;
    pos.y += size.y;
    pos.x = pos.x - static_cast<float>(WIDTH/2);
    pos.x /= static_cast<float>(WIDTH/2);
    pos.y = pos.y - static_cast<float>(HEIGHT/2);
    pos.y /= static_cast<float>(HEIGHT/2);
    pos.y = -pos.y;
    size.x /= static_cast<float>(WIDTH/2);
    size.y /= static_cast<float>(HEIGHT/2);
    eRec->setImage(size, pos);
    nRec->translate(Vec3(pos.x, pos.y, 1.0f));
    nRec->rotate(Vec4(rotAxis.x, rotAxis.y, rotAxis.z, rotAngle));
    nRec->scale(Vec3(size.x, size.y, 1.0f));
    eRec->color.x = color.r;
    eRec->color.y = color.g;
    eRec->color.z = color.b;
    eRec->color.w = color.a;
    eRec->node = nRec.get();
    eRec->model->setMixValue(1.0f);
    nRec->setEntity(std::move(eRec.get()));
    sceneRoot->addChild(std::move(nRec.get()));
    eRec->setAspect(aspect);
    return eRec.get();
}

T2DElement* Inazuma::createRectangleWired(Vec2 pos, Vec2 size, _Color color, float rotAngle, Vec3 rotAxis){
    auto nodeRec = std::make_unique<TNode>();
    nodes.push_back(std::move(nodeRec));
    auto& nRec = nodes[nodes.size()-1];
    auto entityRec = std::make_unique<T2DElement>(&RM, "Inazuma_Rec_Wired", RType::TRRecWired);
    elements2D.push_back(std::move(entityRec));
    auto& eRec = elements2D[elements2D.size()-1];
    eRec->setShader("basic2D");
    pos.x += size.x;
    pos.y += size.y;
    pos.x = pos.x - static_cast<float>(WIDTH/2);
    pos.x /= static_cast<float>(WIDTH/2);
    pos.y = pos.y - static_cast<float>(HEIGHT/2);
    pos.y /= static_cast<float>(HEIGHT/2);
    pos.y = -pos.y;
    size.x /= static_cast<float>(WIDTH/2);
    size.y /= static_cast<float>(HEIGHT/2);
    eRec->setImage(size, pos);
    nRec->translate(Vec3(pos.x, pos.y, 1.0f));
    nRec->rotate(Vec4(rotAxis.x, rotAxis.y, rotAxis.z, rotAngle));
    nRec->scale(Vec3(size.x, size.y, 1.0f));
    eRec->color.x = color.r;
    eRec->color.y = color.g;
    eRec->color.z = color.b;
    eRec->color.w = color.a;
    eRec->wired = true;
    eRec->node = nRec.get();
    eRec->model->setMixValue(1.0f);
    nRec->setEntity(std::move(eRec.get()));
    sceneRoot->addChild(std::move(nRec.get()));
    eRec->setAspect(aspect);
    return eRec.get();
}

T2DElement* Inazuma::createImage(std::string path, Vec2 pos, Vec2 size, float rotAngle, Vec3 rotAxis){
    auto nodeImage = std::make_unique<TNode>();
    nodes.push_back(std::move(nodeImage));
    auto& nImage = nodes[nodes.size()-1];
    auto entityImage = std::make_unique<T2DElement>(&RM, path, RType::TRImage);
    elements2D.push_back(std::move(entityImage));
    auto& eImage = elements2D[elements2D.size()-1];
    eImage->setShader("basic2D");
    eImage->setImage(size, pos);
    nImage->translate(Vec3(pos.x, pos.y, 1.0f));
    nImage->rotate(Vec4(rotAxis.x, rotAxis.y, rotAxis.z, rotAngle));
    nImage->scale(Vec3(size.x, size.y, 1.0f));
    eImage->node = nImage.get();
    nImage->setEntity(std::move(eImage.get()));
    sceneRoot->addChild(std::move(nImage.get()));
    eImage->setAspect(aspect);
    return eImage.get();
}

T2DElement* Inazuma::createImage(Vec2 pos, Vec2 size, float rotAngle, Vec3 rotAxis){
    return createImage("none", pos, size, rotAngle, rotAxis);
}

void Inazuma::setTexture(T2DElement* image, _Texture2D texture){
    _Texture txt{};
    txt.id = texture.id;
    txt.type = texture.type;
    image->difusse = txt;
}

void Inazuma::setTexture(TModel* image, _Texture2D texture, uint16_t maya){
    _Texture txt{};
    txt.id = texture.id;
    txt.type = texture.type;
    uint16_t type{};
    if(texture.type == "texture_diffuese")
        type = 0;
    else if(texture.type == "texture_specular")
        type = 1;
    image->model->getMeshes().at(maya).textures[type] = txt;
}

void Inazuma::setTexture(TAnimationModel* image, _Texture2D texture, uint16_t maya){
    _Texture txt{};
    txt.id = texture.id;
    txt.type = texture.type;
    uint16_t type{};
    if(texture.type == "texture_diffuese")
        type = 0;
    else if(texture.type == "texture_specular")
        type = 1;
    image->model->getMeshes().at(maya).textures[type] = txt;
}

TBillboard* Inazuma::createBillboard(std::string path, Vec3 pos, Vec3 size){
    auto nodeImage = std::make_unique<TNode>();
    nodes.push_back(std::move(nodeImage));
    auto& nImage = nodes[nodes.size()-1];
    auto entityImage = std::make_unique<TBillboard>(&RM, path, RType::TRModel);
    billboards.push_back(std::move(entityImage));
    auto& eImage = billboards[billboards.size()-1];
    eImage->setShader("base");
    eImage->pos = glm::vec3(pos.x, pos.y, pos.z);
    nImage->translate(pos);
    nImage->scale(size);
    eImage->node = nImage.get();
    nImage->setEntity(std::move(eImage.get()));
    sceneRoot->addChild(std::move(nImage.get()));
    eImage->setAspect(aspect);
    return eImage.get();
}

TRay* Inazuma::drawRay(_Ray ray, _Color color){
    auto nodeRay = std::make_unique<TNode>();
    nodes.push_back(std::move(nodeRay));
    auto& nRay = nodes[nodes.size()-1];
    auto entityRay = std::make_unique<TRay>(&RM, ray.position, ray.direction);
    rays.push_back(std::move(entityRay));
    auto& eRay = rays[rays.size()-1];
    eRay->VAO = rayVAO;
    eRay->VBO = rayVBO;
    eRay->color.r = color.r;
    eRay->color.g = color.g;
    eRay->color.b = color.b;
    eRay->color.a = color.a;
    eRay->setShader("basicRay");
    nRay->setEntity(std::move(eRay.get()));
    sceneRoot->addChild(std::move(nRay.get()));
    eRay->setAspect(aspect);
    return eRay.get();
}

TRay* Inazuma::drawRay3D(Vec3 origen, Vec3 destino, _Color color){
    auto nodeRay = std::make_unique<TNode>();
    nodes.push_back(std::move(nodeRay));
    auto& nRay = nodes[nodes.size()-1];
    auto entityRay = std::make_unique<TRay>(&RM, origen, destino);
    rays.push_back(std::move(entityRay));
    auto& eRay = rays[rays.size()-1];
    eRay->VAO = rayVAO;
    eRay->VBO = rayVBO;
    eRay->color.r = color.r;
    eRay->color.g = color.g;
    eRay->color.b = color.b;
    eRay->color.a = color.a;
    eRay->is3D = true;
    eRay->setShader("basicRay");
    nRay->setEntity(std::move(eRay.get()));
    sceneRoot->addChild(std::move(nRay.get()));
    eRay->setAspect(aspect);
    return eRay.get();
}

// Metodo de renderizado global
void Inazuma::draw3D(){
    // Por si acaso
    glEnable(GL_DEPTH_TEST);
    // Renderizamos primero las luces, asi da igual el orden en los que las haya metido el usuario en el arbol
	for(auto& l : lights)
		l->draw(glm::mat4{1.0f}, sceneRoot->getPrincipalCamera());
    // Le decimos al shader cuantas luces hay de cada tipo
    basicShader->setInt("n_directionalLights", static_cast<int>(numLights.x));
    basicShader->setInt("n_pointLights", static_cast<int>(numLights.y));
    basicShader->setInt("n_spotLights", static_cast<int>(numLights.z));
    basicShader->setBool("shadowsActivated", shadows);

    if(skyboxModel)
        skyboxModel->draw(glm::mat4{1.0f}, sceneRoot->getPrincipalCamera());

    // Recorremos el arbol
    sceneRoot->traversal(glm::mat4{1.0f}, sceneRoot->getPrincipalCamera());

    glDisable(GL_DEPTH_TEST);
    for(auto& t : texts)
        renderAllText(t.text, t.x, t.y, t.scale, t.color);
    texts.clear();
    glEnable(GL_DEPTH_TEST);
}

// Redefinicion del metodo de OpenGL, permite hacer resize de la ventana manualmente
void framebuffer_size_callback([[maybe_unused]] GLFWwindow* window, int width, int height){
	glViewport(0, 0, width, height);
}

// Metodo para arrancar OpenGL
GLFWwindow* setupOpenGL(uint16_t width, uint16_t height, std::string name){
    // Inicio GLFW
    if(!glfwInit())
        std::cout << "Failed to initiate GLFW" << std::endl;
    // Le decimos a GLFW las veriones de OpenGL compatibles y el perfil que vamos a usar, el core
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	

    // Creamos la ventana con GLFW
	GLFWwindow* window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
    // Si no se crea la ventana, abortamos y lo decimos por terminal
	if(window == NULL){
		std::cout << "Failed to create window" << std::endl;
		glfwTerminate();
		return nullptr;
	}

    // Le decimos a GLFW que use la ventana creada
	glfwMakeContextCurrent(window);

    // Inicializamos GLAD con el contexto de GLFW
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
		std::cout << "Failed to initializa GLAD" << std::endl;
		return nullptr;
	}
    // Iniciamos el viewport de OpenGL
    glViewport(0, 0, width, height);
    // Le decimos a GLFW que use la funcion nuestra para cada vez que se intente cambiar el size de la ventana
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // Le decimos a STB que le de la vuelta a las imagenes cargadas, que por defecto se cargan siempre al reves 
    stbi_set_flip_vertically_on_load(true);
    // Le decimos a OpenGL que maneje las profundidades con el Z-Buffer
    glEnable(GL_DEPTH_TEST);
    // Activamos el Face Culling
    glEnable(GL_CULL_FACE);
    // Aunque viene por defecto, por si acaso, le decimos que solo lo haga con las caras traseras
    glCullFace(GL_BACK);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
    // Devolvemos la ventana
    return window;
}

void Inazuma::setupShadow(){
    // El objetivo es crear una textura con los valores de profundiad de los objetos de la escena renderizados
	// desde la perspectiva de las luces y luego usar eso para calcular las sombras

	// Creamos el buffer para el mapa de profundidades
	glGenFramebuffers(1, &depthMapFBO);

	// Cramos ahora las texturas
	glGenTextures(1, &depthMap);
    
	// La enlazamos
	glBindTexture(GL_TEXTURE_2D_ARRAY, depthMap);
	// Le asignamos la resolucion y le indicamos el tipo de textura como imagen
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F, SHADOW_WIDTH, SHADOW_HEIGHT, 50, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	// Le damos parametros a la textura, no es muy importante la fidelidad asi que usamos las mas sencillas de computar
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float bc[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, bc);

	// Enlazamos el buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	// Le enlazamos la textura 2D
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthMap, 0);
	// Solo necesitamos la informacion de la profundidad, por lo que le indicamos a OpenGL que no necesita espacio en el 
	// buffer para dibujar ningun color
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

    int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE){
        std::cout << "ERROR::FRAMEBUFFER::Framebuffer error::" << status << std::endl;
    }

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    glGenFramebuffers(1, &depthMapFBOCubes);
    glGenTextures(1, &depthCubemaps);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemaps);

    for(unsigned int i{0}; i<6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBOCubes);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemaps, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    
    status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE){
        std::cout << "ERROR::FRAMEBUFFER::Framebuffer error::" << status << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glGenBuffers(1, &matricesUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4x4) * 16, nullptr, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, matricesUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Inazuma::setupSkybox(){
    glGenTextures(1, &skybox);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox); glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void Inazuma::setupRay(){
    glGenVertexArrays(1, &rayVAO);
    glGenBuffers(1, &rayVBO);

    glBindVertexArray(rayVAO);
	glBindBuffer(GL_ARRAY_BUFFER, rayVBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glBindVertexArray(0);
}

void Inazuma::setupText(){
    FT_Library ft;

    if(FT_Init_FreeType(&ft))
        std::cout << "ERROR::FREETYPE::Error al iniciar la libreria FREETYPE" << std::endl;

    FT_Face face;
    FT_New_Memory_Face(ft, defaultFont, sizeof defaultFont, 0, &face);
    FT_Set_Pixel_Sizes(face, 0, 48);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    for (unsigned char c = 0; c < 128; c++){
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)){
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
    
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
    
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        character ch = {
            texture,
            static_cast<unsigned int>(face->glyph->advance.x),
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top)
        };
        characters.insert(std::pair<char, character>(c, ch));
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    glGenVertexArrays(1, &textVAO);
    glGenBuffers(1, &textVBO);
    glBindVertexArray(textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Inazuma::setupInputs(){
    for(uint16_t keys{32}; keys<349; keys+=1){
        inputs[keys] = InputStates{GLFW_RELEASE, GLFW_RELEASE};
    }
}

void Inazuma::renderAllText(std::string text, float x, float y, float scale, _Color color){
    basicTextShader->use();
    basicTextShader->setMat4("projection", glm::ortho(0.0f, static_cast<float>(WIDTH), -static_cast<float>(HEIGHT), 0.0f));
    if(scale <= 5.0f)
        scale = 5.0f;
    scale /= 48.0f;
    y=-y;
    basicTextShader->setVec4("textColor", color.r, color.g, color.b, color.a);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(textVAO);

    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++){
        character ch = characters[*c];

        float xpos = x + static_cast<float>(ch.Bearing.x) * scale;
        float ypos = y - static_cast<float>((ch.Size.y - ch.Bearing.y)) * scale;

        float w =  static_cast<float>(ch.Size.x) * scale;
        float h =  static_cast<float>(ch.Size.y) * scale;

        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },            
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }           
        };

        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        glBindBuffer(GL_ARRAY_BUFFER, textVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        x += static_cast<float>((ch.Advance >> 6)) * scale;
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Inazuma::loadSkybox(std::vector<std::string> faces){
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox); 
    int width{}, height{}, nrChannels{};
    for(uint16_t i{}; i<faces.size(); i++){
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if(data){
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }else{
            std::cout << "ERROR::SKYBOX::Source skybox images cannot load" << std::endl;
        }
        stbi_image_free(data);
    }
    skyLoaded = true;
    createSkybox();
}

bool isSomeKeyPressed(){
    for(uint16_t keys{32}; keys<349; keys+=1){
        if(glfwGetKey(window, keys) == GLFW_PRESS)
            return true;
    }
    return false;
}

// Usamos GLFW para controlar los inputs por teclado con una tecla por parametro 
bool isKeyPressed(uint16_t key){
    inputs[key].actualState = glfwGetKey(window, key);
    if(inputs[key].actualState == GLFW_PRESS && inputs[key].lastState != GLFW_PRESS){
        inputs[key].lastState = GLFW_PRESS;
        return true;
    }
    inputs[key].lastState = inputs[key].actualState;

    return false;
}

// Usamos GLFW para controlar los inputs por teclado con una tecla por parametro, de moemnto es igual que la de arriba
bool isKeyDown(uint16_t key){
    if(glfwGetKey(window, key) == GLFW_PRESS){
        return true;
    }
    return false;
}

// Limpia la pantalla ocn un color, como OpenGL lo tiene todo normalizado entre 0 y 1 toca dividir para conventir los valores RGBA
void cleanBackground(_Color c){
    // Como las divisiones son costosas, guardamos una copia del color y solo dividimos si el color que le pasas es diferente al ultimo
    if(c != cleanColor){
        // reasignamos los colores al cleanColor global
        cleanColor = _Color(c.r/255.0f, c.g/255.0f, c.b/255.0f, c.a/255.0f);
    }

    // Limpiamos la pantalla con el color de limpieza
    glClearColor(cleanColor.r, cleanColor.g, cleanColor.b, cleanColor.a);
    // Limpiamos tambien el buffer de color y el buffer de profundidad
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

_Color fade(_Color c, float alpha){
    c.a = alpha;
    return c;
}

void switchFullscreen(){
    if(fullscreen)
        glfwSetWindowMonitor(window, NULL, 0, 0, WIDTH, HEIGHT, 60);
    else
        glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, WIDTH, HEIGHT, 60);
    fullscreen = !fullscreen;
}

_RayCollision checkRaysCollisions(_Ray ray, _BoundingBox box){
    _RayCollision collision{};

    bool insideBox = (ray.position.x > box.min.x) && (ray.position.x < box.max.x) &&
                     (ray.position.y > box.min.y) && (ray.position.y < box.max.y) &&
                     (ray.position.z > box.min.z) && (ray.position.z < box.max.z);

    if(insideBox)
        ray.direction = Vec3(-ray.direction.x, -ray.direction.y, -ray.direction.z);

    float t[11]{};

    t[8] = 1.0f/ray.direction.x;
    t[9] = 1.0f/ray.direction.y;
    t[10] = 1.0f/ray.direction.z;

    t[0] = (box.min.x - ray.position.x)*t[8];
    t[1] = (box.max.x - ray.position.x)*t[8];
    t[2] = (box.min.y - ray.position.y)*t[9];
    t[3] = (box.max.y - ray.position.y)*t[9];
    t[4] = (box.min.z - ray.position.z)*t[10];
    t[5] = (box.max.z - ray.position.z)*t[10];
    t[6] = (float)fmax(fmax(fmin(t[0], t[1]), fmin(t[2], t[3])), fmin(t[4], t[5]));
    t[7] = (float)fmin(fmin(fmax(t[0], t[1]), fmax(t[2], t[3])), fmax(t[4], t[5]));

    collision.hit = !((t[7] < 0) || (t[6] > t[7]));
    collision.distance = t[6];

    Vec3 aux{ray.direction.x * collision.distance, ray.direction.y * collision.distance, ray.direction.z * collision.distance};

    collision.point = Vec3(ray.position.x + aux.x, ray.position.y + aux.y, ray.position.z + aux.z);

    collision.normal.x = box.min.x + 0.5f * (box.max.x - box.min.x);
    collision.normal.y = box.min.y + 0.5f * (box.max.y - box.min.y);
    collision.normal.z = box.min.z + 0.5f * (box.max.z - box.min.z);

    // Get vector center point->hit point
    collision.normal = Vec3(collision.point.x - collision.normal.x, collision.point.y - collision.normal.y, collision.point.z - collision.normal.z);
    // Scale vector to unit cube
    // NOTE: We use an additional .01 to fix numerical errors
    collision.normal = Vec3(collision.normal.x * 2.01f, collision.normal.y * 2.01f, collision.normal.z * 2.01f);
    
    aux = Vec3(box.max.x - box.min.x, box.max.y - box.min.y, box.max.z - box.min.z);

    collision.normal = Vec3(collision.normal.x/aux.x, collision.normal.y/aux.y, collision.normal.z/aux.z);
    // The relevant elements of the vector are now slightly larger than 1.0f (or smaller than -1.0f)
    // and the others are somewhere between -1.0 and 1.0 casting to int is exactly our wanted normal!
    collision.normal.x = (float)((int)collision.normal.x);
    collision.normal.y = (float)((int)collision.normal.y);
    collision.normal.z = (float)((int)collision.normal.z);

    aux = collision.normal;

    float length = sqrtf(aux.x*aux.x + aux.y*aux.y + aux.z*aux.z);
    if(length != 0.0f){
        float ilength = 1.0f/length;
        collision.normal.x *= ilength;
        collision.normal.y *= ilength;
        collision.normal.z *= ilength;
    }

    if(insideBox){
        ray.direction = Vec3(-ray.direction.x, -ray.direction.y, -ray.direction.z);
        collision.distance *= -1.0f;
        collision.normal = Vec3(-collision.normal.x, -collision.normal.y, -collision.normal.z);
    }

    return collision;
}

_Texture2D Inazuma::loadTexture(std::string path, TextureTypes type, bool flip){
    stbi_set_flip_vertically_on_load(flip);
    _Texture2D textRes{};
    bool skip{};

    for(unsigned int j{}; j<RM.texturesLoaded.size(); j++){
		if(std::strcmp(RM.texturesLoaded.at(j).path.data(), path.c_str())==0){
			textRes.id = RM.texturesLoaded.at(j).id;
			textRes.type = RM.texturesLoaded.at(j).type;
			skip = true;
			break;
		}
	}

    if(!skip){

        switch(type){
            case TextureTypes::difuse:
                textRes.type = "texture_diffuse";
                break;
            case TextureTypes::specular:
                textRes.type = "texture_specular";
                break;
        }

	    glGenTextures(1, &textRes.id);
	    //replaceString(path, "\\", '/');
	    unsigned char* data = stbi_load(path.c_str(), &textRes.width, &textRes.height, &textRes.nComp, 0);
        
	    if(data){
		    GLenum format{};
		    if(textRes.nComp == 1)
		    	format = GL_RED;
		    else if(textRes.nComp == 3)
		    	format = GL_RGB;
		    else if(textRes.nComp == 4)
		    	// Si tiene cuatro, RGB y un canal alpha
		    	format = GL_RGBA;
		    // Anidamos la textura
		    glBindTexture(GL_TEXTURE_2D, textRes.id);
		    // Cargamos la imagen
		    glTexImage2D(GL_TEXTURE_2D, 0, format, textRes.width, textRes.height, 0, format, GL_UNSIGNED_BYTE, data);

		    // Ponemos los parametros para los mimaps y las texturas
		    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		    // Generamos los mipmaps
		    glGenerateMipmap(GL_TEXTURE_2D);

            _Texture texture{};
			texture.id = textRes.id;
			texture.type = textRes.type;
			texture.path = path;
			RM.texturesLoaded.push_back(texture);

		    stbi_image_free(data);
		}else{
			std::cout << "_Texture failed to load in: " << path << std::endl;
			stbi_image_free(data);
		}
    }

    stbi_set_flip_vertically_on_load(false);
	return textRes;
}

_Texture Inazuma::loadTextureToMesh(std::string path, TextureTypes type){
    stbi_set_flip_vertically_on_load(true);
    _Texture textRes{};
    bool skip{};

    for(unsigned int j{}; j<RM.texturesLoaded.size(); j++){
		if(std::strcmp(RM.texturesLoaded.at(j).path.data(), path.c_str())==0){
			textRes.id = RM.texturesLoaded.at(j).id;
			textRes.type = RM.texturesLoaded.at(j).type;
			skip = true;
			break;
		}
	}

    if(!skip){

        switch(type){
            case TextureTypes::difuse:
                textRes.type = "texture_diffuse";
                break;
            case TextureTypes::specular:
                textRes.type = "texture_specular";
                break;
        }

	    glGenTextures(1, &textRes.id);
        int width{}, height{}, nComp{}; 
	    //replaceString(path, "\\", '/');
	    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nComp, 0);
	    if(data){
		    GLenum format{};
		    if(nComp == 1)
		    	format = GL_RED;
		    else if(nComp == 3)
		    	format = GL_RGB;
		    else if(nComp == 4)
		    	// Si tiene cuatro, RGB y un canal alpha
		    	format = GL_RGBA;
		    // Anidamos la textura
		    glBindTexture(GL_TEXTURE_2D, textRes.id);
		    // Cargamos la imagen
		    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

		    // Ponemos los parametros para los mimaps y las texturas
		    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		    // Generamos los mipmaps
		    glGenerateMipmap(GL_TEXTURE_2D);

			textRes.path = path;
			RM.texturesLoaded.push_back(textRes);

		    stbi_image_free(data);
		}else{
			std::cout << "_Texture failed to load in: " << path << std::endl;
			stbi_image_free(data);
		}
    }

    stbi_set_flip_vertically_on_load(false);
	return textRes;
}

/////////////////////////////////////////////////////////////////
//                      SCENETREE.HPP                          //
/////////////////////////////////////////////////////////////////

// Anade el hijo pasado como parametro al nodo actual y devuelve la cantidad de hijos
uint32_t TNode::addChild(TNode* newNode){
    childs.emplace_back();
    childs[childs.size()-1] = std::move(newNode);
    return static_cast<uint32_t>(childs.size());
}

// Elimina del array de hhijos el hijo pasado como parametro al nodo actual y devuelve la posicion donde estaba y -1 si no ha borrado
uint32_t TNode::removeChild(TNode* remNode){
    // Comprueba por cada hijo si el puntero a entidad es igual
    for(uint32_t i{}; i<=childs.size(); i++){
        if(childs[i]->getEntity() == remNode->getEntity()){
            childs.erase(childs.begin()+i);
            return i;
        }
    }
    return -1;
}

// Asigna una entidad y devuelve true o false si lo ha hecho o no
bool TNode::setEntity(TEntity* newEntity){
    bool res{};
    if(newEntity!=nullptr){
        entity = std::move(newEntity);
        res = true;
    }
    return res;
}

// Actualiza la matriz de transformacion con la funcion translate de GLM y marca que lo ha actualizado
void TNode::translate(Vec3 toTranslate){
    transMatrix = glm::translate(transMatrix, glm::vec3(toTranslate.x, toTranslate.y, toTranslate.z));
    updated = true;
}

// Actualiza la matriz de transformacion con la funcion rotate de GLM y marca que lo ha actualizado
void TNode::rotate(Vec4 toRotate){
    transMatrix = glm::rotate(transMatrix, glm::radians(toRotate.w), glm::vec3{toRotate.x, toRotate.y, toRotate.z});
    updated = true;
}

// Actualiza la matriz de transformacion con la funcion scale de GLM y marca que lo ha actualizado
void TNode::scale(Vec3 toScale){
    transMatrix = glm::scale(transMatrix, glm::vec3(toScale.x, toScale.y, toScale.z));
    updated = true;
}

// Recorrido del arbol
void TNode::traversal(glm::mat4 cumulativeMatrix, TCamera* prCamera){
    // Si se ha actualizado, le pasa la matriz acumulada y recalcula la matriz de transformacion
    if(updated){
        transMatrix = cumulativeMatrix * transMatrix;
        updated = false;
    }
    // Si tiene entidad y esta entidad tiene un puntero a un ResourceManager que existe, le manda a dibujar. Esto se hace
    // para evitar dibujar las luces y las camaras aqui.
    if(entity && entity->RM && !entity->skybox)
        entity->draw(transMatrix, prCamera);
    // Recursivamente llamamos a los hijos
    for(auto& c : childs)
        c->traversal(transMatrix, prCamera);
}

void TNode::renderShadows(glm::mat4 cumulativeMatrix, _Shader* shader){
    // Si se ha actualizado, le pasa la matriz acumulada y recalcula la matriz de transformacion
    if(updated){
        transMatrix = cumulativeMatrix * transMatrix;
        updated = false;
    }
    // Si tiene entidad y esta entidad tiene un puntero a un ResourceManager que existe, le manda a dibujar. Esto se hace
    // para evitar dibujar las luces y las camaras aqui.
    uint16_t aux{};
    if(entity && entity->RM)
        entity->drawShadowArray(shader, aux, transMatrix);
    // Recursivamente llamamos a los hijos
    for(auto& c : childs)
        c->renderShadows(transMatrix, shader);
}

/////////////////////////////////////////////////////////////////
//                     TRESOURCEMANAGER.HPP                    //
/////////////////////////////////////////////////////////////////

// STB necesita incluirse en un cpp y compilarse para funcionar, asi que lo incluyo asi. Ademas tambien necesita que le  
// defina que necesito la parte de el tratamiento de imagenes
#define STB_IMAGE_IMPLEMENTATION
//#include "../include/stb_image.h"
#include "../stb_image.h"

// Llamada a la funcion de recoger recurso segun un modelo
TR_Model* TResourceManager::getModel(std::string name, std::string src, RType rt){
    return getResource<TR_Model>(name, src, rt);
}

// Llamada a la funcion de recoger recurso segun un shader
TR_Shader* TResourceManager::getShader(std::string name, std::string vs, std::string fs){
    return getResource<TR_Shader>(name, vs, fs);
}

TR_Shader* TResourceManager::loadBasicShader(std::string name){
    auto newRec = std::make_unique<TR_Shader>();
    newRec->setName(name);
    newRec->setScene(scene);
    newRec->shader.loadBasicShader(name);
    resources.push_back(std::move(newRec));
    return dynamic_cast<TR_Shader*>(resources[resources.size()-1].get());
}

/////////////////////////////////////////////////////////////////
//                         SHADER.HPP                          //
/////////////////////////////////////////////////////////////////

using namespace std;

// Constructor que lee y renderiza 
_Shader::_Shader(const char* vertexPath, const char* fragmentPath){
    // Seteamos las variables iniciales de almacenamiento y lectura
	string vertexCode{}, fragmentCode{};
	ifstream vShaderFile{}, fShaderFile{};
	// Permitimos a los ifstream lanzar excepciones
	vShaderFile.exceptions (ifstream::failbit | ifstream::badbit);
	fShaderFile.exceptions (ifstream::failbit | ifstream::badbit);
	try{
		// Abrimos los archivos
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		// Creamos as variables para el flujo de datos
		stringstream vShaderStream{}, fShaderStream{};
		// Cargamos el buffer de los archivos en los flujos
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// Cerramos los archivos
		vShaderFile.close();
		fShaderFile.close();
		// Convertimos el flujo de datos en strings
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}catch(ifstream::failure& e){
		cout << "ERROR::SHADER::FILE_NO_READED" << endl;
	}
	// Lo pasamos a cadenas de char y nos llevamos el puntero donde empiezan
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();
	// Ahora con todod leido vamos a compilar y linkar
	unsigned int vertex, fragment;
	int success;
	char infoLog[512];

	// VertexShader
	// Inicializamos el VS
	vertex = glCreateShader(GL_VERTEX_SHADER);
	// Le decimos cual es el codigo 
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	// Compilamos
	glCompileShader(vertex);
	// Miramos su status para ver si la compilacion ha sido exitosa
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	// Si no lo ha sido, obtenemos el Log de error y lanzamos un mensaje
	if(!success){
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		cout << "ERROR::SHADER::VERTEX::COMPLIATION_FAILED::" << vertexPath << "::" << infoLog << endl;
	}

	// FragmentShader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if(!success){
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		cout << "ERROR::SHADER::FRAGMENT::COMPLIATION_FAILED" << infoLog << endl;
	}

	// ShaderProgram
	// Creamos el SP, le unimos los shader y linkamos
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	// Comprobamos que haya salido bien
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if(!success){
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		cout << "ERROR::SHADER::PROGRAM::LINKER_FAILED" << infoLog << endl;
	}
	// Limpiamos
	glDeleteShader(vertex);
	glDeleteShader(fragment);
};

// Le decimos a OpenGL que use el shader
void _Shader::use(){
	glUseProgram(ID);
}

void _Shader::loadBasicShader(std::string name){
    // Si el path del vertex y el fragment es "base", cargamos la informacion desde nuestros punteros basicos
    const char* vs{};
    const char* fs{};
    const char* gs{};
	if(name == "base"){
        vs = basicVertexShader;
        fs = basicFragmentShader;
        gs = NULL;
	}else if( name == "baseShadow"){
        vs = basicShadowVertexShader;
        fs = basicShadowFragmentShader;
        gs = basicShadowGeometryShader;
	}else if(name == "baseShadowCubes"){
        vs = basicShadowVertexShaderCubes;
        fs = basicShadowFragmentShaderCubes;
        gs = basicShadowGeometryShaderCubes;
    }else if(name == "basicSkybox"){
        vs = basicSkyboxVertex;
        fs = basicSkyboxFragment;
        gs = NULL;
    }else if(name == "basic2D"){
        vs = basic2DVertex;
        fs = basic2DFragment;
        gs = NULL;
    }else if(name == "basicText"){
        vs = basicTextVertex;
        fs = basicTextFragment;
        gs = NULL;
    }else if(name == "basicRay"){
        vs = basicRayVertex;
        fs = basicRayFragment;
        gs = NULL;
    }

	unsigned int vertex, fragment, geometry;
	int success;
	char infoLog[512];
	// VertexShader
	// Inicializamos el VS
	vertex = glCreateShader(GL_VERTEX_SHADER);
	// Le decimos cual es el codigo 
	glShaderSource(vertex, 1, &vs, NULL);
	// Compilamos
	glCompileShader(vertex);
	// Miramos su status para ver si la compilacion ha sido exitosa
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	// Si no lo ha sido, obtenemos el Log de error y lanzamos un mensaje
	if(!success){
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		cout << "ERROR::SHADER::VERTEX::COMPLIATION_FAILED::" << name << "::" << infoLog << endl;
	}

	// FragmentShader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fs, NULL);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if(!success){
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		cout << "ERROR::SHADER::FRAGMENT::COMPLIATION_FAILED::" << name << "::" << infoLog << endl;
	}

    if(gs != NULL){
        // GeomtryShader
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &gs, NULL);
		glCompileShader(geometry);
		glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
		if(!success){
			glGetShaderInfoLog(geometry, 512, NULL, infoLog);
			cout << "ERROR::SHADER::GEOMETRY::COMPLIATION_FAILED::" << name << "::" << infoLog << endl;
		}
    }


    // ShaderProgram
    // Creamos el SP, le unimos los shader y linkamos
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    if(gs != NULL)
        glAttachShader(ID, geometry);
    glLinkProgram(ID);
    // Comprobamos que haya salido bien
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if(!success){
    	glGetProgramInfoLog(ID, 512, NULL, infoLog);
    	cout << "ERROR::SHADER::PROGRAM::LINKER_FAILED::" << infoLog << endl;
    }
    // Limpiamos
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if(gs != NULL)
        glDeleteShader(geometry);
}

// Le asignamos un bool a un uniform pasado por parametro
void _Shader::setBool(const string &name, bool value) const{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), static_cast<int>(value));
}

// Le asignamos un int a un uniform pasado por parametro
void _Shader::setInt(const string &name, int value) const{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

// Le asignamos un float a un uniform pasado por parametro
void _Shader::setFloat(const string &name, float value) const{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

// Le asignamos un vector 2 a un uniform pasado por parametro usando otro vector 2
void _Shader::setVec2(const std::string &name, const glm::vec2 &value) const{ 
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
}

// Le asignamos un vector 2 a un uniform pasado por parametro usando dos floats
void _Shader::setVec2(const std::string &name, float x, float y) const{ 
    glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y); 
}

// Le asignamos un vector 3 a un uniform pasado por parametro usando otro vector 3
void _Shader::setVec3(const std::string &name, const glm::vec3 &value) const{ 
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
}

// Le asignamos un vector 3 a un uniform pasado por parametro usando tres floats
void _Shader::setVec3(const std::string &name, float x, float y, float z) const{ 
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z); 
}

// Le asignamos un vector 4 a un uniform pasado por parametro usando otro vector 4
void _Shader::setVec4(const std::string &name, const glm::vec4 &value) const{ 
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
}

// Le asignamos un vector 4 a un uniform pasado por parametro usando cuatro floats
void _Shader::setVec4(const std::string &name, float x, float y, float z, float w) const{ 
    glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w); 
}

// Le asignamos una matriz 2x2 a un uniform pasado por parametro
void _Shader::setMat2(const std::string &name, const glm::mat2 &mat) const{
    glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

// Le asignamos una matriz 3x3 a un uniform pasado por parametro
void _Shader::setMat3(const std::string &name, const glm::mat3 &mat) const{
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

// Le asignamos una matriz 4x4 a un uniform pasado por parametro
void _Shader::setMat4(const std::string &name, const glm::mat4 &mat) const{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}




void Animation::readMissingBones(const aiAnimation* anim, TR_Model* model){
    int size = anim->mNumChannels;
    auto& boneInfoMap = model->getBoneInfoMap();
    int& numBones = model->getNumBones();

    for(int i{}; i < size; i+=1){
        auto channel = anim->mChannels[i];
        std::string boneName = channel->mNodeName.data;
        if(boneInfoMap.find(boneName) == boneInfoMap.end()){
            boneInfoMap[boneName].id = numBones;
            numBones += 1;
        }
        aBones.push_back(Bone(channel->mNodeName.data, boneInfoMap[channel->mNodeName.data].id, channel));
    }

    aBoneInfoMap = boneInfoMap;
}