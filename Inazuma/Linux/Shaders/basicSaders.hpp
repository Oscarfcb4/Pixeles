#pragma once

// Guardamos los shaders base para los vertex, fragment y light shaders basicos.
static constexpr const char* basicVertexShader = 
	R"(#version 430 core
	layout (location = 0) in vec3 aPos;
	layout (location = 1) in vec3 aNormal;
	layout (location = 2) in vec2 aTexCoord;
	layout (location = 3) in ivec4 aBones;
	layout (location = 4) in vec4 aWeights;

	out vec2 TexCoord;
	out vec3 Normal;
	out vec3 FragPos;
	uniform mat4 modelViewProjection;
	uniform mat4 model;
	uniform mat4 view;
	uniform bool image;
	uniform bool anim;
	uniform vec2 imageScale = vec2(1.0f);
	uniform vec2 imagePos = vec2(0.0f);

	const int MAX_BONES = 100;
	const int MAX_BONE_INFLUENCE = 4;
	uniform mat4 finalBonesMatrices[MAX_BONES];

	void main(){
		if(aBones != vec4(-1.0f)){
			vec4 totalPos = vec4(0.0f);
			for(int i = 0; i < MAX_BONE_INFLUENCE; i++){
				if(aBones[i] == -1)
					continue;
				if(aBones[i] >= MAX_BONES){
					totalPos = vec4(aPos, 1.0f);
					break;
				}
				vec4 localPos = finalBonesMatrices[aBones[i]] * vec4(aPos, 1.0f);
				totalPos += localPos * aWeights[i];
			}
			gl_Position = modelViewProjection * totalPos;
		}else{
			gl_Position = modelViewProjection * vec4(aPos, 1.0f);
		}
		// Calculamos las normales, es costoso pero no queda otra porque tenemos que tener
		// en cuenta las normales de cada vertex, asi que no se puede hacer en CPU
		Normal = mat3(transpose(inverse(model))) * aNormal;
		// Le pasamos la posicion del fragment respecto al mundo, luego se interpolara
		FragPos = vec3(model * vec4(aPos, 1.0f));
		// Pasamos las coordenadas de texturas
		TexCoord = aTexCoord;
	};)";

