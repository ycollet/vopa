vopa
====

Two plugins are provided:
 * VoPaSt - a stereo volume and panning plugin controlled via MIDI CC 7 and 10
 * VoPaMo - a mono volume plugin controlled via MIDI CC

To build this plugin, you will need lv2-devel.

```bash
$ make 
$ sudo make install
```

You can specify install PATH via make:
```bash
$ sudo make INSTALLDIR=/usr/lib64/lv2 install
```

The DESTDIR variable can be used for packaging.