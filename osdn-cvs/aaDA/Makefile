CC=m68k-palmos-gcc
CFLAGS=-O2 -Wall
OBJRES=m68k-palmos-obj-res

all: aada.prc

aada.prc: code03e8.aada.grc aada.ro
	build-prc --no-check-resources -o $@ -n AADA -t 'DAcc' -c 'moAA' code03e8.aada.grc aada.ro
	
code03e8.aada.grc: aada.o aada-entry.o
	$(CC) $(CFLAGS) -nostartfiles aada-entry.o aada.o -o aada
	$(OBJRES) aada
	mv code0001.aada.grc code03e8.aada.grc
	
aada.ro aada-rsc-defines.h: aada.rcp
	pilrc -Fj -H aada-rsc-defines.h -q -ro aada.rcp

aada.o: aada.c aada-rsc-defines.h aada-defines.h
	${CC} ${CFLAGS} -o aada.o -c aada.c

clean:
	rm -f *.bin *.grc *~ *.ro *.o aada
	
