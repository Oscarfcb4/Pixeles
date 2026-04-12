#include "shader.hpp"

using namespace std;

Shader::Shader(const char* vertexPath, const char* fragmentPath){
	// Seteamos las variables iniciales de almacenamiento y lectura
	string vertexCode{}, fragmentCode{};
	ifstream vShaderFile{}, fShaderFile{};
	// Permitimos a los ifstream lanzar excepciones
	vShaderFile.exceptions (ifstream::failbit | ifstream::badbit);
	fShaderFile.exceptions (ifstream::failbit | ifstream::badbit);
	try{
		// Abrimos los archivos
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		// Creamos as variables para el flujo de datos
		stringstream vShaderStream{}, fShaderStream{};
		// Cargamos el buffer de los archivos en los flujos
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// Cerramos los archivos
		vShaderFile.close();
		fShaderFile.close();
		// Convertimos el flujo de datos en strings
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}catch(ifstream::failure e){
		cout << "ERROR::SHADER::FILE_NO_READED" << endl;
	}
	// Lo pasamos a cadenas de char y nos llevamos el puntero donde empiezan
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();
	// Ahora con todo leido vamos a compilar y linkar
	unsigned int vertex, fragment;
	int success;
	char infoLog[512];

	// VertexShader
	// Inicializamos el VS
	vertex = glCreateShader(GL_VERTEX_SHADER);
	// Le decimos cual es el codigo 
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	// Compilamos
	glCompileShader(vertex);
	// Miramos su status para ver si la compilacion ha sido exitosa
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	// Si no lo ha sido, obtenemos el Log de error y lanzamos un mensaje
	if(!success){
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		cout << "ERROR::SHADER::VERTEX::COMPLIATION_FAILED" << infoLog << endl;
	}

	// FragmentShader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if(!success){
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		cout << "ERROR::SHADER::FRAGMENT::COMPLIATION_FAILED" << infoLog << endl;
	}

	// ShaderProgram
	// Creamos el SP, le unimos los shader y linkamos
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	// Comprobamos que haya salido bien
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if(!success){
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		cout << "ERROR::SHADER::PROGRAM::LINKER_FAILED" << infoLog << endl;
	}
	// Limpiamos
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader::use(){
	glUseProgram(ID);
}

void Shader::setBool(const string &name, bool value) const{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), static_cast<int>(value));
}

void Shader::setInt(const string &name, int value) const{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const string &name, float value) const{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::set4Float(const string &name, float value1, float value2, float value3, float value4) const{
	glUniform4f(glGetUniformLocation(ID, name.c_str()), value1, value2, value3, value4);
}

void Shader::setMatrix4fv(const std::string &name, const GLfloat *matrix) const {
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, matrix);
}