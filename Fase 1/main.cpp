#include <Inazuma/Inazuma.hpp>

constexpr unsigned int WIDTH{1280};
constexpr unsigned int HEIGHT{720};
float lastX{}, lastY{};
bool firstMouse{true};
float deltaTime{1.0f};
TCamera* principal{};
bool moved{};
bool checkV{};
Vec3 pos1{}, pos2{};
_Color cleanColor_{100, 100, 100, 255};

void processInput(Inazuma& MG){
    if(MG.getPrincipalCamera() && isKeyPressed(K_W)){
		MG.getPrincipalCamera()->processKeyboard(FORWARD, deltaTime);
	}
	if(MG.getPrincipalCamera() && isKeyPressed(K_S)){
		MG.getPrincipalCamera()->processKeyboard(BACKWARD, deltaTime);
	}
	if(MG.getPrincipalCamera() && isKeyPressed(K_A)){
		MG.getPrincipalCamera()->processKeyboard(LEFT, deltaTime);
	}
	if(MG.getPrincipalCamera() && isKeyPressed(K_D)){
		MG.getPrincipalCamera()->processKeyboard(RIGHT, deltaTime);
	}
	if(!checkV && isKeyPressed(K_V)){
		moved = !moved;
		if(moved){
			pos1  = Vec3(-4.0f, 0.0f, 0.0f); 
			pos2  = Vec3(13.0f, 0.0f, -2.0f); 
			cleanColor_.r = 150;
		}else{
			pos1  = Vec3(4.0f, 0.0f, 0.0f); 
			pos2  = Vec3(-13.0f, 0.0f, 2.0f); 
			cleanColor_.r = 100;
		}
	}
	checkV = isKeyPressed(K_V);
	if(isKeyPressed(K_ESCAPE)){
		glfwSetWindowShouldClose(MG.getMainWindow(), true);
	}
}

void mouse_callback([[maybe_unused]] GLFWwindow* window, double xPosIn, double yPosIn){
	float xPos{static_cast<float>(xPosIn)};
	float yPos{static_cast<float>(yPosIn)};

	if(firstMouse){
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	// Calculamos la diferencia entre la posicion actual y la anterior
	float xOffset{xPos - lastX}, yOffset{lastY - yPos};
	// Actualizamos
	lastX = xPos;
	lastY = yPos;

    if(principal)
	    principal->processMouseMovement(xOffset, yOffset);
}

void scroll_callback([[maybe_unused]] GLFWwindow* window, [[maybe_unused]] double xOffset, double yOffset){
	if(principal)
        principal->processMouseScroll(static_cast<float>(yOffset));
}

int main(){
    Inazuma MG{WIDTH, HEIGHT, "Mapa de pruebas"};
	principal = MG.getPrincipalCamera();
	principal->position.y += 1.0f;
	principal->position.z += 8.0f;
    glfwSetCursorPosCallback(MG.getMainWindow(), mouse_callback);
	glfwSetScrollCallback(MG.getMainWindow(), scroll_callback);
    glfwSetInputMode(MG.getMainWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	MG.createPointLight(Vec3(0.7f, 1.0f, 15.0f), Vec3(0.2f, 0.2f, 0.2f), Vec3(0.5f, 0.5f, 0.5f), Vec3(1.0f, 1.0f, 1.0f), Vec3(1.0f, 0.09f, 0.032f));
	auto* spotLight = MG.createSpotLight(MG.getPrincipalCamera()->position, MG.getPrincipalCamera()->front, Vec3(0.0f, 0.0f, 0.0f), Vec3(1.0f, 1.0f, 1.0f), Vec3(1.0f, 1.0f, 1.0f), Vec3(1.0f, 0.09f, 0.032f), Vec2(glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(15.0f))));
	MG.createImage("./resources/barco.png", Vec3(3.0f, 0.0f, 1.0f), Vec3(1.0f, 1.0f, 1.0f));
	auto* lvl = MG.createModel("./resources/testMap.fbx");
	MG.createModel("./resources/backpack/backpack.obj", Vec3(-1.0f, 1.0f, 0.0f));
	MG.createPointLight(Vec3(-15.2f, 1.0f, 2.0f), Vec3(0.2f, 0.2f, 0.2f), Vec3(0.5f, 0.5f, 0.5f), Vec3(1.0f, 1.0f, 1.0f), Vec3(1.0f, 0.09f, 0.032f));
	MG.createPointLight(Vec3(-1.2f, 1.0f, -15.0f), Vec3(0.2f, 0.2f, 0.2f), Vec3(0.5f, 0.5f, 0.5f), Vec3(1.0f, 1.0f, 1.0f), Vec3(1.0f, 0.09f, 0.032f));
	auto* cube1 = MG.createCube(Vec3(8.0f, 0.0f, -10.0f), Vec3(5.0f, 2.0f, 3.0f), _RED);
	auto* cube2 = MG.createCube(Vec3(0.0f, 0.0f, -8.0f), Vec3(1.0f, 2.0f, 1.0f), _GREEN);
	
	lvl->translate(Vec3(0.0f, -1.0f, 0.0f));
	lvl->rotate(Vec4(1.0f, 0.0f, 0.0f, -90.0f));

    while(!glfwWindowShouldClose(MG.getMainWindow())){
		bool hasMoved{moved};
        processInput(MG);
		cleanBackground(cleanColor_);

		if(moved != hasMoved){
			cube1->translate(pos1);
			cube2->translate(pos2);
		}

		auto* sp = dynamic_cast<TLight*>(spotLight->getEntity());
		
        sp->position = MG.getPrincipalCamera()->position;
        sp->direction = MG.getPrincipalCamera()->front;

		MG.render();
	}
	
	glfwTerminate();
    return 0;
}