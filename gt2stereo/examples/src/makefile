all: ../example1.prg ../example2.prg

../example1.prg: example1.s example1.bin
		dasm example1.s -o../example1.prg -v3 -p3
		pucrunch ../example1.prg ../example1.prg -x2048

../example2.prg: example2.s example2.bin
		dasm example2.s -o../example2.prg -v3 -p3
		pucrunch ../example2.prg ../example2.prg -x2048

