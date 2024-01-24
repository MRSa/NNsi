##############################################################################
# $Id: makefile,v 1.3 2004/12/29 06:30:58 mrsa Exp $
#
#  - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#   Makefile for NNsi
#  - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
##############################################################################

all:
	cd Build;$(MAKE)

clean:
	cd Build;$(MAKE) clean

reporter:
	$(MAKE) clean
	cd Build;$(MAKE) debug

debug:
	cd Build;$(MAKE) debug

header:
	cd Rsc;mv nnsh.h nnsh.h.bak;touch nnsh.h;pilrc -h nnsh.h nnsh.rcp.icon;rm -f *.bin

install:
	$(MAKE) clean
	$(MAKE) header
	$(MAKE) all
