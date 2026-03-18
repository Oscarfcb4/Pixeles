CCFLAGS := #-Wall -Wpedantic -Wextra -Wconversion
APPNAME := build/build
CC      := g++
SRC		:= src
OBJ		:= obj
LIBS    := -lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lm ./libs/glad.c
CCVERS  := -std=c++2a
INC     := -Isrc/ -Ishaders/ -Isrc/utils/ -Ilibs/ 

ALLCPPS     := $(shell find src/ -type f -iname *.cpp)
OUTALLCPPS  := $(patsubst %.cpp, %.o, $(ALLCPPS))
OBJALLCPPS  := $(patsubst $(SRC)%, $(OBJ)%, $(OUTALLCPPS))
SUBDIRS     := $(shell find $(SRC) -type d)
OBJSUBDIRS  := $(patsubst $(SRC)%, $(OBJ)%, $(SUBDIRS))

.PHONY: paths

$(OBJ)/%.o : $(SRC)/%.cpp 
	$(CC) -c -o $@ $(CCFLAGS) $(CCVERS) $(INC) $^ 

$(APPNAME): $(OBJALLCPPS)
	$(CC) -o $(APPNAME) $(OBJALLCPPS) $(LIBS)

$(OBJALLCPPS): | $(OBJ)

$(OBJ):
	mkdir $(OBJ)
	mkdir $(OBJ)/utils

paths:
	$(OBJALLCPPS)

clean:
	rm -rf obj
	rm -f $(APPNAME)
	clear

release:
	tar -cf release.zip ./OutsideTheDarkness ./libs ./img ./bin ./music