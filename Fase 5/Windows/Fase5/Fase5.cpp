#include "Fase5.hpp"
#include <SceneTree.hpp>

// La ventana principal
GLFWwindow* window{};
// La raíz del árbol
std::unique_ptr<Node> sceneRoot{};
// Instancia del gestor de recursos
ResourceManager RM{};
// Nos guardamos un puntero a nuestro shader básico
Shader* basicShader{};
Shader* skyboxShader{};
std::unique_ptr<E_Skybox> skyboxModel{};

// La relación de aspecto, que será usada en varias partes
float aspect{ static_cast<float>(ANCHO) / static_cast<float>(ALTO) };

// Variables para deltaTime
float lastFrame{};
float deltaTime{};

unsigned int skybox{};

// Vectores para almacenar los punteros a los nodos, camaras y modelos
std::vector<std::unique_ptr<Node>> nodes{};
std::vector<std::unique_ptr<E_Camera>> cameras{};
std::vector<std::unique_ptr<E_Model>> models{};
// Vector para guardar las luces
std::vector<std::unique_ptr<E_Light>> lights{};
// Aprovechamos un vector 3 para saber cuántas luces hay de cada tipo
// (X direcionales, Y puntuales y Z focales)
glm::vec3 numLights{};

// Los métodos para crear cubos y para crear cámaras
Node* createCube(glm::vec3 pos, glm::vec3 size, Color color, float rotAngle, glm::vec3 rotAxis, bool texture) {
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
	nCube->translate(glm::vec3(pos.x, pos.y, pos.z));
	nCube->rotate(glm::vec4(rotAxis.x, rotAxis.y, rotAxis.z, rotAngle));
	nCube->scale(glm::vec3(size.x, size.y, size.z));
	// Le ponemos el color y el aspect ratio
	eCube->color = color;
	eCube->setAspect(aspect);
	// Guardamos la entidad dentro del nodo
	nCube->setEntity(eCube.get());
	// Lo añadimos a la escena
	sceneRoot->addChild(std::move(nCube.get()));
	// Recuperamos el nodo para el usuario
	return sceneRoot->getChildren()[sceneRoot->getChildren().size() - 1];
}

Node* createCamera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) {
	// Creamos un nodo como un puntero unico
	auto nodeCam = std::make_unique<Node>();
	// Lo pusheamos al vector de nodos y le pasamos la responsabilidad de memoria
	nodes.push_back(std::move(nodeCam));
	// Lo recuperamos como una referencia para trabajar con el
	auto& nCam = nodes[nodes.size() - 1];
	// Creamos la entidad con los parámetros
	auto entityCam = std::make_unique<E_Camera>(position, up, yaw, pitch);
	// Lo pusheamos y le pasamos la responsabilidad de memoria
	cameras.push_back(std::move(entityCam));
	// La recuperamos como referencia
	auto& eCam = cameras[cameras.size() - 1];
	// Si no existe ya una, la asigna como camara principal
	if(sceneRoot->getPrincipalCamera() == NULL){
		sceneRoot->setPrincipalCamera(eCam.get());
	}
	// Le asigna al nodo la entidad
	nCam->setEntity(eCam.get());
	// Lo anadimos a la raiz como hijo, para que forme parte de la escena
	sceneRoot->addChild(nCam.get());
	// Devuelve un puntero al nodo, para que el usuario pueda jugar con él
	return sceneRoot->getChildren()[sceneRoot->getChildren().size() - 1];
}

// Los métodos para crear las luces
Node* createDirectionalLight(glm::vec3 dir, glm::vec3 amb, glm::vec3 dif, glm::vec3 spc, std::string shader) {
	auto nodeLight = std::make_unique<Node>();
	nodes.push_back(std::move(nodeLight));
	auto& nLight = nodes[nodes.size() - 1];
	auto entityLight = std::make_unique<E_Light>();
	lights.push_back(std::move(entityLight));
	auto& eLight = lights[lights.size() - 1];
	// Como ya estará cargado en memoria el gestor de recurso nos devolverá su ID de OpenGL
	eLight->shader = &RM.getShader(shader, shader, shader)->shader;
	// Crea una luz direccional en la entidad luz con los parametros
	eLight->setDirectionalLight(dir, amb, dif, spc);
	// Marcamos la id como el numero actual de luces direccionales
	eLight->id = static_cast<int>(numLights.x);
	nLight->setEntity(std::move(eLight.get()));
	sceneRoot->addChild(std::move(nLight.get()));
	// Aumenta el contador de luces direccionales 
	numLights.x += 1;
	return sceneRoot->getChildren()[sceneRoot->getChildren().size() - 1];
}