static constexpr const char* basicFragmentShader = 
	R"(#version 430 core
	
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
		float alpha;
	};
	
	// Objeto luz global
	struct DirectionalLight{
		vec3 direction;
		vec3 ambient;
		vec3 diffuse;
		vec3 specular;
	 	vec3 lightPos;
	 	mat4 lightSpaceMatrix;
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
		float farPlane;
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
	 	vec3 lightPos;
	 	mat4 lightSpaceMatrix;
	};
	
	// Valor de mezcla entre el color base que le pasa el usuario y el del material, cuanto menor sea el valor mayor sera
	// el color del material y cuanto mayor el del color. Si no tiene material, de base se pone en 0,99, si si tiene se pone a 0.
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
	
	uniform bool shadowsActivated;
	uniform sampler2DArray shadowMap;
	uniform samplerCube shadowCubes;
	int actualLayer = 0;
	
	float calculateShadow(vec3 lightPos, mat4 lightSpaceMatrix, vec3 normal, int mode){
		vec4 FragPosLight = lightSpaceMatrix * vec4(FragPos, 1.0f);
		// Como estas coordenadas las pasamos nosotros, OpenGL no hace la division de perspectiva, asi que la hacemos nosotros
		vec3 lightDir = normalize(lightPos - FragPos);
		vec3 projectionCoords = FragPosLight.xyz / FragPosLight.w;
		if(projectionCoords.z > 1.0f)
			return 0.0f; 
		// Estas coordenadas estan entre -1 y 1, pero las textura de sombras esta entre 0 y 1, asi que necesitamos ajustara
		projectionCoords = projectionCoords * 0.5f + 0.5f;
		// Ahora ya podemos sacar el fragmento mas cercano en profundidad usando la textura de sombras
		//float closest = texture(shadowMap, vec3(projectionCoords.xy, 0)).r;
		// Ahora que tenemos el mas cercano y el actual (la Z en el projectionCoords) podemos interpolar su valor de sombra
		float bias;
		if(mode == 0)
			bias = max(0.005f * (1.0f - dot(normal, lightDir)), 0.000005f);
		else
			bias = max(0.0005f * (1.0f - dot(normal, lightDir)), 0.00005f);
		float shadowValue = 0.0f;
		vec2 texelSize = 1.0f / vec2(textureSize(shadowMap, 0));
		for(int x = -1; x <= 1; x += 1){
			for(int y = -1; y <= 1; y += 1){
				float pcf = texture(shadowMap, vec3(projectionCoords.xy + vec2(x-1.75f, y) * texelSize, actualLayer)).r;
				shadowValue += projectionCoords.z - bias > pcf ? 1.0f : 0.0f;
			}
		}
		shadowValue /= 9.0f;
		actualLayer += 1;
		return shadowValue;
	}
	float closest = 0;
	float far = 0;
	float calculateShadow(vec3 lightPos, vec3 FragPos, float farPlane){
		vec3 fragToLight = FragPos - lightPos;
		float closestDepth = texture(shadowCubes, fragToLight).r;
		closestDepth *= farPlane;
		closest = closestDepth;
		far = farPlane;
		float currentDepth = length(fragToLight);
		float bias = 0.05f;
		float shadow = currentDepth - bias > closestDepth ? 1.0f : 0.0f;
		return shadow;
	}
	
	// Calculamos el efecto de las luces direccionales en el color del fragmento
	vec3 calculateDirectionalLight(DirectionalLight dLight, vec3 normal, vec3 cameraPos, vec3 color){
		// Sacamos y normalizamos la direccion con la que el rayo de luz colisiona con el fragmento desde la colision hacia fuera
		vec3 lightDir = normalize(-dLight.direction);
		//vec3 lightDir = normalize(dLight.lightPos - FragPos);
		// Sacamos la luz difusa entre 0 y el angulo entre la normal entre y la direccion de la luz
		float diffuseIntensity = max(dot(normal, lightDir), 0.0);
		//float diffuseIntensity = max(dot(lightDir, normal), 0.0);
		// Sacamos la luz especular sacando la direccion del reflejo de la direccion y la normal
		vec3 reflectDir = reflect(-lightDir, normal);
		//vec3 halfway = normalize(lightDir + cameraPos);
		// Ahora calculamos la intensidad de esta luz segun el angulo que hacen respecto a la camara y el brillo del material
		float specularIntensity = pow(max(dot(normal, reflectDir), 0.0), material.shininess);
		
		// Con la difusa y la especular calculadas, y sabiendo que el valor de la ambiental es siempre igual que la difusa en este sistema,
		// aplicamos las intensidades a los valores de las luces y sus texturas, las cuales las mezclamos con el color del modelo.
		vec3 ambient = dLight.ambient * mix(vec3(texture(material.diffuse, TexCoord)), material.color, mixValue);
		vec3 diffuse = dLight.diffuse * diffuseIntensity * mix(vec3(texture(material.diffuse, TexCoord)), material.color, mixValue);
		vec3 specular = dLight.specular * specularIntensity * mix(vec3(texture(material.specular, TexCoord)), material.color, mixValue);
		float shadow = 0.0f;
		if(shadowsActivated)
			shadow = calculateShadow(dLight.lightPos, dLight.lightSpaceMatrix, normal, 0);
		// Devolvemos el resultado
		//float shadow = 0.0f;
		return (ambient + (1.0f - shadow) * (diffuse + specular)) * mix(vec3(texture(material.diffuse, TexCoord)), material.color, mixValue);
	}
	
	vec3 calculatePointLight(PointLight pLight, vec3 normal, vec3 FragPos, vec3 viewDir, vec3 color){
		// Para la puntual necesitamos saber la direccion pero respecto al fragmento especifico
		vec3 lightDir = normalize(pLight.position - FragPos);
		// Difusa
		float diffuseIntensity = max(dot(normal, lightDir), 0.0);
		// Especular
		vec3 reflectDir = reflect(-lightDir, normal);
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
	
		// Aplicamos los calculos de las luces y la atenuacion
		vec3 ambient = (pLight.ambient * mix(vec3(texture(material.diffuse, TexCoord)), material.color, mixValue)) * attenuation;
		vec3 diffuse = (pLight.diffuse * diffuseIntensity * mix(vec3(texture(material.diffuse, TexCoord)), material.color, mixValue)) * attenuation;
		vec3 specular = (pLight.specular * specularIntensity * mix(vec3(texture(material.specular, TexCoord)), material.color, mixValue)) * attenuation;
		float shadow = 0.0f;
		if(shadowsActivated)
			shadow = calculateShadow(pLight.position, FragPos, pLight.farPlane);
		return (ambient + (1.0f - shadow) * (diffuse + specular)) * mix(vec3(texture(material.diffuse, TexCoord)), material.color, mixValue);
	}
	
	vec3 calculateSpotLight(SpotLight sLight, vec3 normal, vec3 FragPos, vec3 viewDir, vec3 color){
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
		vec3 ambient = sLight.ambient * mix(vec3(texture(material.diffuse, TexCoord)), material.color, mixValue) * attenuation * intensity;
		vec3 diffuse = sLight.diffuse * diffuseIntensity * mix(vec3(texture(material.diffuse, TexCoord)), material.color, mixValue) * attenuation *  intensity;
		vec3 specular = sLight.specular * specularIntensity * mix(vec3(texture(material.specular, TexCoord)), material.color, mixValue) * attenuation * intensity;
		float shadow = 0.0f;
		if(shadowsActivated)
			shadow = calculateShadow(sLight.lightPos, sLight.lightSpaceMatrix, normal, 1);
		return (ambient + (1.0f - shadow) * (diffuse + specular)) * mix(vec3(texture(material.diffuse, TexCoord)), material.color, mixValue);
	}
	
	void main(){
		// Creamos un colro base para el color que ira aumentando segun las luces
		vec3 color;
		if(n_directionalLights == 0 && n_directionalLights == 0 && n_directionalLights == 0){
			color = mix(vec3(texture(material.diffuse, TexCoord)), material.color, mixValue);
		}else{ 
			color = vec3(0.0f, 0.0f, 0.0f);
			// Importantisimo normalizar tanto la normal como la distancia entre el fragmento y la camara
			vec3 normal = normalize(Normal);
			vec3 viewDir = normalize(cameraPos - FragPos);
			
			// Segun el numero de luces que haya anadido el usuario, se calculara tantas veces se necesite cada tipo ce luz
			for(int i = 0; i < n_directionalLights; i++)
				color = calculateDirectionalLight(dLight[i], normal, viewDir, color);
			
			for(int i = 0; i < n_pointLights; i++)
				color += calculatePointLight(pLight[i], normal, FragPos, viewDir, color);
			
			for(int i = 0; i < n_spotLights; i++)
				color += calculateSpotLight(sLight[i], normal, FragPos, viewDir, color);
			
			// Una vez hemos sacado las luces, se pasa el resultado total
		}
		FragColor = vec4(color, material.alpha);
	};)";

