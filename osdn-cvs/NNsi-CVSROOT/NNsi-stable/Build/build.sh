#!/bin/sh
echo "----- Start Building NNsi Binaries -----"
rm -f ../OBJ/*.prc
cp -p Makefile.small Makefile
make clean;make
cp -p Makefile.std  Makefile
make clean;make
cp -p Makefile.arm Makefile
Make clean;make;make clean
echo "-------------  Done  -------------------"
