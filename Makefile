OBJECTS = vopa.o
LIBRARY = libvopa.so
TTLS = vopa.ttl manifest.ttl
CC = gcc
CFLAGS += -Wall -O3 -ffast-math -lm `pkg-config --cflags --libs lv2core` -fPIC
DESTDIR = /tmp/vopa
INSTALLDIR = $(DESTDIR)/usr/lib/lv2/
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
