#version 330 core
layout (location = 0) in vec3 aPos;
	
out vec3 texCoords;
	
uniform mat4 projectionView;
	
void main(){
	texCoords = aPos;
	vec4 pos = projectionView * vec4(aPos, 1.0f);
	gl_Position = pos.xyww;
};