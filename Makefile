xcomp: main.o look_debug.o utils.o
	gcc -o dwarf main.o utils.o look_debug.o -ggdb

main.o : main.h main.c
	gcc -c main.c -ggdb
look_debug.o: look_debug.h look_debug.c
	gcc -c look_debug.c -ggdb
utils.o: utils.h utils.c
	gcc -c utils.c -ggdb

clean:
	rm dwarf *.o


