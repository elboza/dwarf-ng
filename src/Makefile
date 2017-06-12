CC=gcc
LIBFLEX=-lfl
ifeq ($(FLEX),osx)
LIBFLEX=-ll
endif
CFLAGS=-ggdb -Wall -std=gnu99
#CFLAGS=-ggdb
LIBS=-lreadline -lm $(LIBFLEX)
SRC=src
OBJECTS= main.o repl.o dw_elf_utils.o dw_macho_utils.o dw_pe_utils.o dw_readline_completion.o dw_switchers.o libdwarf.o output.o sh_elf_utils.o sh_macho_utils.o sh_pe_utils.o sh_switchers.o utils.o vars.o
GRAM= cfglang.tab.c shlang.tab.c lex.yy.c lex.cfgyy.c
RMGRAM= $(GRAM) cfglang.output shlang.output cfglang.tab.h shlang.tab.h
TARGET=dwarf

all: $(TARGET)

.PHONY: clean

$(TARGET): $(GRAM) $(OBJECTS)
	$(CC) $(OBJECTS) $(GRAM) $(LIBS) -o $(TARGET)
	
%.o : %.c
	$(CC) $(CFLAGS) -c $<
	
cfglang.tab.c cfglang.tab.h: cfglang.y cfglang.h
	bison -d -v -p cfgyy cfglang.y
	
lex.cfgyy.c: cfglang.l cfglang.h
	flex -P cfgyy cfglang.l

shlang.tab.c shlang.tab.h: shlang.y shlang.h
	bison -d -v shlang.y

lex.yy.c: shlang.l shlang.tab.h
	flex shlang.l

clean:
	rm -f *.o $(RMGRAM)
	rm -f $(TARGET)
