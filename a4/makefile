
all:
	gcc -o UnixLs UnixLs.c 

valgrind:
	valgrind --leak-check=full --show-leak-kinds=all ./UnixLs -lRi

clean:
	rm UnixLs
