#!/bin/bash

SCRIPT_DIR=$(cd "$(dirname $0)"; pwd)
. $SCRIPT_DIR/../util.sh
. $SCRIPT_DIR/version.sh
set_prefix
set_build_dir

. $PREFIX_OPT/env.sh

sh $SCRIPT_DIR/setup.sh

BUILD_TYPES="Release Debug"
for build_type in $BUILD_TYPES; do
  cd $BUILD_DIR
  rm -rf Elemental-$ELEMENTAL_VERSION-build-$build_type
  mkdir -p Elemental-$ELEMENTAL_VERSION-build-$build_type && cd Elemental-$ELEMENTAL_VERSION-build-$build_type
  check cmake -DCMAKE_BUILD_TYPE="Hybrid$build_type" -DCMAKE_INSTALL_PREFIX=$PREFIX_ROKKO/$build_type \
    -DCMAKE_C_COMPILER=gcc -DCMAKE_Fortran_COMPILER=gfortran \
    -DMATH_LIBS="-Wl,-framework -Wl,vecLib" \
    -DELEM_SHARED_LIBRARIES=ON \
    -DCMAKE_INSTALL_RPATH="$PREFIX_ROKKO/$build_type/lib" -DCMAKE_SKIP_BUILD_RPATH=OFF -DCMAKE_BUILD_WITH_INSTALL_RPATH=OFF -DCMAKE_INSTALL_RPATH_USE_LINK_PATH=ON -DCMAKE_MACOSX_RPATH=1 \
    $BUILD_DIR/Elemental-$ELEMENTAL_VERSION
  check make VERBOSE=1 -j4
  $SUDO make install
done