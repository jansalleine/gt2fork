# makefile for linux

PREFIX=$(PWD)/../linux/
SUFFIX=
LIBS=`sdl2-config --libs` -ljack -lasound

CC?=gcc
CXX?=g++
CFLAGS=`sdl2-config --cflags` -D__LINUX_ALSA__ -D__UNIX_JACK__ -DJACK_HAS_PORT_RENAME=1 -O3 -Wall -Wno-deprecated -I'asm'
CXXFLAGS=$(CFLAGS) -fpermissive
LDFLAGS=-Wl,-z,now

include Makefile.inc

.PHONY: install uninstall

install:
	./install.sh

uninstall:
	./uninstall.sh
