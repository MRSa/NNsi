##############################################################################
# $Id: makefile,v 1.1.1.1 2003/08/15 12:14:30 mrsa Exp $
#
#  - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#   Makefile for NNsi
#  - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
##############################################################################

all:
	cd Build;$(MAKE)

clean:
	cd Build;$(MAKE) clean

header:
	cd Rsc;mv nnsh.h nnsh.h.bak;touch nnsh.h;pilrc -h nnsh.h nnsh.rcp;rm -f *.bin

install:
	$(MAKE) clean
	$(MAKE) header
	$(MAKE) all
