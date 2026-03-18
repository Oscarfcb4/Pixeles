#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <shader.hpp>
#include <iostream>
#include <math.h>
// Librearia GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace glm;

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window){
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
		glfwSetWindowShouldClose(window, true);
	}
	if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
		glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
	}else{
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	}
}

int main(){
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	

	GLFWwindow* window = glfwCreateWindow(800, 600, "Learn OpenGL", NULL, NULL);
	if(window == NULL){
		std::cout << "Failed to create window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
		std::cout << "Failed to initializa GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, 800, 600); 
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

	Shader ourShader("./shaders/shader.vs", "./shaders/shader.fs");

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
	vec3 cubePositions[] = {
		vec3( 0.0f, 0.0f, 0.0f),
		vec3( 2.0f, 5.0f, -15.0f),
		vec3(-1.5f, -2.2f, -2.5f),
		vec3(-3.8f, -2.0f, -12.3f),
		vec3( 2.4f, -0.4f, -3.5f),
		vec3(-1.7f, 3.0f, -7.5f),
		vec3( 1.3f, -2.0f, -2.5f),
		vec3( 1.5f, 2.0f, -2.5f),
		vec3( 1.5f, 0.2f, -1.5f),
		vec3(-1.3f, 1.0f, -1.5f)
	};

	unsigned int indices[] = {
		0, 1, 3,
		2, 3, 1
	};

	unsigned int EBO, VBO, VAO;
	glGenBuffers(1, &EBO);
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	stbi_set_flip_vertically_on_load(true);

	unsigned int texture[2];
	glGenTextures(2, texture);

	glBindTexture(GL_TEXTURE_2D, texture[0]);
	int width, height, nrChannels;
	unsigned char *data = stbi_load("textures/barco.png", &width, &height, &nrChannels, 0);
	if(data){
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}else{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	glBindTexture(GL_TEXTURE_2D, texture[1]);
	data = stbi_load("textures/awesomeface.png", &width, &height, &nrChannels, 0);
	if(data){
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}else{
			std::cout << "Failed to load texture" << std::endl;
		}
	stbi_image_free(data);

	// Recuperamos las direcciones de los uniforms a los que vamos a añadir las matrices
	unsigned int transformLoc = glGetUniformLocation(ourShader.ID, "transform");
	unsigned int modelLoc = glGetUniformLocation(ourShader.ID, "model");
	unsigned int viewLoc = glGetUniformLocation(ourShader.ID, "view");
	unsigned int projectionLoc = glGetUniformLocation(ourShader.ID, "projection");
	
	// Creamos la matriz modelo
	mat4 model = mat4(1.0f);

	// Creamos la matriz vista
	mat4 view = mat4(1.0f);
	view = translate(view, vec3(0.0f, 0.0f, -3.0f));

	// Creamos la matriz de proyeccion
	mat4 projection{};
	projection = perspective(radians(45.0f), static_cast<float>(width/height), 0.1f, 100.0f);

	// Permitimos que OpenGL use el Z-Buffer
	glEnable(GL_DEPTH_TEST);

	// Wireframe mode
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	while(!glfwWindowShouldClose(window)){
		processInput(window);
		// Tenemos que limpiar tambien el Z-Buffer ahora cada iteracion
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ourShader.use();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture[1]);
		glBindVertexArray(VAO);

		for(unsigned int i=0; i<10; i++){
			// Inicializamos la matriz modelo, para que vuelva a 0 cada vez
			mat4 model{mat4(1.0f)};
			// Cogemos las posiciones de nuestro array de posiciones segun el bucle y lo
			// usamos para transladar la matriz modelo
			model = translate(model, cubePositions[i]);
			// Un angulo de rotacion diferente para cada cubo y segun el tiempo de ejecucion
			float angle{};
			if(i>0){
				angle = glfwGetTime() * 20.0f * i;
			}else{
				angle = glfwGetTime() * 20.0f;
			}
			// Lo rotamos segun ese angulo para que este girando
			model = rotate(model, radians(angle), vec3(1.0f, 0.3f, 0.5f));
			//glUniformMatrix4fv(transformLoc, 1, GL_FALSE, value_ptr(trans));
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
	glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
	glfwTerminate();
	return 0;
}

