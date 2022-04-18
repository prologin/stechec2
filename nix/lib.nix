{ pkgs, ... }:

{

  mkStechec2Game = { name, game, stechec2, version }:
    let
      stechecWithGame = pkgs.stdenv.mkDerivation {
        inherit name version;
        src = game;
        builder = pkgs.writeShellScript "builder" ''
          ${pkgs.coreutils}/bin/cp --no-preserve=mode,ownership -r ${stechec2.src} $out
          ${pkgs.coreutils}/bin/cp --no-preserve=mode,ownership -r $src $out/games/${name}
        '';
      };
    in
      pkgs.stdenv.mkDerivation {
        inherit (stechec2.deps) buildInputs nativeBuildInputs checkInputs;
        inherit name version;
        src = stechecWithGame;
        preConfigure = let p = stechec2.stechecPython; in ''
          export PYTHONPATH="''${PYTHONPATH:+:}${p}/${p.sitePackages}"
          echo $PYTHONPATH
        '';
        wafConfigureFlags = ["--with-games=${name}" "--games-only"];
      };

}
