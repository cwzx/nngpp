#!/usr/bin/env bash
set -euo pipefail

cd ~/Downloads
wget https://github.com/nanomsg/nng/archive/v1.0.1.tar.gz
gunzip -c v1.0.1.tar.gz | tar xvf -
cd nng-1.0.1
mkdir build
cd build
cmake ../
make -j
sudo make install
