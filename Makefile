OBJS = main.c

CC = g++

# -w suppresses all warnings
# COMPILER_FLAGS = -w

LINKER_FLAGS = -lSDL3

OBJ_NAME = main

all: $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)
