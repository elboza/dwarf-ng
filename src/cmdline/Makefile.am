EXTRA_DIST = shlang.l shlang.y shlang.h
AUTOMAKE_OPTION = nostdinc no-dependencies
AM_LFLAGS =
AM_YFLAGS = -y -d -v
bin_PROGRAMS= dwarf
y.tab.o: y.tab.c
y.tab.c: shlang.y shlang.l
	$(YACC) $(AM_YFLAGS) shlang.y -o y.tab.c
lex.yy.o: lex.yy.c
lex.yy.c: shlang.l shlang.h
	$(LEX) $(AM_LFLAGS) shlang.l

DWARF_LIB_OBJECTS = y.tab.o lex.yy.o
BUILT_SOURCES = y.tab.c lex.yy.c y.tab.h
CLEANFILES=$(BUILT_SOURCES) y.output
dwarf_SOURCES= 	$(BUILT_SOURCES)\
		main.c\
		main.h\
		dw_readline_completion.c\
		dw_readline_completion.h\
		repl.c\
		repl.h\
		output.c\
		output.h\
		sh_switchers.h\
		sh_switchers.c\
		sh_macho_utils.h\
		sh_macho_utils.c\
		sh_elf_utils.h\
		sh_elf_utils.c\
		sh_pe_utils.h\
		sh_pe_utils.c
LIBS=../libdwarf/libdwarf.a
dwarf_CFLAGS= -Wl,--no-as-needed -O0 -lm -lreadline
#nodist_dwarf_SOURCES= $(BUILT_SOURCES)
