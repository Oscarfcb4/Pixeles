cc      = ccache g++
LIBPATH = -Wl,-rpath,./libs
LIBS    = -L./libs/ ./libs/glad.c -lglfw3 -lGLU -lGL -lX11
INC     = -Iinc/ -I../

build: fase1.o
	$(cc) -o build ./fase1.o $(LIBPATH) $(LIBS)

fase1.o:
	$(cc) -c -o fase1.o -std=c++2a $(INC) ./fase1.cpp

# Primero -> ccache g++ -std=c++2a -c -o  fase1.o ./fase1.cpp  -Wl,-rpath,./libs/ -Iinc/ -L./libs/
# Segundo -> ccache g++ -o build ./fase1.o -Wl,-rpath,./libs -L./libs/ -lglfw3 -lGLU -lGL -lX11 ./libs/glad.c

clean:
	rm -rf ./fase1.o
	rm -f ./build
	rm -f ./libs/build
	clear