static constexpr const char* basicLightShader = 
	R"(#version 430 core
	// Este es un shader basico para pintar puntos blancos donde estan las luces, uso exclusivo para 'ver' donde estan exactamente
	// de moemnto no le doy uso, pero a futuro me gustaria hacer una funcion que active y desactive este shader para que el usuario pueda
	// ver visualmente donde se ha colocado cada luz.
	
	out vec4 FragColor;
	
	void main(){
		FragColor = vec4(1.0f);
	};)";

static constexpr const char* basicShadowVertexShader =
	R"(#version 430 core
	layout (location = 0) in vec3 aPos;
	
	uniform mat4 model;
	
	void main(){
		gl_Position = model * vec4(aPos, 1.0f);
	};)";
	
static constexpr const char* basicShadowGeometryShader =
	R"(#version 430 core
	layout (triangles, invocations = 6) in;
	layout (triangle_strip, max_vertices = 3) out;
	layout (std140) uniform LightSpaceMatrices{mat4 lightSpaceMatrices[6];};
	
	void main(){
		for(int i = 0; i<3; i+=1){
			gl_Position = lightSpaceMatrices[gl_InvocationID] * gl_in[i].gl_Position;
			gl_Layer = gl_InvocationID;
			EmitVertex();
		}
		EndPrimitive();
	};)";

static constexpr const char* basicShadowFragmentShader =
	R"(#version 430 core
	
	void main(){
	
	};)";

