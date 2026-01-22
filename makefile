# Collect all .c files and convert them to .o object files
OBJ := $(patsubst %.c, %.o, $(wildcard *.c))

# Target to build the executable
a.out: $(OBJ)
	gcc $(OBJ) -o minishell

# Clean rule to remove generated files
clean:
	rm *.o  minishell