{
  description = "Prologin's Stechec2";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-21.11";
    futils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, futils }:
    let
      inherit (nixpkgs) lib;
      inherit (lib) recursiveUpdate;
      inherit (futils.lib) eachDefaultSystem;

      anySystemOutputs = {
        overlay = final: prev: {
          stechec2 = final.callPackage ./nix/stechec2.nix { };
          mkStechec2Game = final.callPackage ./nix/lib.nix { };

          tictactoe = final.callPackage ./nix/tictactoe.nix { };
          plusminus = final.callPackage ./nix/plusminus.nix { };
        };
      };

      multipleSystemsOutpus = eachDefaultSystem (system:
        let
          pkgs = import nixpkgs {
            inherit system;
            overlays = [ self.overlay ];
          };
        in
        {
          packages = {
            inherit (pkgs) stechec2;
            inherit (pkgs) tictactoe plusminus;
          };

          devShell = pkgs.mkShell {
            buildInputs = lib.attrValues self.packages.${system};
          };
        }
      );
    in
    recursiveUpdate anySystemOutputs multipleSystemsOutpus;
}
