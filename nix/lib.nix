{ coreutils
, stechec2
, writeShellScript
, stdenv
}:

{ name
, game
, version
}:
let
  stechecWithGame = stdenv.mkDerivation {
    pname = name;
    inherit version;
    src = game;
    builder = writeShellScript "builder" ''
      ${coreutils}/bin/cp --no-preserve=mode,ownership -r ${stechec2.src} $out
      ${coreutils}/bin/cp --no-preserve=mode,ownership -r $src $out/games/${name}
    '';
  };
in
stdenv.mkDerivation {
  inherit (stechec2.deps) buildInputs nativeBuildInputs checkInputs;
  pname = name;
  inherit version;
  src = stechecWithGame;

  # this is needed to build player environment within waf (see prologin2021)
  preConfigure = let p = stechec2.stechecPython; in ''
    export PYTHONPATH="''${PYTHONPATH:+:}${p}/${p.sitePackages}"
    export WSCRIPT_SKIP_TESTS=1
  '';
  wafConfigureFlags = ["--with-games=${name}" "--games-only"];
}
