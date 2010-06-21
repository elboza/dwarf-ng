
CLEANFILES= 

EXTRA_DIST = MANUAL.txt dwarf.man

man1_MANS = dwarf.man

# html stuff is installed in the documentation directories, too
htmldir = $(docdir)
#html_DATA = dwarf.man.html
doc_DATA = MANUAL.txt

all:	docs

docs:        dwarf.man.html

dwarf.man.html: dwarf.man
	if [ -f /usr/bin/man2html ];	then man2html dwarf.man>dwarf.man.html ;fi

clean:
	        rm -f  dwarf.man.html 

