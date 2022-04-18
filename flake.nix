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
        lib = import ./nix/lib.nix { inherit pkgs; };
        packages = futils.lib.flattenTree {
          stechec2 = import ./nix/stechec2.nix { inherit pkgs lib; };

          tictactoe = import ./nix/tictactoe.nix { inherit lib; inherit (packages) stechec2; };
          plusminus = import ./nix/plusminus.nix { inherit lib; inherit (packages) stechec2; };
        };

        defaultPackage = packages.stechec2;
        devShell = pkgs.mkShell {
          buildInputs = (pkgs.lib.attrValues packages);
        };
      }
    );
}
