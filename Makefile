.PHONY: clean

all:
	$(MAKE) -C src
	$(MAKE) -C doc

clean:
	$(MAKE) -C src clean
	$(MAKE) -C doc clean
