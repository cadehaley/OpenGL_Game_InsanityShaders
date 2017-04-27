#OBJS specifies which files to compile as part of the project
OBJS = insanity_main.cpp glib.cpp
FLAGS = -w -std=c++11 -lSDL2 -lSDL2_image -lGL -lGLU -lGLEW

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = insanity

#This is the target that compiles our executable
all : $(OBJS)
	g++ $(OBJS) $(FLAGS) -o $(OBJ_NAME)

clean:
	rm $(OBJ_NAME)
