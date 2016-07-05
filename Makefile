TTLS = vopast.ttl vopamo.ttl manifest.ttl
CC = gcc
CFLAGS += -Wall -O3 -ffast-math -lm `pkg-config --cflags --libs lv2core` -fPIC
#CFLAGS += -DUSE_FAST_SQRT
DESTDIR ?= 
INSTALLDIR ?= $(DESTDIR)/usr/lib/lv2/
INSTALLNAME = vopa.lv2/

.PHONY : all clean install uninstall

all : libvopast.so libvopamo.so

libvopast.so: vopast.o
	$(CC) $(CFLAGS) vopast.o -shared -o libvopast.so

vopast.o : vopast.c vopast.h
	$(CC) $(CFLAGS) -c vopast.c -o vopast.o

libvopamo.so: vopamo.o
	$(CC) $(CFLAGS) vopamo.o -shared -o libvopamo.so

vopamo.o : vopamo.c vopast.h
	$(CC) $(CFLAGS) -c vopamo.c -o vopamo.o

clean :
	rm -f *.so *.o

install :
	mkdir -p $(INSTALLDIR)/$(INSTALLNAME)
	install *.so $(INSTALLDIR)/$(INSTALLNAME)
	install -m 644 $(TTLS) $(INSTALLDIR)/$(INSTALLNAME)

uninstall :
	rm -rf $(INSTALLDIR)/$(INSTALLNAME)
