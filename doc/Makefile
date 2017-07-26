.PHONY: clean

all: docs

docs:
	if [ -f /usr/bin/man2html ];	then man2html <dwarf.man>dwarf.man.html ;fi
	
clean:
	rm -f  dwarf.man.html

