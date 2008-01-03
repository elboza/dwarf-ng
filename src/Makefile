xcomp: main.o look_debug.o utils.o lang2.o y.tab.o lex.yy.o vars.o
	gcc -o dwarf main.o utils.o look_debug.o lang2.o y.tab.o vars.o lex.yy.o -ggdb -lreadline

main.o : main.h main.c
	gcc -c main.c -ggdb
look_debug.o: look_debug.h look_debug.c
	gcc -c look_debug.c -ggdb
utils.o: utils.h utils.c
	gcc -c utils.c -ggdb
lang2.o: lang.l lang.y lang.h lang2.c
	bison -y -d lang.y
	flex lang.l
	gcc -c y.tab.c lex.yy.c -ggdb
	gcc -c lang2.c -ggdb
vars.o: vars.h vars.c
	gcc -c vars.c -ggdb

clean:
	rm dwarf y.tab.* *.o


