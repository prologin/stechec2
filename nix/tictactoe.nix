{ self, pkgs, ... }:


let
  stechec2-src = fetchGit {
    url = "https://github.com/prologin/stechec2.git";
    rev = "12a087716795e965354aac57904ee840f65fe789";
  };

  stechecWithGame = pkgs.stdenv.mkDerivation {
    name = "stechecWithGame";
    version = "1.0";

    src = ../games/tictactoe;

    builder = pkgs.writeShellScript "builder" ''
      export PATH="${pkgs.coreutils}/bin"
      cp --no-preserve=mode,ownership -r ${stechec2-src} $out
      cp --no-preserve=mode,ownership -r $src $out/games/tictactoe
    '';
  };
in

pkgs.stdenv.mkDerivation {
  name = "tictactoe";
  version = "1.0";

  src = stechecWithGame;

  buildInputs = with pkgs; [
    gtest
    zeromq
    cppzmq
    gcc
    gflags
    gcovr
    pkg-config
    wafHook
  ];

  preConfigure = ''
    wafConfigureFlags="--with-games=tictactoe --games-only"
  '';
}
