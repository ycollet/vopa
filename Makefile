OBJECTS = vopast.o vopamo.o
LIBRARY = libvopast.so libvopamo.so 
TTLS = vopast.ttl vopamo.ttl manifest.ttl
CC = gcc
CFLAGS += -Wall -O3 -ffast-math -lm `pkg-config --cflags --libs lv2core` -fPIC
DESTDIR ?= 
INSTALLDIR ?= $(DESTDIR)/usr/lib/lv2/
INSTALLNAME = vopa.lv2/

$(LIBRARY) : $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -shared -o $@

.SUFFIXES : .c .o

.c.o:
	$(CC) $(CFLAGS) -c $<

vopa.o : vopa.c

.PHONY : clean install uninstall

clean :
	rm -f $(LIBRARY) $(OBJECTS)

install :
	mkdir -p $(INSTALLDIR)$(INSTALLNAME)
	install $(LIBRARY) $(INSTALLDIR)$(INSTALLNAME)
	install -m 644 $(TTLS) $(INSTALLDIR)$(INSTALLNAME)

uninstall :
	rm -rf $(INSTALLDIR)$(INSTALLNAME)
