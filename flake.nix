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
        lib = import ./nix/lib.nix { inherit pkgs; };
      in
      rec {
        packages = futils.lib.flattenTree {
          stechec2 = import ./nix/stechec2.nix { inherit pkgs lib; };
          tictactoe = import ./nix/tictactoe.nix { inherit pkgs lib; stechec2 = packages.stechec2; };
        };

        defaultPackage = packages.stechec2;
        devShell = pkgs.mkShell {
          buildInputs = (pkgs.lib.attrValues packages);
        };
      }
    );
}
