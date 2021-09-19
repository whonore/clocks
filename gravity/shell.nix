{ pkgs ? import <nixpkgs> { } }:
with pkgs;

let
  py-packages = python-packages: with python-packages; [ pillow ];
  python-png = python3.withPackages py-packages;
in mkShell { buildInputs = [ python-png ]; }
