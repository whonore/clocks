{
  inputs.nixpkgs.url = "github:nixos/nixpkgs";

  outputs = { self, nixpkgs }:
    let
      pkgs = nixpkgs.legacyPackages.x86_64-linux;
      py-packages = python-packages: with python-packages; [ pillow ];
      python-png = pkgs.python3.withPackages py-packages;
    in {
      devShell.x86_64-linux = pkgs.mkShell { buildInputs = [ python-png ]; };
    };
}
