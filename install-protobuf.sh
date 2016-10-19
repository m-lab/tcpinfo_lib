#!/bin/bash
# Alternative mechanism for checking and installing protobuf support.
# Prefer to use the CMake external project in CMakeLists.txt

set -x
# check to see if protobuf tools already exist.
LIBS=`/usr/bin/pkg-config --libs protobuf`
if [ $? -ne 0 ] ; then
  echo "Install protobuf from web."
  git clone git@github.com:google/protobuf.git
  cd protobuf && git checkout v3.1.0
  ./autogen.sh && ./configure --prefix=/usr && make && sudo make install && sudo ldconfig
else
  echo "Protobuf already installed."
fi
