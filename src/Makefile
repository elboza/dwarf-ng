xcomp: main.o utils.o lang2.o y.tab.o lex.yy.o vars.o dw_rl.o dw_elf_utils.o dw_macho_utils.o dw_pe_utils.o
	gcc -o dwarf main.o utils.o lang2.o y.tab.o dw_rl.o dw_elf_utils.o dw_macho_utils.o dw_pe_utils.o vars.o lex.yy.o -ggdb -lreadline

main.o : main.h main.c
	gcc -c main.c -ggdb
utils.o: utils.h utils.c
	gcc -c utils.c -ggdb
lang2.o: lang.l lang.y lang.h lang2.c
	bison -y -d lang.y
	flex lang.l
	gcc -c y.tab.c lex.yy.c -ggdb
	gcc -c lang2.c -ggdb
vars.o: vars.h vars.c
	gcc -c vars.c -ggdb
dw_rl.o: dw_readline_completion.c dw_readline_completion.h
	gcc -c dw_readline_completion.c -o dw_rl.o -ggdb
dw_elf_utils.o: dw_elf_utils.c dw_elf_utils.h
	gcc -c dw_elf_utils.c -ggdb
dw_macho_utils.o: dw_macho_utils.c dw_macho_utils.h
	gcc -c dw_macho_utils.c -ggdb
dw_pe_utils.o: dw_pe_utils.c dw_pe_utils.h
	gcc -c dw_pe_utils.c -ggdb

clean:
	rm dwarf y.tab.* *.o


