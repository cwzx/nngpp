#!/usr/bin/env bash
set -euo pipefail

cd ~/Downloads
wget https://github.com/catchorg/Catch2/archive/v2.3.0.tar.gz
gunzip -c v2.3.0.tar.gz | tar xvf -
cd Catch2-2.3.0
mkdir build
cd build
cmake ../
make -j
sudo make install
