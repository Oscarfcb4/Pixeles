#include <glad/glad.h>
#include <glfw3.h>
#include <glm.hpp>
#include <iostream>

// Definimos el ancho, alto y nombre de nuestra ventana
constexpr unsigned int ANCHO{ 1280 };
constexpr unsigned int ALTO{ 720 };
constexpr const char* NOMBRE = "Una ventana a OpenGL";

// Definimos nuestros colores de ventana fuera del main
glm::vec4 color1(0.78, 0.78, 0.78, 0.78);
glm::vec4 color2(0.6, 0.6, 0.6, 1);
glm::vec4 color3(0.22, 0.22, 0.22, 1);

int main() {
	// Inicio GLFW
	glfwInit();

	// Le decimos a GLFW las veriones de OpenGL compatibles y el perfil que vamos a usar, el core
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Creamos la ventana con GLFW
	GLFWwindow* window = glfwCreateWindow(ANCHO, ALTO, NOMBRE, NULL, NULL);

	// Si no se crea la ventana, abortamos y lo decimos por terminal
	if (window == NULL) {
		std::cout << "Ha fallado la creacion de la ventana" << std::endl;
		glfwTerminate();
		return 0;
	}

	// Le decimos a GLFW que use la ventana creada
	glfwMakeContextCurrent(window);

	// Inicializamos GLAD con el contexto de GLFW
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Ha fallado la inicializacion de GLAD" << std::endl;
		return 0;
	}

	// Iniciamos el viewport de OpenGL
	glViewport(0, 0, ANCHO, ALTO);

	// El color actual de la ventana
	glm::vec4 colorActual(color1);

	// Inicializamos el bucle de renderizado
	while (!glfwWindowShouldClose(window)) {
		// Si aprieta la tecla numérica 1
		if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
			colorActual = color1;
		}
		// Si aprieta la tecla numérica 2
		if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
			colorActual = color2;
		}
		// Si aprieta la tecla numérica 3
		if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
			colorActual = color3;
		}

		// Limpiamos la pantalla con el color actual
		glClearColor(colorActual.x, colorActual.y, colorActual.z, colorActual.w);
		// Limpiamos tambien el buffer de color
		glClear(GL_COLOR_BUFFER_BIT);

		// Procesamos todos los eventos pendientes antes de la siguiente iteracion
		glfwPollEvents();
		// Cambiamos los buffer
		glfwSwapBuffers(window);
	}

	// Destruimos la ventana y cerramos glfw
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
