vopa
====

A LV2 plugin which control the volume and the panning of a stereo signal using Midi CC 7 and 10.

To build this plugin, you will need lv2-devel.

$ make 
$ sudo make install

You can specify install PATH via make:
$ sudo make INSTALLDIR=/usr/lib64/lv2 install

The DESTDIR variable can be used for packaging.