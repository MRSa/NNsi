CC=m68k-palmos-gcc
CFLAGS=-O2 -Wall -DUSE_CLIE -DUSE_REAL_WIDTH -DUSE_DIA -DUSE_SONY_JPEG -DUSE_JPEGLIB
OBJRES=m68k-palmos-obj-res

all: CaptureDA.prc

CaptureDA.prc: code03e8.capda.grc capda.ro
	build-prc --no-check-resources -o $@ -n CaptureDA -t 'DAcc' -c 'capD' code03e8.capda.grc capda.ro

code03e8.capda.grc: capda.o capda-entry.o
	$(CC) $(CFLAGS) -nostartfiles capda-entry.o capda.o -o capda
	$(OBJRES) capda
	mv code0001.capda.grc code03e8.capda.grc

capda.ro capda-rsc-defines.h: capda.rcp
	pilrc -Fj -H capda-rsc-defines.h -q -ro capda.rcp

capda.o: capda.c capda-rsc-defines.h capda-defines.h
	${CC} ${CFLAGS} -o capda.o -c capda.c

clean:
	rm -f *.bin *.grc *~ *.ro *.o capda
