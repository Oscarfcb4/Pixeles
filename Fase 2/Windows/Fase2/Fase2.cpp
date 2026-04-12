#include <glad/glad.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <iostream>
#include <shader.hpp>

constexpr unsigned int ANCHO{ 1280 };
constexpr unsigned int ALTO{ 720 };
constexpr const char* NOMBRE = "Una escena cubista";

// Definimos nuestro propio tipo color
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

// Guardamos las posiciones de los vertices para crear una caja
float vertices[] = {
	// Cara delantera (z = 0.5)
	-0.5f,  0.5f,  0.5f,  // Vértice 3 (Esquina superior izquierda)
	 0.5f,  0.5f,  0.5f,  // Vértice 2 (Esquina superior derecha)
	-0.5f, -0.5f,  0.5f,  // Vértice 0 (Esquina inferior izquierda)
	 0.5f, -0.5f,  0.5f,  // Vértice 1 (Esquina inferior derecha)

	// Cara trasera (Eje Z = -0.5)
	-0.5f,  0.5f, -0.5f,  // Vértice 7 
	 0.5f,  0.5f, -0.5f,  // Vértice 6 
	-0.5f, -0.5f, -0.5f,  // Vértice 4 
	 0.5f, -0.5f, -0.5f   // Vértice 5 
};

// El orden de las caras en el cubo
unsigned int indices[] = {
	// Cara delantera
	3, 2, 0,
	3, 0, 1,

	// Cara trasera
	7, 6, 4,
	7, 4, 5,

	// Cara izquierda
	7, 5, 1,
	7, 1, 3,

	// Cara derecha
	6, 4, 0,
	6, 0, 2,

	// Cara inferior
	7, 7, 2,
	7, 2, 6,

	// Cara superior
	5, 4, 0,
	5, 0, 1
};

// Guardamos las posiciones de todas las cajas que vamos a iterar
glm::vec3 cubePositions[] = {
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(2.0f, 5.0f, -15.0f),
	glm::vec3(-1.5f, -2.2f, -2.5f),
	glm::vec3(-3.8f, -2.0f, -12.3f),
	glm::vec3(2.4f, -0.4f, -3.5f),
	glm::vec3(-1.7f, 3.0f, -7.5f),
	glm::vec3(1.3f, -2.0f, -2.5f),
	glm::vec3(1.5f, 2.0f, -2.5f),
	glm::vec3(1.5f, 0.2f, -1.5f),
	glm::vec3(-1.3f, 1.0f, -1.5f)
};

// Guardamos los colores de cada cubo
Color cubeColors[] = {
	Color(1.0f, 0.0f, 0.0f, 1.0f), // Rojo
	Color(0.0f, 1.0f, 0.0f, 1.0f), // Verde
	Color(0.0f, 0.0f, 1.0f, 1.0f), // Azul
	Color(1.0f, 1.0f, 0.0f, 1.0f), // Amarillo
	Color(0.0f, 1.0f, 1.0f, 1.0f), // Cian
	Color(1.0f, 0.0f, 1.0f, 1.0f), // Magenta
	Color(1.0f, 1.0f, 1.0f, 1.0f), // Blanco
	Color(0.0f, 0.0f, 0.0f, 1.0f), // Negro
	Color(0.5f, 0.5f, 0.5f, 1.0f), // Gris
	Color(1.0f, 0.5f, 0.0f, 1.0f)  // Naranja
};

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

	// Activamos el testeo de profundidad antes del bucle de renderizado
	glEnable(GL_DEPTH_TEST);

	// Cargamos nuestros shaders basicos
	Shader ourShader("./Shaders/vertex_shader_basico.vs", "./Shaders/fragment_shader_basico.fs");

	// Inicializamos el bucle de renderizado
	while (!glfwWindowShouldClose(window)) {
		// Ahora tenemos que limpiar tambien el Z-Buffer ahora cada iteracion
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Le indicamos a OpenGL que vamos a usar nuestro shader
		ourShader.use();

		// Iteramos una vez por cada caja
		for (unsigned int i = 0; i < 10; i++) {
			// Inicializamos la matriz modelo, para que vuelva a 0 cada vez
			glm::mat4 model{ glm::mat4(1.0f) };
			// Cogemos las posiciones de nuestro array de posiciones segun el bucle y lo
			// usamos para transladar la matriz modelo
			model = translate(model, cubePositions[i]);
			// Un angulo de rotacion diferente para cada cubo y segun el tiempo de ejecucion
			float angle{};
			if (i > 0) {
				angle = glfwGetTime() * 20.0f * i;
			}
			else {
				angle = glfwGetTime() * 20.0f;
			}
			// Lo rotamos segun ese angulo para que este girando
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			// Metemos las matrices en los uniforms
			ourShader.setMatrix4fv("model", value_ptr(model));
			ourShader.setMatrix4fv("view", value_ptr(view));
			ourShader.setMatrix4fv("projection", value_ptr(projection));
			// Metemos un color distinto cada vez, siguiendo el bucle
			ourShader.set4Float("ourColor", cubeColors[i].r, cubeColors[i].g, cubeColors[i].b, cubeColors[i].a);

			// Dibujamos cada cubo
			glDrawElements(
				GL_TRIANGLES,    // El tipo de primitiva
				36,              // El número total de vértices a dibujar
				GL_UNSIGNED_INT, // El tipo de los indices
				0                // Si hay un offset a la hora de leer los indices
			);
		}

		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	// Destruimos la ventana y cerramos glfw
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}