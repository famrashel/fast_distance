#!/bin/csh
# Script to clean up before installing.

set PYTHONSCRIPT='fast_distance'
set CFUNC='fast_distance'

set PWD=`pwd`
cd `dirname $0`

#printf 'Previous directory: %s\n' $PWD
#printf 'Current  directory: %s\n' `pwd`

sudo rm -f -r build $PYTHONSCRIPT.egg-info
sudo rm -f "_"$CFUNC".so"
rm -f *.pyc

sudo rm -f $CFUNC/$CFUNC"_wrap.c" $CFUNC/$CFUNC".py"

cd $PWD
