#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 modelViewProjection;
uniform mat4 model;

void main(){
	gl_Position = modelViewProjection * vec4(aPos, 1.0f);
	// Le pasamos la posicion del fragment respecto al mundo
	FragPos = vec3(model * vec4(aPos, 1.0f));
	// Pasamos las coordenadas de texturas
	TexCoord = aTexCoord;
	// Calculamos las normales, es costoso pero no queda otra porque tenemos que tener
	// en cuenta las normales de cada vertex, asi que no se puede hacer en CPU
	Normal = mat3(transpose(inverse(model))) * aNormal;
};
