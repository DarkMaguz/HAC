#!/bin/bash

cd /tmp/app
./configure --prefix=/usr
make -j8
make install-strip

cd /
rm -rf /tmp/app

