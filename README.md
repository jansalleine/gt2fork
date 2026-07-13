# gt2fork v0.7

Fork of **GoatTracker 2.75 / 2.76 Stereo** by Jan Wassermann

[https://github.com/jansalleine/gt2fork](https://github.com/jansalleine/gt2fork)

- Editor by Lasse Öörni and Jan Wassermann.
- Music playroutines by Lasse Öörni.
- Uses reSID engine by Dag Lem and Leandro Nini.
- Uses reSIDfp distortion / nonlinearity by Antti Lankila and Leandro Nini.
- Uses 6510 crossassembler from Exomizer2 beta by Magnus Lind.
- Uses the SDL2 library.
- Uses the RtMidi library.
- Uses parts of the GoatTracker icon by Antonio Vera.
- Command quick reference by Simon Bennett.
- Patches and further development by Stefan A. Haubenthal, Valerio Cannone,
  Raine M. Ekman, Tero Lindeman, Henrik Paulini and Groepaz.
- Microtonal support by Birgit Jauernig.

Music data for stereo and mono GoatTracker 2 is fully compatible with gt2fork.
This includes song *(\*.SNG)* and instrument *(\*.INS)* datafiles as well as the
C64 player routines.

## Resources

- [GoatTracker 2 on SourceForge.net](http://sourceforge.net/projects/goattracker2)
- [drfiemost reSID GitHub clone](https://github.com/drfiemost/resid)
- [vice-emu (inluding reSID)](https://sourceforge.net/projects/vice-emu/)
- [drfiemost reSIDfp GitHub clone](https://github.com/drfiemost/residfp)
- [sidplay-residfp on SourceForge.net](https://sourceforge.net/projects/sidplay-residfp/)
- [SDL2 project homepage](https://www.libsdl.org/)
- [RtMidi homepage](http://www.music.mcgill.ca/~gary/rtmidi/)

## Features

- Scalable window
- On-the-fly switching between mono *(1SID)* and stereo *(2SID)* mode instead of
  having two executables
- Custom palette now supports GIMP palette file format
- On-the-fly switching between a black and a blue (C64 basic editor style) theme

## Packages

You can find a PGKBUILD for Arch Linux on the AUR (maintained by myself).

[AUR gt2fork-git](https://aur.archlinux.org/packages/gt2fork-git)

## Compiling

gt2fork is developed on Arch Linux but should compile in any recent
Linux distribution with these libraries installed:

- sdl2
- alsa
- jack

### Linux for Linux
    cd src
    make

### Linux for Windows
32Bit:

    cd src
    make -f Makefile.xmingw32

64 Bit:

    cd src
    make -f Makefile.xmingw64

### Build scripts

You can also use the bash scripts in this directory which will also make clean
earlier compiles. Sometimes it's hard to keep track of which makefile was used
last.

## License

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

