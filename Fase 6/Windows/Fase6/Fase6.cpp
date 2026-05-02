// Sólo hace falta incluir Pixeles
#include <Pixeles.hpp>

// Variables para deltaTime
float lastFrame{};
float deltaTime{};

// Instanciamos el motor
Pixeles MG{ 1280, 720, "Tu Primer Motor Gráfico" };

// Guardamos un puntero a nuestra cámara principal
E_Camera* principalCamera{};

// Procesamos los inputs pero con nuestro motor
void processInput() {
	// Es lo mismo pero escondemos GLFW
	if(MG.isKeyPressed(K_W)) {
		principalCamera->processKeyboard(FORWARD, deltaTime);
	}
	if(MG.isKeyPressed(K_S)) {
		principalCamera->processKeyboard(BACKWARD, deltaTime);
	}
	if(MG.isKeyPressed(K_A)) {
		principalCamera->processKeyboard(LEFT, deltaTime);
	}
	if(MG.isKeyPressed(K_D)) {
		principalCamera->processKeyboard(RIGHT, deltaTime);
	}
	// Escape para cerrar la ventana.
	if(MG.isKeyPressed(K_ESCAPE)) {
		// Con esto, saldrá del bucle de renderizado
		MG.stopRendering(true);
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
	if (principalCamera != NULL)
		// Con la diferencia entre posiciones, llamamos a nuestro método de movimiento del ratón
		principalCamera->processMouseMovement(xOffset, yOffset);
}

// Procesamos el input de la rueda del ratón, necesita X e Y por compatibilidad con dispositivos raros
void zoomRaton(GLFWwindow* window, double xOffset, double yOffset) {
	// Si tiene cámara principal, la usamos
	if (principalCamera != NULL)
		// A nosotros obviamente sólo nos interesa el movimiento vertical
		principalCamera->processMouseScroll(static_cast<float>(yOffset));
}

int main() {
	// Creamos la cámara principal
	principalCamera = MG.createCamera(Vec3(-0.3f, 0.0f, 5.0f), Vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);

	// Seteamos lo del ratón (No lo he escondido para ir rápido, pero podrías hacerlo si quieres)
	glfwSetCursorPosCallback(MG.getMainWindow(), movimientoRaton);
	glfwSetScrollCallback(MG.getMainWindow(), zoomRaton);
	glfwSetInputMode(MG.getMainWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Creamos las luces con nuestro motor, como ves ya no es 
	// necesario que le pasemos todos los parámetros
	MG.createDirectionalLight();
	MG.createPointLight(
		Vec3(-3.8f, 0.0f, -12.3f), Vec3(0.2f, 0.2f, 0.2f), Vec3(0.8f, 0.8f, 0.8f),
		Vec3(1.0f, 1.0f, 1.0f), Vec3(1.0f, 0.09f, 0.032f));
	auto* spotLight = MG.createSpotLight(
		principalCamera->getPosition(), principalCamera->getFront(),
		Vec3(0.0f, 0.0f, 0.0f), Vec3(1.0f, 1.0f, 1.0f), Vec3(1.0f, 1.0f, 1.0f),
		Vec3(1.0f, 0.09f, 0.032f), Vec2(12.5f, 15.0f));

	// Creamos los cubos
	std::vector<E_Model*> cubos{};
	E_Model* cubo1  = MG.createCube(Vec3(0.0f, 0.0f, 0.0f), Vec3(1.0f, 1.0f, 1.0f), Color(1.0f, 0.0f, 0.0f, 1.0f),  45.0f, Vec3(1.0f, 0.0f, 0.0f), true);
	E_Model* cubo2  = MG.createCube(Vec3(2.0f, 5.0f, -15.0f), Vec3(1.0f, 1.0f, 1.0f), Color(0.0f, 1.0f, 0.0f, 1.0f), 45.0f, Vec3(1.0f, 0.0f, 0.0f));
	E_Model* cubo3  = MG.createCube(Vec3(-1.5f, -2.2f, -2.5f), Vec3(1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 1.0f, 1.0f),  45.0f, Vec3(1.0f, 0.0f, 0.0f));
	E_Model* cubo4  = MG.createCube(Vec3(-3.8f, -2.0f, -12.3f), Vec3(1.0f, 1.0f, 1.0f), Color(1.0f, 1.0f, 0.0f, 1.0f),  45.0f, Vec3(1.0f, 0.0f, 0.0f));
	E_Model* cubo5  = MG.createCube(Vec3(2.4f, -0.4f, -3.5f), Vec3(1.0f, 1.0f, 1.0f), Color(0.0f, 1.0f, 1.0f, 1.0f),  45.0f, Vec3(1.0f, 0.0f, 0.0f));
	E_Model* cubo6  = MG.createCube(Vec3(-1.7f, 3.0f, -7.5f), Vec3(1.0f, 1.0f, 1.0f), Color(1.0f, 0.0f, 1.0f, 1.0f),  45.0f, Vec3(1.0f, 0.0f, 0.0f));
	E_Model* cubo7  = MG.createCube(Vec3(1.3f, -2.0f, -2.5f), Vec3(1.0f, 1.0f, 1.0f), Color(1.0f, 1.0f, 1.0f, 1.0f), 45.0f, Vec3(1.0f, 0.0f, 0.0f));
	E_Model* cubo8  = MG.createCube(Vec3(1.5f, 2.0f, -2.5f), Vec3(1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f), 45.0f, Vec3(1.0f, 0.0f, 0.0f));
	E_Model* cubo9  = MG.createCube(Vec3(1.5f, 0.2f, -1.5f), Vec3(1.0f, 1.0f, 1.0f), Color(0.5f, 0.5f, 0.5f, 1.0f), 45.0f, Vec3(1.0f, 0.0f, 0.0f));
	E_Model* cubo10 = MG.createCube(Vec3(-1.3f, 1.0f, -1.5f), Vec3(1.0f, 1.0f, 1.0f), Color(1.0f, 0.5f, 0.0f, 1.0f), 45.0f, Vec3(1.0f, 0.0f, 0.0f));
	
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
	cubo1->setTexture("./Resources/Caja.png");

	// Creamos el modelo del nivel y lo colocamos en el mundo
	E_Model* nivel = MG.createModel("./resources/Mapa.obj", Vec3(-1.0f, -3.5f, 15.0f),
		Vec3(1.0f, 1.0f, 1.0f), 0.0f, Vec3(0.0f, 1.0f, 0.0f));

	// Cargamos la textura para el nivel
	nivel->setTexture("./Resources/Mapa.png");

	// Cargamos el personaje
	E_Model* personaje = MG.createModel("./resources/Personaje.fbx", Vec3(-5.0f, -1.5f, 0.0f),
		Vec3(0.01f, 0.01f, 0.01f), -90.0f, Vec3(1.0f, 0.0f, 0.0f));

	// Cargamos el skybox
	std::vector<std::string> caras{};
	caras.push_back("./Resources/Skybox/right.jpg");
	caras.push_back("./Resources/Skybox/left.jpg");
	caras.push_back("./Resources/Skybox/top.jpg");
	caras.push_back("./Resources/Skybox/bottom.jpg");
	caras.push_back("./Resources/Skybox/front.jpg");
	caras.push_back("./Resources/Skybox/back.jpg");
	MG.createSkybox(caras);

	// Inicializamos el bucle de renderizado, usando nuestro motor
	while (MG.rendering()) {
		// Iniciamos el start de nuestro motor
		MG.start();
		// Controlamos el input del usuario
		processInput();
		// Limpiamos el fondo
		MG.cleanBackground(Color(100.0f, 100.0f, 100.0f, 255.0f));

		// Calculamos el deltaTime (Esto podría hacerlo tu motor si quieres)
		float currentFrame = MG.getTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Iteramos por cada cubo
		for (unsigned int i = 0; i < 10; i++) {
			// Lo mismo que la fase2, pero usando los nodos de nuestro árbol
			E_Model* cubo = cubos.at(i);
			float frameAngle{};
			float rotationSpeed{40.0f};
			if (i > 0) {
				frameAngle = deltaTime * rotationSpeed * static_cast<float>(i);
			}
			else {
				frameAngle = deltaTime * rotationSpeed;
			}
			cubo->rotate(Vec4(1.0f, 0.3f, 0.5f, frameAngle));
		}

		// Actualizamos la posición y dirección de la luz con la de la cámara
		spotLight->setPosition(principalCamera->getPosition());
		spotLight->setDirection(principalCamera->getFront());

		// Que nuestro personaje nos persiga con la mirada
		personaje->lookAt(principalCamera->getPosition(), Vec4(1.0f, 0.0f, 0.0f, -90.0f), 180.0f);
		
		// Finalizamos y renderizamos
		MG.end();
	}

	// Cerramos la ventana cuando termine
	MG.closeWindow();
	return 0;
}