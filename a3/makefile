all:
	gcc -g -o simulation simulation.c list.c
valgrind: all
	valgrind --leak-check=full --show-leak-kinds=all ./simulation
clean:
	rm simulation
