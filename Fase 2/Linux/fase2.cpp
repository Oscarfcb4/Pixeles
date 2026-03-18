#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>

// Definimos el ancho, alto y nombre de nuestra ventana
constexpr unsigned int ANCHO{1280};
constexpr unsigned int ALTO{720};
constexpr const char* NOMBRE = "Ventana";

const char *vertexShaderSource = 
	"#version 330 core\n"
	"layout (location = 0) in vec3 aPos;\n"
	"void main(){\n"
	"gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0f);\n"
	"}\0";

const char *fragmentShaderSource = 
	"#version 330 core\n"
	"out vec4 FragColor;\n"
	"void main(){\n"
	"FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
	"}\0";

typedef struct Color {
    float r, g, b, a;
    bool operator==(const Color& c) const {
        return (
            r == c.r && 
            g == c.g && 
            b == c.b && 
            a == c.a
        );
    }
} Color;

struct TEntity{
    void draw(glm::mat4 matrix = glm::mat4(1.0f));
};

// Definimos nuestros colores de ventana
glm::vec4 color1(200, 200, 200, 200);
glm::vec4 color2(155, 155, 155, 255);
glm::vec4 color3(55, 55, 55, 255);

// Redefinicion del metodo de OpenGL, permite hacer resize de la ventana manualmente
void framebuffer_size_callback([[maybe_unused]] GLFWwindow* window, int width, int height){
	glViewport(0, 0, width, height);
}

int main(){
    // Inicio GLFW
    glfwInit();

    // Le decimos a GLFW las veriones de OpenGL compatibles y el perfil que vamos a usar, el core
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Creamos la ventana con GLFW
	GLFWwindow* window = glfwCreateWindow(ANCHO, ALTO, NOMBRE, NULL, NULL);
    // Si no se crea la ventana, abortamos y lo decimos por terminal
	if(window == NULL){
		std::cout << "Ha fallado la creacion de la ventana" << std::endl;
		glfwTerminate();
		return 0;
	}

    // Le decimos a GLFW que use la ventana creada
	glfwMakeContextCurrent(window);

    // Inicializamos GLAD con el contexto de GLFW
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
		std::cout << "Ha fallado la inicializacion de GLAD" << std::endl;
		return 0;
	}

    // Iniciamos el viewport de OpenGL
    glViewport(0, 0, ANCHO, ALTO);

    // Le decimos a GLFW que use la funcion nuestra para cada vez que se intente cambiar el size de la ventana
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// El color actual de la ventana
	glm::vec4 colorActual(color1);

	// Creamos el vertexShader basico
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	// Le asignamos la source del codigo, en este caso lo hemos creado arriba.
	// El 1 es por el numero de Strings que le vamos a pasar, en este caso un 1
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	// Creamos tambien el fragmentShader
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	// Creamos el programa de shaders linkando los dos shaders
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	// Ahora activamos el programa
	glUseProgram(shaderProgram);
	// A partir de ahora, toda llamada a rendrizar usara nuestro programa
	// Una vez linkados, eliminamos la instancia
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// Inicializamos el bucle de renderizado
    while(!glfwWindowShouldClose(window)){
		// Si aprieta la tecla 1
		if(glfwGetKey(window, 49) == GLFW_PRESS){
			colorActual = color1;
		}
		// Si aprieta la tecla 2
		if(glfwGetKey(window, 50) == GLFW_PRESS){
			colorActual = color2;
		}
		// Si aprieta la tecla 3
		if(glfwGetKey(window, 51) == GLFW_PRESS){
			colorActual = color3;
		}

		// Limpiamos la pantalla con el color de limpieza
    	glClearColor(colorActual.x/255.0f, colorActual.y/255.0f, colorActual.z/255.0f, colorActual.w/255.0f);
    	// Limpiamos tambien el buffer de color y el buffer de profundidad
    	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
