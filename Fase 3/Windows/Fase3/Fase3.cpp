#include "Fase3.hpp"
#include "./SceneTree/SceneTree.hpp"

// La ventana principal
GLFWwindow* window{};
// La raíz del árbol
std::unique_ptr<Node> sceneRoot{};
// Instancia del gestor de recursos
ResourceManager RM{};

// La relación de aspecto, que será usada en varias partes
float aspect{ static_cast<float>(ANCHO) / static_cast<float>(ALTO) };

// Variables para deltaTime
float lastFrame{};
float deltaTime{};

// Vectores para almacenar los nodos, camaras y modelos
std::vector<std::unique_ptr<Node>> nodes{};
std::vector<std::unique_ptr<E_Camera>> cameras{};
std::vector<std::unique_ptr<E_Model>> models{};

// Los métodos para crear cubos y para crear cámaras
Node* createCube(glm::vec3 pos, glm::vec3 size, Color color, float rotAngle, glm::vec3 rotAxis) {
	auto nodeCube = std::make_unique<Node>();
	nodes.push_back(std::move(nodeCube));
	auto& nCube = nodes[nodes.size() - 1];
	// Como siempre va a ser igual le pasamos ya las cosas por el constructor
	auto entityModel = std::make_unique<E_Model>(&RM, "Cubo", RType::RCube);
	models.push_back(std::move(entityModel));
	auto& eCube = models[models.size() - 1];
	// El cubo siempre va a tener solo un shader, el base
	eCube->setShader("base");
	// Aplicamos transformaciones
	nCube->translate(glm::vec3(pos.x, pos.y, pos.z));
	nCube->rotate(glm::vec4(rotAxis.x, rotAxis.y, rotAxis.z, rotAngle));
	nCube->scale(glm::vec3(size.x / 2, size.y / 2, size.z / 2));
	// Le ponemos el color
	eCube->color = color;
	eCube->setAspect(aspect);
	nCube->setEntity(std::move(eCube.get()));
	sceneRoot->addChild(std::move(nCube.get()));
	return sceneRoot->getChildren()[sceneRoot->getChildren().size() - 1];
}

