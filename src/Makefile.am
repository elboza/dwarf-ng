EXTRA_DIST = lang.l lang.y
AUTOMAKE_OPTION = nostdinc no-dependencies
#dwarf_LDADD = 
#AM_CFLAGS = -Wall
bin_PROGRAMS = dwarf
y.tab.o: y.tab.c
y.tab.c: lang.y lang.l
	$(YACC) $(YFLAGS) lang.y -o y.tab.c
lex.yy.o: lex.yy.c
lex.yy.c: lang.l lang.h
	$(LEX) $(LFLAGS) lang.l

LFLAGS = 
YFLAGS = -y -d -v

DWARF_LIB_OBJECTS = y.tab.o lex.yy.o
BUILT_SOURCES = y.tab.c y.tab.h lex.yy.c
CLEANFILES=$(BUILT_SOURCES) y.output 
dwarf_SOURCES = $(BUILT_SOURCES)\
		dw_elf_utils.h \
		dw_elf_utils.c \
		dw_macho_utils.h \
		dw_macho_utils.c \
		dw_pe_utils.h \
		dw_pe_utils.c \
		dw_readline_completion.h \
		dw_readline_completion.c \
		elf.h \
		lang.h \
		lang2.c \
		macho.h \
		main.h \
		main.c \
		mzexe.h \
		output.h \
		output.c \
		pe.h \
		stdint.h \
		utils.h \
		utils.c \
		vars.h \
		vars.c

noinst_HEADERS = 

man_MANS = 

#distdir=../$(PACKAGE)-$(VERSION)