static constexpr const char* basicShadowVertexShaderCubes =
	R"(#version 430 core
	layout (location = 0) in vec3 aPos;
	
	uniform mat4 model;
	
	void main(){
		gl_Position = model * vec4(aPos, 1.0f);
	};)";
	
static constexpr const char* basicShadowGeometryShaderCubes =
	R"(#version 430 core
	layout (triangles) in;
	layout (triangle_strip, max_vertices = 18) out;
	
	uniform mat4 shadowMatrices[6];
	
	out vec4 FragPos;
	
	void main(){
		for(int faces = 0; faces<6; ++faces){
			gl_Layer = faces;
			for(int i = 0; i<3; ++i){
				FragPos = gl_in[i].gl_Position;
				gl_Position = shadowMatrices[faces] * FragPos;
				EmitVertex();
			}
			EndPrimitive();
		}
	};)";

static constexpr const char* basicShadowFragmentShaderCubes =
	R"(#version 430 core
	in vec4 FragPos;
	
	uniform vec3 lightPos;
	uniform float farPlane;
	
	void main(){
		float lightDistance = length(FragPos.xyz - lightPos);
		lightDistance = lightDistance/farPlane;
		gl_FragDepth = lightDistance;
	};)";

static constexpr const char* basicSkyboxVertex =
	R"(#version 430 core
	layout (location = 0) in vec3 aPos;
	
	out vec3 texCoords;
	
	uniform mat4 projectionView;
	
	void main(){
		texCoords = aPos;
		vec4 pos = projectionView * vec4(aPos, 1.0f);
		gl_Position = pos.xyww;
	};)";

static constexpr const char* basicSkyboxFragment =
	R"(#version 430 core
	out vec4 FragColor;
	
	in vec3 texCoords;
	
	uniform samplerCube skybox;
	
	void main(){
		FragColor = texture(skybox, texCoords);
	};)";

static constexpr const char* basic2DVertex =
	R"(#version 430 core
	layout (location = 0) in vec3 aPos;
	layout (location = 1) in vec3 aNormal;
	layout (location = 2) in vec2 aTexCoord;
	
	out vec2 TexCoord;

	uniform vec2 imageScale = vec2(1.0f);
	uniform vec2 imagePos = vec2(0.0f);
	
	void main(){
		vec2 scaleAux = vec2(1.0f);
		vec2 imageAux = vec2(1.0f);
		vec3 pos = (aPos * vec3(imageScale, 1.0f)) + vec3(imagePos, 0.0f);
		gl_Position = vec4(pos, 1.0f);
		TexCoord = aTexCoord;
	};)";

static constexpr const char* basic2DFragment =
	R"(#version 430 core
	out vec4 FragColor;

	in vec2 TexCoord;

	struct Material{
		sampler2D diffuse;
		sampler2D specular;
		float shininess;
		vec3 color;
		float alpha;
	};

	uniform Material material;
	uniform float mixValue;
	
	void main(){
		float alpha = texture(material.diffuse, TexCoord).a;
		if(mixValue == 1.0f)
			alpha = 1.0f;
		FragColor = vec4(mix(vec3(texture(material.diffuse, TexCoord)), material.color, mixValue), material.alpha) * vec4(1.0f, 1.0f, 1.0f, alpha);
	};)";

static constexpr const char* basicTextVertex =
	R"(#version 430 core
	layout (location = 0) in vec4 vtx;
	out vec2 textCoords;

	uniform mat4 projection;

	void main(){
		gl_Position = projection * vec4(vtx.xy, 0.0f, 1.0f);
		textCoords = vtx.zw;
	};)";

static constexpr const char* basicTextFragment =
	R"(#version 430 core
	in vec2 textCoords;
	out vec4 FragColor;

	uniform sampler2D text;
	uniform vec4 textColor;

	void main(){
		FragColor = vec4(textColor) * vec4(1.0f, 1.0f, 1.0f, texture(text, textCoords).x);
	};)";

static constexpr const char* basicRayVertex =
	R"(#version 430 core
	layout (location = 0) in vec3 aPos;

	uniform mat4 mvp;

	void main(){
		gl_Position = mvp * vec4(aPos, 1.0f);
	};)";

static constexpr const char* basicRayFragment =
	R"(#version 430 core
	out vec4 FragColor;

	uniform vec4 color;

	void main(){
		FragColor = color;
	};)";