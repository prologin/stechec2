{ pkgs, stechec2, ... }:


let
  stechecWithGame = pkgs.stdenv.mkDerivation {
    name = "stechecWithGame";
    version = "1.0";

    src = ../games/tictactoe;

    builder = pkgs.writeShellScript "builder" ''
      export PATH="${pkgs.coreutils}/bin"
      cp --no-preserve=mode,ownership -r ${stechec2.src} $out
      cp --no-preserve=mode,ownership -r $src $out/games/tictactoe
    '';
  };
in

pkgs.stdenv.mkDerivation {
  inherit (stechec2.deps) buildInputs nativeBuildInputs checkInputs;

  name = "tictactoe";
  version = "1.0";

  src = stechecWithGame;

  wafConfigureFlags = ["--with-games=tictactoe" "--games-only"];
}
