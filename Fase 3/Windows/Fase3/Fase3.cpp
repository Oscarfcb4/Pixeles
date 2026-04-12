#include "Fase3.h"




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

	// Cargamos nuestros shaders basicos
	Shader ourShader("./Shaders/vertex_shader_basico.vs", "./Shaders/fragment_shader_basico.fs");

	// Inicializamos el bucle de renderizado
	while (!glfwWindowShouldClose(window)) {
		// Tenemos que limpiar tambien el Z-Buffer ahora cada iteracion
		glClear(GL_COLOR_BUFFER_BIT);
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