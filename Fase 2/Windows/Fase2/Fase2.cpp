#include <glad/glad.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <iostream>
#include <shader.hpp>

// La libreria std para el procesamiento de imagenes
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

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
	-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // Cara trasera T1 - Abajo izquierda  
	0.5f, -0.5f, -0.5f, 1.0f, 0.0f, // Cara trasera T1 - Abajo derecha
	0.5f,  0.5f, -0.5f, 1.0f, 1.0f, // Cara trasera T1 - Arriba derecha
	0.5f,  0.5f, -0.5f, 1.0f, 1.0f, // Cara trasera T2 - Arriba derecha 
	-0.5f,  0.5f, -0.5f, 0.0f, 1.0f, // Cara trasera T2 - Arriba izquierda
	-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // Cara trasera T2 - Abajo izquierda

	-0.5f, -0.5f,  0.5f, 0.0f, 0.0f, // Lo mismo con todas pero
	0.5f, -0.5f,  0.5f, 1.0f, 0.0f, // siendo la cara delantera
	0.5f,  0.5f,  0.5f, 1.0f, 1.0f,
	0.5f,  0.5f,  0.5f, 1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f, 0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f, 0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f, 1.0f, 0.0f, // Cara lateral izquierda T1 - Arriba al frente
	-0.5f,  0.5f, -0.5f, 1.0f, 1.0f, // Cara lateral izquierda T1 - Arriba al fondo
	-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // Cara lateral izquierda T1 - Abajo al fondo
	-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // Cara lateral izquierda T2 - Abajo al fondo
	-0.5f, -0.5f,  0.5f, 0.0f, 0.0f, // Cara lateral izquierda T2 - Abajo al frente
	-0.5f,  0.5f,  0.5f, 1.0f, 0.0f, // Cara lateral izquierda T2 - Arriba al frente

	0.5f,  0.5f,  0.5f, 1.0f, 0.0f, // Lo mismo pero con la cara
	0.5f,  0.5f, -0.5f, 1.0f, 1.0f, // lateral derecha
	0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
	0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
	0.5f, -0.5f,  0.5f, 0.0f, 0.0f,
	0.5f,  0.5f,  0.5f, 1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // Cara base inferior T1 - Frente izquierda
	0.5f, -0.5f, -0.5f, 1.0f, 1.0f, // Cara base inferior T1 - Frente derecha
	0.5f, -0.5f,  0.5f, 1.0f, 0.0f, // Cara base inferior T1 - Fondo derecha
	0.5f, -0.5f,  0.5f, 1.0f, 0.0f, // Cara base inferior T2 - Fondo derecha
	-0.5f, -0.5f,  0.5f, 0.0f, 0.0f, // Cara base inferior T2 - Fondo izquierda
	-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // Cara base inferior T2 - Frente izquierda

	-0.5f,  0.5f, -0.5f, 0.0f, 1.0f, // Lo mismo pero con la cara
	0.5f,  0.5f, -0.5f, 1.0f, 1.0f, // base superior
	0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
	0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f, 0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f, 0.0f, 1.0f
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

	// Damos un color base al fondo para que no salga negro puro
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

	// Cargamos nuestros shaders basicos
	Shader ourShader("./Shaders/vertex_shader_basico.vs", "./Shaders/fragment_shader_basico.fs");

	// El orden de las caras en el cubo
	unsigned int indices[] = {
		0, 1, 3,
		2, 3, 1
	};

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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Para evitar que por defecto cargue las imágenes al revés 
	stbi_set_flip_vertically_on_load(true);

	// Creamos un array de texturas, de momento de tamaño uno
	unsigned int texture[1];

	// Generamos la textura y la enlazamos
	glGenTextures(1, texture);
	glBindTexture(GL_TEXTURE_2D, texture[0]);

	// Cargamos con stbi la imagen guardando su ancho, alto y canales
	int width, height, nrChannels;
	unsigned char* data = stbi_load("Texturas/barco.png", &width, &height, &nrChannels, 0);

	// Si la carga correctamente, generamos una textura 2D y un mipmap con la información recibida de stbi
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Ha fallado la carga de texturas" << std::endl;
	}
	// Libreramos la memoria de stbi, importante
	stbi_image_free(data);

	// Recuperamos las direcciones de los uniforms en los shaders a los que vamos a añadir las matrices
	unsigned int transformLoc = glGetUniformLocation(ourShader.ID, "transform");
	unsigned int modelLoc = glGetUniformLocation(ourShader.ID, "model");
	unsigned int viewLoc = glGetUniformLocation(ourShader.ID, "view");
	unsigned int projectionLoc = glGetUniformLocation(ourShader.ID, "projection");

	// Creamos la matriz modelo
	glm::mat4 model = glm::mat4(1.0f);

	// Creamos la matriz vista
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

	// Creamos la matriz de proyeccion
	glm::mat4 projection{};
	projection = glm::perspective(glm::radians(45.0f), static_cast<float>(width / height), 0.1f, 100.0f);

	// Permitimos que OpenGL use el Z-Buffer
	glEnable(GL_DEPTH_TEST);

	// Inicializamos el bucle de renderizado
	while (!glfwWindowShouldClose(window)) {
		// Tenemos que limpiar tambien el Z-Buffer ahora cada iteracion
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Le indicamos a OpenGL que vamos a usar nuestro shader
		ourShader.use();

		// Activamos y enlazamos nuestra textura y nuestro Array Buffer
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture[0]);
		glBindVertexArray(VAO);

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
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model));
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(view));
			glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, value_ptr(projection));
			ourShader.set4Float("ourColor", 0.0f, 1.0f, 0.0f, 1.0f);
			ourShader.setInt("texture2", 1);

			// Dibujamos cada cubo
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	// Destruimos la ventana y cerramos glfw
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}