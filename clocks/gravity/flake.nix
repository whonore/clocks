{
  inputs.nixpkgs.url = "github:nixos/nixpkgs";

  outputs = {
    self,
    nixpkgs,
  }: let
    pkgs = nixpkgs.legacyPackages.x86_64-linux;
    py-packages = python-packages: with python-packages; [pillow];
    python-png = pkgs.python3.withPackages py-packages;
  in {
    devShells.x86_64-linux.default = pkgs.mkShell {
      packages = [python-png];
    };
  };
}
