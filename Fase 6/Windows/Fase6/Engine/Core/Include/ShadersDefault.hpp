#pragma once

const char* defaultVertexShader = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

// Mantenemos el uniform optimizado
uniform mat4 modelViewProjection;
// Pasamos otra vez la modelo pero sola
uniform mat4 model;

void main(){
	// Aquí nos seguimos librando de multiplicar matrices
	gl_Position = modelViewProjection * vec4(aPos, 1.0f);
	// Le pasamos la posicion del fragment respecto al mundo
	FragPos = vec3(model * vec4(aPos, 1.0f));
	// Pasamos las coordenadas de texturas
	TexCoord = aTexCoord;
	// Calculamos las normales por cada vértice
	Normal = mat3(transpose(inverse(model))) * aNormal;
};
)";

const char* defaultFragmentShader = R"(
#version 330 core

out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

// Objeto material, con todo lo necesario para calcular el color de los materiales
struct Material{
	// Textura del color
	sampler2D diffuse;
	// Textura de los brillos
	sampler2D specular;
	float shininess;
	vec3 color;
};

// Objeto luz direccional
struct DirectionalLight{
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
 
// Objeto luz puntual
struct PointLight{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float constant;
	float linear;
	float cuadratic;
};

// Objeto luz focal
struct SpotLight{
	vec3 position;
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float constant;
	float linear;
	float cuadratic;
	float cutOff;
	float outerCutOff;
};

// Valor de mezcla entre el color base que le pasa el usuario y el del material, cuanto menor sea el valor mayor sera
// el color del material y cuanto mayor el del color. Si no tiene material, de base se pone en 1.0, si si tiene se pone a 0.
uniform float mixValue;

uniform vec3 cameraPos;
uniform Material material;
// Valores dinamicos para saber la cantidad de luces que hay y de que tipo, por defecto estan en 0.
uniform int n_directionalLights = 0;
uniform int n_pointLights = 0;
uniform int n_spotLights = 0;

// Definimos unos arrays de los distintos tipos de luces, como maximo admito 10 luces. A futuro se podria ver como mejorar esto
// para hacerlo mas dinamico.
#define n_Max_directionalLights 10
uniform DirectionalLight dLight[n_Max_directionalLights];

#define n_Max_pointLights 10
uniform PointLight pLight[n_Max_pointLights];

#define n_Max_spotLights 10
uniform SpotLight sLight[n_Max_spotLights];

// Calculamos el efecto de las luces direccionales en el color del fragmento
vec3 calculateDirectionalLight(DirectionalLight dLight, vec3 Normal, vec3 viewDir){
	// Sacamos y normalizamos la direccion con la que el rayo de luz colisiona con el fragmento, al revés
	vec3 lightDir = normalize(-dLight.direction);
	// Sacamos la intensidad entre 0 y el angulo entre la normal y la direccion de la luz
	float diffuseIntensity = max(dot(Normal, lightDir), 0.0);
	// Sacamos la dirección del reflejo con la inversa de la luz de la dirección y la normal
	vec3 reflectDir = reflect(-lightDir, Normal);
	// Ahora calculamos la intensidad de esta luz segun el angulo que hacen respecto a la camara y 
	// el brillo del material
	float specularIntensity = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	// Con la difusa y la especular calculadas, y sabiendo que el valor de la ambiental es siempre igual que la difusa en este sistema,
	// aplicamos las intensidades a los valores de las luces y sus texturas, las cuales las mezclamos con el color del modelo.
	vec3 ambient = dLight.ambient * mix(vec3(texture(material.diffuse, TexCoord)), material.color, mixValue);
	vec3 diffuse = dLight.diffuse * diffuseIntensity * mix(vec3(texture(material.diffuse, TexCoord)), material.color, mixValue);
	vec3 specular = dLight.specular * specularIntensity * mix(vec3(texture(material.specular, TexCoord)), material.color, mixValue);
	// Devolvemos el resultado
	return (ambient + diffuse + specular);
}

vec3 calculatePointLight(PointLight pLight, vec3 Normal, vec3 FragPos, vec3 viewDir){
	// Para la puntual necesitamos saber la direccion pero respecto al fragmento especifico
	vec3 lightDir = normalize(pLight.position - FragPos);
	// Difusa
	float diffuseIntensity = max(dot(Normal, lightDir), 0.0);
	// Especular
	vec3 reflectDir = reflect(-lightDir, Normal);
	float specularIntensity = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	// Ahora calculamos la atenuacion, primero sacando la distancia entre la posicion y el fragmento
	float distance = length(pLight.position - FragPos);
	// Ahora aplicamos la formula de la atenuacion
	//
	// 							    1.0
	// ---------------------------------------------------------------
	//   constante + (lineal * distancia) + (cuadratico*distancia^2)
	//
	// Siendo constante, lineal y cuadratico valores dependientes del usuario.
	float attenuation = 1.0 / (pLight.constant + (pLight.linear * distance) + (pLight.cuadratic * (distance * distance)));

	// Aplicamos los calculos de las luces y le añadimos la atenuacion
	vec3 ambient = (pLight.ambient * mix(vec3(texture(material.diffuse, TexCoord)), material.color, mixValue)) * attenuation;
	vec3 diffuse = (pLight.diffuse * diffuseIntensity * mix(vec3(texture(material.diffuse, TexCoord)), material.color, mixValue)) * attenuation;
	vec3 specular = (pLight.specular * specularIntensity * mix(vec3(texture(material.specular, TexCoord)), material.color, mixValue)) * attenuation;
	return (ambient + diffuse + specular);
}

vec3 calculateSpotLight(SpotLight sLight, vec3 normal, vec3 FragPos, vec3 viewDir){
	vec3 lightDir = normalize(sLight.position - FragPos);
	// Difusa
	float diffuseIntensity = max(dot(normal, lightDir), 0.0);
	// Especular
	vec3 reflectDir = reflect(-lightDir, normal);
	float specularIntensity = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	// Atenuacion
	float distance = length(sLight.position - FragPos);
	float attenuation = 1.0 / (sLight.constant + (sLight.linear * distance) + (sLight.cuadratic * (distance * distance)));
	// Calculamos los angulos de cierre para la luz dirigida, para saber si entran dentro o no de un cono y de cuál.
	// Usamos la dirección de la luz (al revés) y el vector director del fragmento a la luz
	float limitAngle = dot(lightDir, normalize(-sLight.direction));
	// Primero calculamos el angulo limite que separa CutOff de OuterCutOff
	float fadeOffAngle = sLight.cutOff - sLight.outerCutOff;
	// Hacemos la interpolacion de intensidades. Devolvera un numero mas cercano a 1.0 si está más cerca
	// del CutOff y más cercano a 0,0 si lo está del OuterCutOff. Clampeamos el valor para que si está
	// fuera de ambos conos sea un 0,0 y si está completamente dentro del CutOff sea 1.0.
	float intensity = clamp((limitAngle - sLight.outerCutOff) / fadeOffAngle, 0.0, 1.0);
	// Aplicamos todos los valores calculados, añadiendo la intensidad interpolada
	vec3 ambient = (sLight.ambient * mix(vec3(texture(material.diffuse, TexCoord)), material.color, mixValue)) * attenuation * intensity;
	vec3 diffuse = (sLight.diffuse * diffuseIntensity * mix(vec3(texture(material.diffuse, TexCoord)), material.color, mixValue)) * attenuation *  intensity;
	vec3 specular = (sLight.specular * specularIntensity * mix(vec3(texture(material.specular, TexCoord)), material.color, mixValue)) * attenuation * intensity;
	return (ambient + diffuse + specular);
}

void main(){
	// Creamos un color base para el color que ira aumentando según las luces
	vec3 color = vec3(0.0f, 0.0f, 0.0f);
	// Importantísimo normalizarla normal interpolada
	vec3 normal = normalize(Normal);
	// Sacamos el vector director que apunta desde el fragmento a la cámara
	vec3 viewDir = normalize(cameraPos - FragPos);

	// Segun el numero de luces que haya anadido el usuario, se calculara tantas veces se necesite cada tipo ce luz
	for(int i = 0; i < n_directionalLights; i++)
		color += calculateDirectionalLight(dLight[i], normal, viewDir);

	for(int i = 0; i < n_pointLights; i++)
		color += calculatePointLight(pLight[i], normal, FragPos, viewDir);

	for(int i = 0; i < n_spotLights; i++)
		color += calculateSpotLight(sLight[i], normal, FragPos, viewDir);

	// Una vez hemos sacado las luces, se pasa el resultado total
	FragColor = vec4(color, 1.0);
};
)";

const char* skyboxVertexShader = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
	
out vec3 texCoords;
	
// Matriz de proyeccion combinada con la vista
uniform mat4 projectionView;
	
void main(){
	// Las coordenadas de textura del cubo mapa coinciden con la posicion del vertice
	texCoords = aPos;
	// Transforma el vertice a la vista de la c�mara usando la matriz projectionView
	vec4 pos = projectionView * vec4(aPos, 1.0f);
	// Establece la profundidad a 1.0f (valor maximo) para que el skybox se renderice detras de todos los objetos
	// Al asignar w a z, tras la division de perspectiva la profundidad sera 1.0f
	gl_Position = pos.xyww;
};
)";

const char* skyboxFragmentShader = R"(
#version 330 core
out vec4 FragColor;
	
in vec3 texCoords;

// No es una simple textura, es una textura cúbica
uniform samplerCube skybox;
	
void main(){
	// El color es el de la textura, sin más complicación
	FragColor = texture(skybox, texCoords);
};
)";