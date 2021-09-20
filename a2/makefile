all:
	gcc -Wall -Werror -pthread -o s-talk tcp.c list.c
clean:
	rm s-talk
localone:
	./s-talk 1234 127.0.0.1 4321
localtwo:
	./s-talk 4321 127.0.0.1 1234	
asbone:
	./s-talk 1234 asb9804u-a01 4321
asbtwo:
	./s-talk 4321 asb9804u-a03 1234

	

