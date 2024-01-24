#!/bin/sh
#----------------------------------------------------------------------------
# $Id: build.sh,v 1.3 2003/02/06 15:39:45 mrsa Exp $
#----------------------------------------------------------------------------
echo "----- Start Building NNsi Binaries -----"
rm -f ../OBJ/*.prc
cp -p Makefile.small Makefile
make clean;make
cp -p Makefile.std  Makefile
make clean;make
cp -p Makefile.arm Makefile
Make clean;make;make clean
echo "-------------  Done  -------------------"
