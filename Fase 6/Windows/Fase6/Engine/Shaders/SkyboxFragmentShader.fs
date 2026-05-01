#version 330 core
out vec4 FragColor;
	
in vec3 texCoords;

// No es una simple textura, es una textura cúbica
uniform samplerCube skybox;
	
void main(){
	// El color es el de la textura, sin más complicación
	FragColor = texture(skybox, texCoords);
};