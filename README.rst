.. |(version)| replace:: 1.0
.. -*- reStructuredText -*-

.. contents::

============================================
GoatTracker v2.73 & GoatTracker v2.74 Stereo
============================================

Editor by Lasse Öörni (loorni@gmail.com)
HardSID 4U support by Téli Sándor.
Uses reSID engine by Dag Lem.
Uses reSID distortion / nonlinearity by Antti Lankila.
Uses 6510 crossassembler from Exomizer2 beta by Magnus Lind.
Uses the SDL library.
GoatTracker icon by Antonio Vera.
Command quick reference by Simon Bennett.
Patches by Stefan A. Haubenthal, Valerio Cannone, Raine M. Ekman and Tero Lindeman.

Distributed under GNU General Public License
(see the file COPYING for details)

Covert BitOps homepage:
http://covertbitops.c64.org

GoatTracker 2 SourceForge.net page:
http://sourceforge.net/projects/goattracker2

=======
gt2fork
=======

GoatTracker modifications by Jan Wassermann aka Spider Jerusalem ^ MYD ^ TRSi

gt2fork homepage:
https://gt2fork.jansalleine.com/

=========
Compiling
=========
Linux:
------

**Mono Version**
    cd goattrk2/src/

    make

**Stereo Version**
    cd gt2stereo/src/

    make

Windows:
--------

I have no idea :-) Just build for windows on linux with mingw32.

**Mono Version**
    cd goattrk2/src/

    make -f makefile.xmingw32

**Stereo Version**
    cd gt2stereo/src/

    make -f makefile.xmingw32

Other OS:
---------

Not sure about it.
You'd have to adjust the makefiles accordingly to makefile.common.
I left those files as they were.

=======
Changes
=======

In no particular order:

- Replaced unnecessary "0" chars with "." chars
- Fine tuned colorscheming (all colors *should* be defined in goattrk2.h)
- Included current reSID from VICE project [http://vice-emu.sourceforge.net/]
