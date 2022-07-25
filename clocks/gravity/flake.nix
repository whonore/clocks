{
  inputs.nixpkgs.url = "github:nixos/nixpkgs";

  outputs = {
    self,
    nixpkgs,
  }: let
    env = system: let
      pkgs = nixpkgs.legacyPackages.${system};
      py-packages = python-packages: with python-packages; [pillow];
      python-png = pkgs.python3.withPackages py-packages;
    in
      pkgs.mkShell {packages = [python-png];};
  in {
    devShells.aarch64-darwin.default = env "aarch64-darwin";
    devShells.aarch64-linux.default = env "aarch64-linux";
    devShells.i686-linux.default = env "i686-linux";
    devShells.x86_64-darwin.default = env "x86_64-darwin";
    devShells.x86_64-linux.default = env "x86_64-linux";
  };
}
