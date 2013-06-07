#!/bin/bash -x

PREFIX="$1"
test -z "$PREFIX" && PREFIX=$HOME/opt/rokko
echo "PREFIX = $PREFIX"
dir=`dirname $0`
SCRIPT_DIR=`cd $dir && pwd`

mkdir -p $HOME/build
cd $HOME/build
rm -rf elpa_lib-201305
tar zxf $HOME/source/elpa_lib-201305.tar.gz

cd $HOME/build/elpa_lib-201305
patch -p1 < $SCRIPT_DIR/elpa-cmake.patch

cd $HOME/build
rm -rf elpa_lib-201305-build && mkdir -p elpa_lib-201305-build && cd elpa_lib-201305-build
cmake -DCMAKE_INSTALL_PREFIX=$PREFIX -DCMAKE_C_COMPILER=mpicc -DCMAKE_Fortran_COMPILER=mpif90 -DCMAKE_Fortran_FLAGS="-O3 -ffree-line-length-none" -DSCALAPACK_LIB="-L$PREFIX/lib -lscalapack -Wl,-framework -Wl,vecLib" $HOME/build/elpa_lib-201305

make -j4
make install
