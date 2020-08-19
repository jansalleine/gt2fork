# makefile for linux

PREFIX=$(PWD)/../linux/
SUFFIX=
LIBS=`sdl2-config --libs`

CC?=gcc
CXX?=g++
CFLAGS=`sdl2-config --cflags` -O3 -Wall -I'bme' -I'asm'
CXXFLAGS=$(CFLAGS) -fpermissive

include Makefile.inc

.PHONY: install uninstall

install:
	./install.sh

uninstall:
	./uninstall.sh
