#!/bin/bash
(
    cd src && \
    make -f Makefile.xmingw32 clean && \
    make -f Makefile.xmingw64 clean && \
    make clean && \
    make
)

exit $?
