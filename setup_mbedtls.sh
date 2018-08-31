#!/usr/bin/env bash
set -euo pipefail

cd ~/Downloads
wget https://tls.mbed.org/download/start/mbedtls-2.12.0-apache.tgz
gunzip -c mbedtls-2.12.0-apache.tgz | tar xvf -
cd mbedtls-2.12.0
mkdir build
cd build
cmake ../
make -j
sudo make install
