#!/bin/bash -x

PREFIX="$1"
test -z "$PREFIX" && PREFIX=$HOME/opt/rokko
echo "PREFIX = $PREFIX"
dir=`dirname $0`
SCRIPT_DIR=`cd $dir && pwd`

mkdir -p $HOME/build
cd $HOME/build
rm -rf elpa_lib-201305 elpa_lib-201305-build
tar zxf $HOME/source/elpa_lib-201305.tar.gz

cd $HOME/build/elpa_lib-201305
patch -p1 < $SCRIPT_DIR/elpa_lib-201305.patch

cd $HOME/build
mkdir -p elpa_lib-201305-build && cd elpa_lib-201305-build
if [ `which mpicxx > /dev/null 2>&1; echo $?` = 0 ]; then
  cmake -DCMAKE_C_COMPILER=mpicc \
      -DCMAKE_Fortran_COMPILER=mpif90 -DCMAKE_Fortran_FLAGS="-O3 -xSSE3" \
      -DSCALAPACK_LIB="-lmkl_scalapack_lp64 -lmkl_blacs_intelmpi_lp64 -mkl=parallel" \
      -DCMAKE_INSTALL_PREFIX=$PREFIX $HOME/build/elpa_lib-201305
else
  cmake -DCMAKE_C_COMPILER=icc \
      -DCMAKE_Fortran_COMPILER=ifort -DCMAKE_Fortran_FLAGS="-O3 -xSSE3" \
      -DMPI_C_INCLUDE_PATH="/usr/include" -DMPI_C_LIBRARIES="-lmpi" \
      -DMPI_Fortran_LIBRARIES="-lmpi" \
      -DSCALAPACK_LIB="-lmkl_scalapack_lp64 -lmkl_blacs_intelmpi_lp64 -mkl=parallel" \
      -DCMAKE_INSTALL_PREFIX=$PREFIX $HOME/build/elpa_lib-201305
fi

make -j4 VERBOSE=1
make install