Node* createPointLight(glm::vec3 pos, glm::vec3 amb, glm::vec3 dif, 
  glm::vec3 spc, glm::vec3 att, std::string shader) {
	auto nodeLight = std::make_unique<Node>();
	nodes.push_back(std::move(nodeLight));
	auto& nLight = nodes[nodes.size() - 1];
	auto entityLight = std::make_unique<E_Light>();
	lights.push_back(std::move(entityLight));
	auto& eLight = lights[lights.size() - 1];
	eLight->shader = basicShader;
	// Crea una luz puntual en la entidad luz con los parametros
	eLight->setPointLight(pos, amb, dif, spc, att);
	// Esta vez usamos el numero de luces puntuales
	eLight->id = static_cast<int>(numLights.y);
	nLight->setEntity(std::move(eLight.get()));
	sceneRoot->addChild(std::move(nLight.get()));
	// Esta vez aumentamos el numero de luces puntuales
	numLights.y += 1;
	return sceneRoot->getChildren()[sceneRoot->getChildren().size() - 1];
}

Node* createSpotLight(glm::vec3 pos, glm::vec3 dir, glm::vec3 amb, glm::vec3 dif, glm::vec3 spc, 
  glm::vec3 att, glm::vec2 cut, std::string shader) {
	auto nodeLight = std::make_unique<Node>();
	nodes.push_back(std::move(nodeLight));
	auto& nLight = nodes[nodes.size() - 1];
	auto entityLight = std::make_unique<E_Light>();
	lights.push_back(std::move(entityLight));
	auto& eLight = lights[lights.size() - 1];
	eLight->shader = basicShader;
	// Crea una luz focal en la entidad luz con los parametros
	eLight->setSpotLight(pos, dir, amb, dif, spc, att, cut);
	// Esta vez aumentamos el numero de luces focales
	eLight->id = static_cast<int>(numLights.z);
	nLight->setEntity(std::move(eLight.get()));
	sceneRoot->addChild(std::move(nLight.get()));
	// Esta vez aumentamos el numero de luces focales
	numLights.z += 1;
	return sceneRoot->getChildren()[sceneRoot->getChildren().size() - 1];
}

// Carga del modelo 3D
Node* createModel(std::string path, glm::vec3 pos, glm::vec3 size, float rotAngle, glm::vec3 rotAxis, std::string shader) {
	auto nodeModel = std::make_unique<Node>();
	nodes.push_back(std::move(nodeModel));
	auto& nModel = nodes[nodes.size() - 1];
	auto entityModel = std::make_unique<E_Model>(&RM, path, RType::RModel);
	models.push_back(std::move(entityModel));
	auto& eModel = models[models.size() - 1];
	// Si el shader no es el basico, lo setea (este metodo lo buscara en el resource manager)
	if (shader != "default")
		eModel->setShader(shader);
	else
		eModel->setShader("default");
	// Translada el modelo a la posicion por parametro
	nModel->translate(pos);
	// Luego lo rota
	nModel->rotate(glm::vec4(rotAxis.x, rotAxis.y, rotAxis.z, rotAngle));
	// Y luego lo escala
	nModel->scale(size);
	nModel->setEntity(std::move(eModel.get()));
	// Le ajusta la relacion de aspecto
	eModel->setAspect(aspect);
	sceneRoot->addChild(std::move(nModel.get()));
	return sceneRoot->getChildren()[sceneRoot->getChildren().size() - 1];
}

Node* loadSkybox() {
	auto nodeCube = std::make_unique<Node>();
	nodes.push_back(std::move(nodeCube));
	auto& nCube = nodes[nodes.size() - 1];
	// Como siempre va a ser igual le pasamos ya las cosas por el constructor
	auto entityModel = std::make_unique<E_Skybox>(&RM, "Cubo", RType::RCube);
	skyboxModel = std::move(entityModel);
	auto& eCube = skyboxModel;
	// El cubo siempre va a tener solo un shader, el base
	eCube->setShader("skybox");
	eCube->setAspect(aspect);
	nCube->setEntity(std::move(eCube.get()));
	sceneRoot->addChild(std::move(nCube.get()));
	return nCube.get();
}

