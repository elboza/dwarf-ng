VERSION=`cat VERSION|grep VERSION=\"|cut -d '"' -f2`
BINDIR=/usr/local/bin
MANDIR=/usr/local/share/man/man1
SRC=dwarf
TARGET=dwarf
SRC_DIR=src
DOC_DIR=doc
DIST_DIR=dwarf
MANSRC=doc/${TARGET}.man
MANDST=man/${TARGET}.man
MANTARGET=${TARGET}.1
SHELL=/bin/bash

.PHONY: clean install uninstall dist

all:
	$(MAKE) -C src
	$(MAKE) -C doc

clean:
	if [ -d ${DIST_DIR} ]; then rmdir ${DIST_DIR}; fi
	$(MAKE) -C src clean
	$(MAKE) -C doc clean
install:
	echo "installing ${TARGET} to ${DESTDIR}${BINDIR}"
	mkdir -p ${DESTDIR}${BINDIR}
	cp -p ${SRC_DIR}/${SRC} ${DESTDIR}${BINDIR}/${TARGET}
	chmod 555 ${DESTDIR}${BINDIR}/${TARGET}
	mkdir -p ${DESTDIR}${MANDIR}
	cp -p ${MANSRC} ${DESTDIR}${MANDIR}/${MANTARGET}
	chmod 644 ${DESTDIR}${MANDIR}/${MANTARGET}
	if [ ! -f $(wildcard ~/.dwarfrc) ]; then cp src/dwarfrc ~/.dwarfrc ;echo ".."; fi
	
uninstall:
	rm -f ${DESTDIR}${BINDIR}/${TARGET}
	rm -f ${DESTDIR}${MANDIR}/${MANTARGET}
	
dist:
	mkdir -p ${DIST_DIR}/${SRC_DIR}
	cp ${SRC_DIR}/* ${DIST_DIR}/${SRC_DIR}/
	cp Makefile ${DIST_DIR}/
	cp VERSION ${DIST_DIR}/
	cp README.md ${DIST_DIR}/
	cp ChangeLog ${DIST_DIR}/
	cp LICENCE ${DIST_DIR}/
	mkdir -p ${DIST_DIR}/media
	cp media/* ${DIST_DIR}/media/
	mkdir -p ${DIST_DIR}/doc
	cp -p ${DOC_DIR}/* ${DIST_DIR}/${DOC_DIR}/
	COPYFILE_DISABLE=1 tar -cvzf ${TARGET}-${VERSION}.tar.gz ${DIST_DIR}/
	rm -rf ./${DIST_DIR}/*
	rmdir ${DIST_DIR}

help:
	@ echo "dwarf-ng Makefile help:"
	@ echo " "
	@ echo "Build options:"
	@ echo "=============="
	@ echo "  FLEX=osx  - to build on macosx systems (make FLEX=osx)"
	@ echo "              uses -ll instead of -lfl flex gcc flag"
	@ echo " "
	@ echo "Targets:"
	@ echo "========"
	@ echo "The following targets are available"
	@ echo "  help      - print this message"
	@ echo "  install   - install everything"
	@ echo "  uninstall - uninstall everything"
	@ echo "  clean     - remove any temporary files"
	@ echo "  dist      - make a dist .tar.gz tarball package"
