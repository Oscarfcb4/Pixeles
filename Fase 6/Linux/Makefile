cc      = ccache g++
LIBPATH = -Wl,-rpath,./libs
LIBS    = -L./libs/ ./libs/glad.c -lglfw3 -lGLU -lGL -lX11 
INC     = -Iinc/ -I../ -IShaders/

build: fase2.o
	$(cc) -o build ./Fase3.o ./Shader.o ./ResourceManager.o ./SceneTree.o $(LIBPATH) $(LIBS)

fase2.o:
	$(cc) -c Fase3.o -std=c++2a $(INC) ./Fase3.cpp ./Shaders/Shader.cpp ./ResourceManager/ResourceManager.cpp ./SceneTree/SceneTree.cpp

# Primero -> ccache g++ -std=c++2a -c -o fase2.o ./fase2.cpp  -Wl,-rpath,./libs/ -Iinc/ -L./libs/
# Segundo -> ccache g++ -o build ./fase2.o -Wl,-rpath,./libs -L./libs/ -lglfw3 -lGLU -lGL -lX11 ./libs/glad.c

clean:
	rm -rf ./Fase3.o
	rm -rf ./Shader.o
	rm -rf ./ResourceManager.o
	rm -rf ./SceneTree.o
	rm -f ./build
	rm -f ./libs/build
	clear