Node* createSkybox(std::vector<std::string> faces) {
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
	return loadSkybox();
}

// El color del fondo, para evitar recalcularlo si es el mismo
Color colorFondo;

// Método donde encapsulamos la responsabilidad de limpiar el fondo
void cleanBackground(Color c) {
	// Como las divisiones son costosas, guardamos una copia del color y solo dividimos si el color que le pasas es diferente al ultimo
	if (c != colorFondo) {
		// Reasignamos los colores al colorFondo global, en RGBA para facilitarselo al usuario
		colorFondo = Color(c.r / 255.0f, c.g / 255.0f, c.b / 255.0f, c.a / 255.0f);
	}

	// Limpiamos la pantalla con el color de limpieza
	glClearColor(colorFondo.r, colorFondo.g, colorFondo.b, colorFondo.a);
	// Limpiamos el buffer de color y el buffer de profundidad
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

// Procesamos los inputs del teclado de los usuarios con GLFW
void processInput() {
	// El movimiento de la cámara con WASD, usando los enumerados de GLFW y glfwGetKey
	if(glfwGetKey(window, GLFW_KEY_W)) {
		// Recuperamos la cámara de nuestro vector y usamos nuestro método de movimiento
		cameras.at(0)->processKeyboard(FORWARD, deltaTime);
	}
	if(glfwGetKey(window, GLFW_KEY_S)) {
		cameras.at(0)->processKeyboard(BACKWARD, deltaTime);
	}
	if(glfwGetKey(window, GLFW_KEY_A)) {
		cameras.at(0)->processKeyboard(LEFT, deltaTime);
	}
	if(glfwGetKey(window, GLFW_KEY_D)) {
		cameras.at(0)->processKeyboard(RIGHT, deltaTime);
	}
	// Escape para cerrar la ventana.
	if(glfwGetKey(window, GLFW_KEY_ESCAPE)) {
		// Con esto, saldrá del bucle de renderizado
		glfwSetWindowShouldClose(window, true);
	}
}

// Guardamos las posiciones anteriores del ratón
float lastX{}, lastY{};
// Si es la primera lectura del ratón
bool firstMouse{ true };

// Procesamos los inputs del movimiento del ratón
void movimientoRaton(GLFWwindow* window, double xPosIn, double yPosIn) {
	// Guardamos la posición que nos pasa GLFW
	float xPos{ static_cast<float>(xPosIn) };
	float yPos{ static_cast<float>(yPosIn) };

	// Si es la primera vez que leemos el ratón, la guardamos antes de actualizarlas
	if (firstMouse) {
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	// Calculamos la diferencia entre la posicion actual y la anterior en ambos ejes
	float xOffset{ xPos - lastX };
	float yOffset{ lastY - yPos };
	// Actualizamos
	lastX = xPos;
	lastY = yPos;

	// Recuperamos la cámara principal, en el caso de que haya
	if (cameras.at(0) != NULL)
		// Con la diferencia entre posiciones, llamamos a nuestro método de movimiento del ratón
		cameras.at(0)->processMouseMovement(xOffset, yOffset);
}

// Procesamos el input de la rueda del ratón, necesita X e Y por compatibilidad con dispositivos raros
void zoomRaton(GLFWwindow* window, double xOffset, double yOffset) {
	// Si tiene cámara principal, la usamos
	if (cameras.at(0) != NULL)
		// A nosotros obviamente sólo nos interesa el movimiento vertical
		cameras.at(0)->processMouseScroll(static_cast<float>(yOffset));
}

// Método donde encapsulas la responsabilidad del setup de OpenGL
bool configurarOpenGL() {
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(ANCHO, ALTO, NOMBRE, NULL, NULL);
	if (window == NULL) {
		std::cout << "Ha fallado la creacion de la ventana" << std::endl;
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Ha fallado la inicializacion de GLAD" << std::endl;
		return false;
	}

	glfwSetCursorPosCallback(window, movimientoRaton);
	glfwSetScrollCallback(window, zoomRaton);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glEnable(GL_DEPTH_TEST);
	// Activamos el Face Culling
	glEnable(GL_CULL_FACE);
	// Aunque viene por defecto, por si acaso, le decimos que solo lo haga con las caras traseras
	glCullFace(GL_BACK);

	glGenTextures(1, &skybox);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox); glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glViewport(0, 0, ANCHO, ALTO);
}

int main() {
	// Llamamos a nuestra función que configura OpenGL
	if (!configurarOpenGL())
		return -1;

	// Iniciamos la raiz del arbol de la escena
	sceneRoot = std::make_unique<Node>();
	// Le decimos a nuestro Resource Manager que esta es nuestra raíz
	RM.setScene(sceneRoot.get());
	// Creamos la cámara principal
	Node* camara = createCamera(glm::vec3(-0.3f, 0.0f, 5.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);
	// Leemos los shaders, primero el default
	basicShader = &RM.getShader("default", "./Shaders/DefaultVertexShader.vs", "./Shaders/DefaultFragmentShader.fs")->shader;
	// Luego el del skybox
	skyboxShader = &RM.getShader("skybox", "./Shaders/SkyboxVertexShader.vs", "./Shaders/SkyboxFragmentShader.fs")->shader;


	// Creamos las luces
	createDirectionalLight(
		glm::vec3(-0.2f, -1.0f, -0.3f), glm::vec3(0.7f, 0.7f, 0.7f),
		glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(0.2f, 0.2f, 0.2f), "default");
	createPointLight(
		glm::vec3(-3.8f, 0.0f, -12.3f), glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.8f, 0.8f, 0.8f),
		glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 0.09f, 0.032f), "default");
	auto* spotLight = createSpotLight(
		cameras.at(0)->position, cameras.at(0)->front,
		glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 0.09f, 0.032f), glm::vec2(glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(15.0f))), "default");

	// Creamos los cubos, igual que en la Fase2
	std::vector<Node*> cubos{};
	Node* cubo1  = createCube(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), Color(1.0f, 0.0f, 0.0f, 1.0f),  45.0f, glm::vec3(1.0f, 0.0f, 0.0f), true);
	Node* cubo2  = createCube(glm::vec3(2.0f, 5.0f, -15.0f), glm::vec3(1.0f, 1.0f, 1.0f), Color(0.0f, 1.0f, 0.0f, 1.0f), 45.0f, glm::vec3(1.0f, 0.0f, 0.0f), false);
	Node* cubo3  = createCube(glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 1.0f, 1.0f),  45.0f, glm::vec3(1.0f, 0.0f, 0.0f), false);
	Node* cubo4  = createCube(glm::vec3(-3.8f, -2.0f, -12.3f), glm::vec3(1.0f, 1.0f, 1.0f), Color(1.0f, 1.0f, 0.0f, 1.0f),  45.0f, glm::vec3(1.0f, 0.0f, 0.0f), false);
	Node* cubo5  = createCube(glm::vec3(2.4f, -0.4f, -3.5f), glm::vec3(1.0f, 1.0f, 1.0f), Color(0.0f, 1.0f, 1.0f, 1.0f),  45.0f, glm::vec3(1.0f, 0.0f, 0.0f), false);
	Node* cubo6  = createCube(glm::vec3(-1.7f, 3.0f, -7.5f), glm::vec3(1.0f, 1.0f, 1.0f), Color(1.0f, 0.0f, 1.0f, 1.0f),  45.0f, glm::vec3(1.0f, 0.0f, 0.0f), false);
	Node* cubo7  = createCube(glm::vec3(1.3f, -2.0f, -2.5f), glm::vec3(1.0f, 1.0f, 1.0f), Color(1.0f, 1.0f, 1.0f, 1.0f), 45.0f, glm::vec3(1.0f, 0.0f, 0.0f), false);
	Node* cubo8  = createCube(glm::vec3(1.5f, 2.0f, -2.5f), glm::vec3(1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f), 45.0f, glm::vec3(1.0f, 0.0f, 0.0f), false);
	Node* cubo9  = createCube(glm::vec3(1.5f, 0.2f, -1.5f), glm::vec3(1.0f, 1.0f, 1.0f), Color(0.5f, 0.5f, 0.5f, 1.0f), 45.0f, glm::vec3(1.0f, 0.0f, 0.0f), false);
	Node* cubo10 = createCube(glm::vec3(-1.3f, 1.0f, -1.5f), glm::vec3(1.0f, 1.0f, 1.0f), Color(1.0f, 0.5f, 0.0f, 1.0f), 45.0f, glm::vec3(1.0f, 0.0f, 0.0f), false);
	
	// Nos guardamos los cubos para iterar luego
	cubos.push_back(cubo1);
	cubos.push_back(cubo2);
	cubos.push_back(cubo3);
	cubos.push_back(cubo4);
	cubos.push_back(cubo5);
	cubos.push_back(cubo6);
	cubos.push_back(cubo7);
	cubos.push_back(cubo8);
	cubos.push_back(cubo9);
	cubos.push_back(cubo10);

	// Cargamos la textura para el cubo rojo, el primero
	dynamic_cast<E_Model*>(cubo1->getEntity())->setTexture("./Resources/Caja.png");

	Node* nivel = createModel("./resources/Mapa.obj", glm::vec3(-1.0f, -3.5f, 15.0f), 
		glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f), "default");

	// Cargamos la textura para el cubo rojo, el primero
	dynamic_cast<E_Model*>(nivel->getEntity())->setTexture("./Resources/Mapa.png");

	Node* personaje = createModel("./resources/Personaje.fbx", glm::vec3(-5.0f, -1.5f, 0.0f),
		glm::vec3(0.01f, 0.01f, 0.01f), -90.0f, glm::vec3(1.0f, 0.0f, 0.0f), "default");

	// Cargamos la textura para el cubo rojo, el primero
	dynamic_cast<E_Model*>(personaje->getEntity())->setTexture("./Resources/Personaje.png");

	std::vector<std::string> caras{};
	caras.push_back("./Resources/Skybox/right.jpg");
	caras.push_back("./Resources/Skybox/left.jpg");
	caras.push_back("./Resources/Skybox/top.jpg");
	caras.push_back("./Resources/Skybox/bottom.jpg");
	caras.push_back("./Resources/Skybox/front.jpg");
	caras.push_back("./Resources/Skybox/back.jpg");
	createSkybox(caras);

	// Inicializamos el bucle de renderizado
	while (!glfwWindowShouldClose(window)) {
		// Controlamos el input del usuario
		processInput();
		// Limpiamos el fondo
		cleanBackground(Color(100.0f, 100.0f, 100.0f, 255.0f));

		// Calculamos el deltaTime
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Iteramos por cada cubo
		for (unsigned int i = 0; i < 10; i++) {
			// Lo mismo que la fase2, pero usando los nodos de nuestro árbol
			Node* cubo = cubos.at(i);
			float frameAngle{};
			float rotationSpeed{40.0f};
			if (i > 0) {
				frameAngle = deltaTime * rotationSpeed * static_cast<float>(i);
			}
			else {
				frameAngle = deltaTime * rotationSpeed;
			}
			cubo->rotate(glm::vec4(1.0f, 0.3f, 0.5f, frameAngle));
		}

		// Casteamos la entidad genérica a entidad luz
		auto* sp = dynamic_cast<E_Light*>(spotLight->getEntity());

		// Actualizamos la posición y dirección de la luz con la de la cámara
		sp->position = cameras.at(0)->position;
		sp->direction = cameras.at(0)->front;

		personaje->lookAt(cameras.at(0)->position, glm::vec4(1.0f, 0.0f, 0.0f, -90.0f), 180.0f);

		// Renderizamos primero las luces, asi da igual el orden en los que las haya metido el usuario en el arbol
		for (auto& l : lights)
			l->draw(glm::mat4{ 1.0f }, sceneRoot->getPrincipalCamera());
		// Le decimos al shader cuantas luces hay de cada tipo
		basicShader->setInt("n_directionalLights", static_cast<int>(numLights.x));
		basicShader->setInt("n_pointLights", static_cast<int>(numLights.y));
		basicShader->setInt("n_spotLights", static_cast<int>(numLights.z));

		skyboxModel->draw(glm::mat4{ 1.0f }, sceneRoot->getPrincipalCamera());

		// Recorremos el arbol
		sceneRoot->traversal(glm::mat4{ 1.0f }, sceneRoot->getPrincipalCamera());

		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	// Destruimos la ventana y cerramos glfw
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}