{
  description = "A very basic flake";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-21.11";
  };

  outputs = { self, nixpkgs }: 
  let
    pkgs = import nixpkgs { system = "x86_64-linux"; };
  in

  {
    defaultPackage.x86_64-linux = ( import ./stechec2.nix { inherit pkgs; });
    packages.x86_64-linux.stechec2 = self.outputs.defaultPackage.x86_64-linux;
  };
}
