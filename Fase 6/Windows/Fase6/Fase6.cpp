#include <Pixeles/Pixeles.hpp>
#include <Pixeles/E_AllEntities.hpp>
#include <Pixeles/SceneTree.hpp>

// Variables para deltaTime
float lastFrame{};
float deltaTime{};

Pixeles MG{ 1280, 720, "Tu Primer Motor Gráfico"};

E_Camera* principalCamera{};

// Procesamos los inputs del teclado de los usuarios con GLFW
void processInput() {
	// El movimiento de la cámara con WASD, usando los enumerados de GLFW y glfwGetKey
	if(MG.isKeyPressed(K_W)) {
		// Recuperamos la cámara de nuestro vector y usamos nuestro método de movimiento
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

int main() {
	// Creamos la cámara principal
	principalCamera = MG.createCamera(Vec3(-0.3f, 0.0f, 5.0f), Vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);

	// Creamos las luces
	MG.createDirectionalLight();
	MG.createPointLight(
		Vec3(-3.8f, 0.0f, -12.3f), Vec3(0.2f, 0.2f, 0.2f), Vec3(0.8f, 0.8f, 0.8f),
		Vec3(1.0f, 1.0f, 1.0f), Vec3(1.0f, 0.09f, 0.032f));
	auto* spotLight = MG.createSpotLight(
		principalCamera->getPosition(), principalCamera->getFront(),
		Vec3(0.0f, 0.0f, 0.0f), Vec3(1.0f, 1.0f, 1.0f), Vec3(1.0f, 1.0f, 1.0f),
		Vec3(1.0f, 0.09f, 0.032f), Vec2(12.5f, 15.0f));

	// Creamos los cubos, igual que en la Fase2
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

	E_Model* personaje = MG.createModel("./resources/Personaje.fbx", Vec3(-5.0f, -1.5f, 0.0f),
		Vec3(0.01f, 0.01f, 0.01f), -90.0f, Vec3(1.0f, 0.0f, 0.0f));

	std::vector<std::string> caras{};
	caras.push_back("./Resources/Skybox/right.jpg");
	caras.push_back("./Resources/Skybox/left.jpg");
	caras.push_back("./Resources/Skybox/top.jpg");
	caras.push_back("./Resources/Skybox/bottom.jpg");
	caras.push_back("./Resources/Skybox/front.jpg");
	caras.push_back("./Resources/Skybox/back.jpg");
	MG.createSkybox(caras);

	// Inicializamos el bucle de renderizado
	while (MG.rendering()) {
		MG.start();
		// Controlamos el input del usuario
		processInput();
		// Limpiamos el fondo
		MG.cleanBackground(Color(100.0f, 100.0f, 100.0f, 255.0f));

		// Calculamos el deltaTime
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

		MG.end();
	}

	MG.closeWindow();
	return 0;
}