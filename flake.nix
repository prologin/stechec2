{
  description = "Prologin's Stechec2";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-21.11";
    futils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, futils }: 
    futils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
      in
      rec {
        packages = futils.lib.flattenTree {
          stechec2 = import ./nix/stechec2.nix { inherit pkgs; };
          tictactoe = import ./nix/tictactoe.nix { inherit pkgs; stechec2 = packages.stechec2; };
        };

        defaultPackage = packages.stechec2;
        devShell = pkgs.mkShell {
          buildInputs = (pkgs.lib.attrValues packages);
        };
      }
    );
}
