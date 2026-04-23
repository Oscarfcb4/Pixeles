#version 330 core

out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

// Objeto material, con todo lo necesario para calcular el color de los materiales
struct Material{
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
	vec3 color;
};

// Objeto luz global
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
// el color del material y cuanto mayor el del color. Si no tiene material, de base se pone en 0,99, si si tiene se pone a 0.
uniform float mix_value;

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
vec3 calculateDirectionalLight(DirectionalLight dLight, vec3 Normal, vec3 cameraPos){
	// Sacamos y normalizamos la direccion con la que el rayo de luz colisiona con el fragmento desde la colision hacia fuera
	vec3 lightDir = normalize(-dLight.direction);
	// Sacamos la luz difusa entre 0 y el angulo entre la normal entre y la direccion de la luz
	float diffuseIntensity = max(dot(Normal, lightDir), 0.0);
	// Sacamos la luz especular sacando la direccion del reflejo de la direccion y la normal
	vec3 reflectDir = reflect(-lightDir, Normal);
	// Ahora calculamos la intensidad de esta luz segun el angulo que hacen respecto a la camara y el brillo del material
	float specularIntensity = pow(max(dot(cameraPos, reflectDir), 0.0), material.shininess);

	// Con la difusa y la especular calculadas, y sabiendo que el valor de la ambiental es siempre igual que la difusa en este sistema,
	// aplicamos las intensidades a los valores de las luces y sus texturas, las cuales las mezclamos con el color del modelo.
	vec3 ambient = dLight.ambient * mix(vec3(texture(material.diffuse, TexCoord)), material.color, mix_value);
	vec3 diffuse = dLight.diffuse * diffuseIntensity * mix(vec3(texture(material.diffuse, TexCoord)), material.color, mix_value);
	vec3 specular = dLight.specular * specularIntensity * mix(vec3(texture(material.specular, TexCoord)), material.color, mix_value);
	// Devolvemos el resultado
	return (ambient + diffuse + specular);
}

vec3 calculatePointLight(PointLight pLight, vec3 Normal, vec3 FragPos, vec3 cameraPos){
	// Para la puntual necesitamos saber la direccion pero respecto al fragmento especifico
	vec3 lightDir = normalize(pLight.position - FragPos);
	// Difusa
	float diffuseIntensity = max(dot(Normal, lightDir), 0.0);
	// Especular
	vec3 reflectDir = reflect(-lightDir, Normal);
	float specularIntensity = pow(max(dot(cameraPos, reflectDir), 0.0), material.shininess);
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

	// Aplicamos los calculos de las luces y la atenuacion
	vec3 ambient = (pLight.ambient * mix(vec3(texture(material.diffuse, TexCoord)), material.color, mix_value)) * attenuation;
	vec3 diffuse = (pLight.diffuse * diffuseIntensity * mix(vec3(texture(material.diffuse, TexCoord)), material.color, mix_value)) * attenuation;
	vec3 specular = (pLight.specular * specularIntensity * mix(vec3(texture(material.specular, TexCoord)), material.color, mix_value)) * attenuation;
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
	// Calculamos los angulos de cierre para la luz dirigida, para saber si entran dentro o no del limite al que enfoca la luz.
	// Simplemente con el angulo que hacen la direccion de la luz respecto al fragmento y la direccion actual de la luz en si
	float limitAngle = dot(lightDir, normalize(-sLight.direction));
	// Para hacerlo mas natural, tenemos un segundo radio que interpolara las intensidades segun lo cerca o lejos que estes del limite,
	// dando asi un efecto de desvanecido a la luz. Primero calculamos el angulo limite de esta zona.
	float fadeOffAngle = sLight.cutOff - sLight.outerCutOff;
	// Hacemos la interpolacion de intensidades. Es un poco complejo matematicamente y no lo termino de entender bien, el caso es que devolvera
	// un numero mas cercano a 1.0 si esta en el limite interior y 0,0 si esta en el exterior, o entre medias.
	float intensity = clamp((limitAngle - sLight.outerCutOff) / fadeOffAngle, 0.0, 1.0);
	// Aplicamos todos los valores calculados
	vec3 ambient = sLight.ambient * mix(vec3(texture(material.diffuse, TexCoord)), material.color, mix_value) * attenuation * intensity;
	vec3 diffuse = sLight.diffuse * diffuseIntensity * mix(vec3(texture(material.diffuse, TexCoord)), material.color, mix_value) * attenuation *  intensity;
	vec3 specular = sLight.specular * specularIntensity * mix(vec3(texture(material.specular, TexCoord)), material.color, mix_value) * attenuation * intensity;
	return (ambient + diffuse + specular);
}

void main(){
	// Creamos un colro base para el color que ira aumentando segun las luces
	vec3 color = vec3(0.0f, 0.0f, 0.0f);
	// Importantisimo normalizar tanto la normal como la distancia entre el fragmento y la camara
	vec3 normal = normalize(Normal);
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