cc      = ccache g++
LIBPATH = -Wl,-rpath,./libs
LIBS    = -L./libs/ ./libs/glad.c -lglfw3 -lGLU -lGL -lX11 
INC     = -Iinc/ -I../ -Ishaders/

build: fase2.o
	$(cc) -o build ./fase2.o ./shader.o $(LIBPATH) $(LIBS)

fase2.o:
	$(cc) -c fase2.o -std=c++2a $(INC) ./fase2.cpp ./shaders/shader.cpp

# Primero -> ccache g++ -std=c++2a -c -o fase2.o ./fase2.cpp  -Wl,-rpath,./libs/ -Iinc/ -L./libs/
# Segundo -> ccache g++ -o build ./fase2.o -Wl,-rpath,./libs -L./libs/ -lglfw3 -lGLU -lGL -lX11 ./libs/glad.c

clean:
	rm -rf ./fase2.o
	rm -f ./build
	rm -f ./libs/build
	clear