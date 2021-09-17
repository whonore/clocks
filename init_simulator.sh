#!/bin/sh

set -e

if [ -z "$1" ] || [ ! -d "$1" ]; then
    echo "Usage: $0 [clock_dir]"
    exit 1
fi

simulator="$1/simulator"
if [ ! -d "$simulator" ]; then
    mkdir "$simulator"
fi

ln -srf simulator/arduino "$simulator"
ln -srf simulator/ino2c.py "$simulator"
ln -srf simulator/Makefile "$simulator"
