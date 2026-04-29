#version 330 core
layout (location = 0) in vec3 aPos;
	
out vec3 texCoords;
	
// Matriz de proyeccion combinada con la vista
uniform mat4 projectionView;
	
void main(){
	// Las coordenadas de textura del cubo mapa coinciden con la posicion del vertice
	texCoords = aPos;
	// Transforma el vertice a la vista de la cámara usando la matriz projectionView
	vec4 pos = projectionView * vec4(aPos, 1.0f);
	// Establece la profundidad a 1.0f (valor maximo) para que el skybox se renderice detras de todos los objetos
	// Al asignar w a z, tras la division de perspectiva la profundidad sera 1.0f
	gl_Position = pos.xyww;
};