Node* createCamera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) {
	// Creamos un nodo como un puntero unico
	auto nodeCam = std::make_unique<Node>();
	// Lo pusheamos al vector de nodos y le pasamos la responsabilidad de memoria
	nodes.push_back(std::move(nodeCam));
	// Lo recuperamos como una referencia para trabajar con el
	auto& nCam = nodes[nodes.size() - 1];
	// Creamos la entidad 
	auto entityCam = std::make_unique<E_Camera>(position, up, yaw, pitch);
	// Lo pusheamos y le pasamos la responsabilidad de memoria
	cameras.push_back(std::move(entityCam));
	// La recuperamos como referencia
	auto& eCam = cameras[cameras.size() - 1];
	// Si no existe una camara, la asigna como camara principal
	if(sceneRoot->getPrincipalCamera() == NULL){
		sceneRoot->setPrincipalCamera(eCam.get());
	}
	// Le asigna al nodo la entidad
	nCam->setEntity(eCam.get());
	// Le anadimos a la raiz como hijo
	sceneRoot->addChild(nCam.get());
	// Devuelve un puntero al nodo
	return sceneRoot->getChildren()[sceneRoot->getChildren().size() - 1];
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

// La velocidad de la cámara, 
float speed{ 1.0f };

void processInput() {
	if(glfwGetKey(window, GLFW_KEY_W)) {
		cameras.at(0)->processKeyboard(FORWARD, speed);
	}
	if(glfwGetKey(window, GLFW_KEY_S)) {
		cameras.at(0)->processKeyboard(BACKWARD, speed);
	}
	if(glfwGetKey(window, GLFW_KEY_A)) {
		cameras.at(0)->processKeyboard(LEFT, speed);
	}
	if(glfwGetKey(window, GLFW_KEY_D)) {
		cameras.at(0)->processKeyboard(RIGHT, speed);
	}
	if(glfwGetKey(window, GLFW_KEY_ESCAPE)) {
		glfwSetWindowShouldClose(window, true);
	}
}

// Guardamos las posiciones anteriores del ratón
float lastX{}, lastY{};
// Si es la primera lectura del ratón
bool firstMouse{ true };

void mouse_callback([[maybe_unused]] GLFWwindow* window, double xPosIn, double yPosIn) {
	float xPos{ static_cast<float>(xPosIn) };
	float yPos{ static_cast<float>(yPosIn) };

	if (firstMouse) {
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	// Calculamos la diferencia entre la posicion actual y la anterior
	float xOffset{ xPos - lastX }, yOffset{ lastY - yPos };
	// Actualizamos
	lastX = xPos;
	lastY = yPos;

	if (cameras.at(0) != NULL)
		cameras.at(0)->processMouseMovement(xOffset, yOffset);
}

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

	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glViewport(0, 0, ANCHO, ALTO);
	glEnable(GL_DEPTH_TEST);
}

int main() {
	
	// Llamamos a nuestra función que configura OpenGL
	if (!configurarOpenGL())
		return -1;

	// Iniciamos al raiz del arbol de la escena
	sceneRoot = std::make_unique<Node>();
	// Le decimos a nuestro Resource Manager que esta es nuestra raíz
	RM.setScene(sceneRoot.get());
	// Cargamos nuestros shaders basicos
	Shader* basicShader = &RM.getShader("base", "./Shaders/vertex_shader_basico.vs", "./Shaders/fragment_shader_basico.fs")->shader;
	// Creamos la cámara principal
	Node* camara = createCamera(glm::vec3(-0.3f, 0.0f, 5.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);

	// Creamos los cubos, igual que en la Fase2
	std::vector<Node*> cubos{};
	Node* cubo1  = createCube(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), Color(1.0f, 0.0f, 0.0f, 1.0f),  45.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	Node* cubo2  = createCube(glm::vec3(2.0f, 5.0f, -15.0f), glm::vec3(1.0f, 1.0f, 1.0f), Color(0.0f, 1.0f, 0.0f, 1.0f), 45.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	Node* cubo3  = createCube(glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 1.0f, 1.0f),  45.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	Node* cubo4  = createCube(glm::vec3(-3.8f, -2.0f, -12.3f), glm::vec3(1.0f, 1.0f, 1.0f), Color(1.0f, 1.0f, 0.0f, 1.0f),  45.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	Node* cubo5  = createCube(glm::vec3(2.4f, -0.4f, -3.5f), glm::vec3(1.0f, 1.0f, 1.0f), Color(0.0f, 1.0f, 1.0f, 1.0f),  45.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	Node* cubo6  = createCube(glm::vec3(-1.7f, 3.0f, -7.5f), glm::vec3(1.0f, 1.0f, 1.0f), Color(1.0f, 0.0f, 1.0f, 1.0f),  45.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	Node* cubo7  = createCube(glm::vec3(1.3f, -2.0f, -2.5f), glm::vec3(1.0f, 1.0f, 1.0f), Color(1.0f, 1.0f, 1.0f, 1.0f), 45.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	Node* cubo8  = createCube(glm::vec3(1.5f, 2.0f, -2.5f), glm::vec3(1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f), 45.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	Node* cubo9  = createCube(glm::vec3(1.5f, 0.2f, -1.5f), glm::vec3(1.0f, 1.0f, 1.0f), Color(0.5f, 0.5f, 0.5f, 1.0f), 45.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	Node* cubo10 = createCube(glm::vec3(-1.3f, 1.0f, -1.5f), glm::vec3(1.0f, 1.0f, 1.0f), Color(1.0f, 0.5f, 0.0f, 1.0f), 45.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	
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
			float rotationSpeed{2000.0f};
			if (i > 0) {
				frameAngle = deltaTime * rotationSpeed * static_cast<float>(i);
			}
			else {
				frameAngle = deltaTime * rotationSpeed;
			}
			cubo->rotate(glm::vec4(1.0f, 0.3f, 0.5f, glm::radians(frameAngle)));
		}

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