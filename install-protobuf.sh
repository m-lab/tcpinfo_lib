#!/bin/bash
set -x
# check to see if protobuf folder is empty
LIBS=`/usr/bin/pkg-config --libs protobufxxx`
if [ $? -ne 0 ] ; then
  echo "Install protobuf from web."
  git clone git@github.com:google/protobuf.git
  cd protobuf && git checkout v3.1.0
  ./autogen.sh && ./configure --prefix=/usr && make && sudo make install
else
  echo "Protobuf already installed."
fi
