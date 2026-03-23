#version 330 core
in vec2 TexCoord;

uniform vec4 ourColor;
out vec4 FragColor;

// Para obtener el acceso textura, que GLSL los trata con un tipo concreto
// llamado sampler, en este caso como es 2D es un sampler2D
uniform sampler2D texture1;

void main(){
	// Usamos el metodo de GLSL texture que recibe el sampler y las coordenadas
	FragColor = texture(texture1, TexCoord);
};
