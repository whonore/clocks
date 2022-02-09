#!/bin/sh
set -e

TOP=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd -P)/..

if [ -z "$1" ] || [ ! -d "$1" ]; then
    echo "Usage: $0 [clock_dir]"
    exit 1
fi

simulator="$1/simulator"
if [ ! -d "$simulator" ]; then
    mkdir "$simulator"
fi

ln -srf "$TOP"/simulator/arduino "$simulator"
ln -srf "$TOP"/simulator/ino2c.py "$simulator"
ln -srf "$TOP"/simulator/Makefile "$simulator"
