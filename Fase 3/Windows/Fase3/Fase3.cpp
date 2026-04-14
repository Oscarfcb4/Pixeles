#include "Fase3.h"
#include "./SceneTree/sceneTree.hpp"


std::unique_ptr<TNode> sceneRoot{};
TNode* getSceneRoot() { return sceneRoot.get(); };
GLFWwindow* mainWindow{};
// Instancia del gestor de recursos
TResourceManager RM{};
// Vectores para almacenar los nodos, necesario para gestionar y encapsular las responsabilidades de la destruccion de la memoria
std::vector<std::unique_ptr<TNode>> nodes{};

int main() {
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(ANCHO, ALTO, NOMBRE, NULL, NULL);
	if (window == NULL) {
		std::cout << "Ha fallado la creacion de la ventana" << std::endl;
		glfwTerminate();
		return 0;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Ha fallado la inicializacion de GLAD" << std::endl;
		return 0;
	}


	sceneRoot = std::make_unique<TNode>();
	RM.setScene(sceneRoot.get());

	glViewport(0, 0, ANCHO, ALTO);

	// Iniciamos nuestros Buffers y el Array Buffer
	unsigned int EBO, VBO, VAO;
	glGenBuffers(1, &EBO);
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);

	// Enlazamos el Array para indicarle a OpenGL que estamos trabajando ahora en él
	glBindVertexArray(VAO);

	// Enlazamos el Vertex Buffer sobre el Array Buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Le indicamos el tamaño del buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// 3 Floats para las posiciones de cada vértice
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Creamos la matriz modelo, no necesita alteraciones
	glm::mat4 model = glm::mat4(1.0f);

	// Creamos la matriz vista
	glm::mat4 view = glm::mat4(1.0f);
	// Colocamos la "Cámara" con la que vamos a ver en el mundo
	// Usamos la función de GLM para hacer la transformación de traslación
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

	// Creamos la matriz de proyeccion
	glm::mat4 projection{};
	// Usamos la función de GLM para la proyección perspectiva
	projection = glm::perspective(
		glm::radians(45.0f),              // Ángulo
		static_cast<float>(ANCHO / ALTO), // Relación de aspecto
		0.1f,                             // Plano Near
		100.0f                            // Plano Far
	);

	// Damos un color base al fondo para que no salga negro puro
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

	// Cargamos nuestros shaders basicos
	Shader ourShader("./Shaders/vertex_shader_basico.vs", "./Shaders/fragment_shader_basico.fs");

	// Inicializamos el bucle de renderizado
	while (!glfwWindowShouldClose(window)) {
		
		// Por si acaso
		glEnable(GL_DEPTH_TEST);
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