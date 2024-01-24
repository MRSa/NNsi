CC=m68k-palmos-gcc
CFLAGS=-O2 -Wall -DUSE_CLIE -DUSE_DIA
OBJRES=m68k-palmos-obj-res

all: a2da.prc

a2da.prc: code03e8.a2da.grc a2da.ro
	build-prc --no-check-resources -o $@ -n A2DA -t 'DAcc' -c 'a2DA' code03e8.a2da.grc a2da.ro
	
code03e8.a2da.grc: a2da.o a2da-entry.o
	$(CC) $(CFLAGS) -nostartfiles a2da-entry.o a2da.o -o a2da
	$(OBJRES) a2da
	mv code0001.a2da.grc code03e8.a2da.grc
	
a2da.ro a2da-rsc-defines.h: a2da.rcp
	pilrc -Fj -H a2da-rsc-defines.h -q -ro a2da.rcp

a2da.o: a2da.c a2da-rsc-defines.h a2da-defines.h
	${CC} ${CFLAGS} -o a2da.o -c a2da.c

clean:
	rm -f *.bin *.grc *~ *.ro *.o a2da
	
