EXTRA_DIST = cfglang.l cfglang.y cfglang.h
AUTOMAKE_OPTION = nostdinc no-dependencies

AM_LFLAGS = -P cfgyy
AM_YFLAGS = -y -d -v -p cfgyy

y.tab.o: y.tab.c
y.tab.c: cfglang.y cfglang.l
	$(YACC) $(AM_YFLAGS) cfglang.y -o y.tab.c
lex.cfgyy.o: lex.yy.c
lex.cfgyy.c: cfglang.l cfglang.h
	$(LEX) $(AM_LFLAGS) cfglang.l

DWARF_LIB_OBJECTS = vars.o utils.o y.tab.o lex.yy.o
BUILT_SOURCES = y.tab.c lex.cfgyy.c y.tab.h
CLEANFILES=$(BUILT_SOURCES) y.output
BUILT_LIB_SOURCES = 	$(BUILT_SOURCES)\
			vars.c\
			vars.h\
			utils.c\
			utils.h\
			libdwarf.c\
			libdwarf.h\
			elf.h\
			macho.h\
			pe.h\
			mzexe.h\
			stdint.h\
			dw_switchers.h\
			dw_switchers.c\
			dw_elf_utils.h\
			dw_elf_utils.c\
			dw_macho_utils.h\
			dw_macho_utils.c\
			dw_pe_utils.h\
			dw_pe_utils.c

lib_LIBRARIES=libdwarf.a
libdwarf_a_SOURCES=     $(BUILT_LIB_SOURCES)

libdwarf_a_CFLAGS = -O0 -lm
#libdwarf_a_LIBADD =
#libdwarf_a_LDFLAGS =
noinst_HEADERS = 
#nodist_libdwarf_a_SOURCES=$(BUILT_SOURCES)
man_MANS = 

#distdir=../../$(PACKAGE)-$(VERSION)
