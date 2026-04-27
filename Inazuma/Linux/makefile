CCFLAGS := -Wall -Wpedantic -Wextra -Wconversion -fsanitize=address -g

Inazuma.a: Inazuma.o
	mv ./libInazuma.so ../../libs/

Inazuma.o:
	ccache g++ $(CCFLAGS) -std=c++2a -shared -o libInazuma.so -fPIC Inazuma.cpp -Wl,-rpath,../libs -I../