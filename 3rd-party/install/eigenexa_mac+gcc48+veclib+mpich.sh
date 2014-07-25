#!/bin/bash -x

PREFIX="$1"
test -z "$PREFIX" && PREFIX=$HOME/opt/rokko
echo "PREFIX = $PREFIX"
dir=`dirname $0`
SCRIPT_DIR=`cd $dir && pwd`

mkdir -p $HOME/build
cd $HOME/build
rm -rf EigenExa-2.2a
if test -f $HOME/source/EigenExa-2.2a.tgz; then
  tar zxf $HOME/source/EigenExa-2.2a.tgz
else
  wget -O - http://www.aics.riken.jp/labs/lpnctrt/EigenExa-2.2a.tgz | tar zxf -
fi
cd $HOME/build/EigenExa-2.2a
patch -p1 < $SCRIPT_DIR/EigenExa-2.2a.patch

cd $HOME/build
rm -rf EigenExa-2.2a-build && mkdir -p EigenExa-2.2a-build && cd EigenExa-2.2a-build
cmake -DCMAKE_INSTALL_PREFIX=$PREFIX \
    -DCMAKE_C_COMPILER=mpicc -DCMAKE_Fortran_COMPILER=mpif90 \
    -DSCALAPACK_LIB="-L$PREFIX/lib -lscalapack -Wl,-framework -Wl,vecLib" \
    $HOME/build/EigenExa-2.2a

make -j4 VERBOSE=1
make install