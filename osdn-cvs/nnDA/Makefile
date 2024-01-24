CC=m68k-palmos-gcc
CFLAGS=-O2 -Wall -DUSE_CLIE -DUSE_REAL_WIDTH -DUSE_DIA -DUSE_SONY_JPEG -DUSE_JPEGLIB -DUSE_PNOJPEGLIB2
OBJRES=m68k-palmos-obj-res

all: nnda.prc

nnda.prc: code03e8.nnda.grc nnda.ro
	build-prc --no-check-resources -o $@ -n NNsiInfoDA -t 'DAcc' -c 'nnDA' code03e8.nnda.grc nnda.ro

code03e8.nnda.grc: nnda.o nnda-entry.o
	$(CC) $(CFLAGS) -nostartfiles nnda-entry.o nnda.o -o nnda
	$(OBJRES) nnda
	mv code0001.nnda.grc code03e8.nnda.grc

nnda.ro nnda-rsc-defines.h: nnda.rcp
	pilrc -Fj -H nnda-rsc-defines.h -q -ro nnda.rcp

nnda.o: nnda.c nnda-rsc-defines.h nnda-defines.h
	${CC} ${CFLAGS} -o nnda.o -c nnda.c

clean:
	rm -f *.bin *.grc *~ *.ro *.o nnda

pnojpeglib1:
	$(MAKE) clean
	$(MAKE) "CFLAGS=-O2 -Wall -DUSE_CLIE -DUSE_REAL_WIDTH -DUSE_DIA -DUSE_SONY_JPEG -DUSE_JPEGLIB"
