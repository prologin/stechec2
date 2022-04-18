{ pkgs, ... }:

let
  stechecPython = pkgs.python39.withPackages (ps: [ ps.pyyaml ps.jinja2 ] );

  stechec-unwrapped = pkgs.stdenv.mkDerivation rec {
    name = "prologin-stechec2-unwrapped";
    version = "1.0";

    src = ./..;
    passthru.src = ./..;

    nativeBuildInputs = with pkgs; [
      pkg-config
      wafHook
    ];

    buildInputs = with pkgs; [
      zeromq
      cppzmq
      gflags
      python39
    ];

    checkInputs = with pkgs; [
      gtest
      gcovr
    ];

    passthru.deps = {
      inherit nativeBuildInputs buildInputs checkInputs;
    };

    fixupPhase = ''
      mv $out/bin/stechec2-run $out/bin/stechec2-run-unwrapped
      rm $out/bin/stechec2-generator
    '';
  };

  stechec2-run-wrapper = pkgs.writeShellScript "stechec2-run-wrapper" ''
    export PYTHONPATH="${stechecPython}/${stechecPython.sitePackages}"
    exec ${stechecPython}/bin/python ${stechec-unwrapped}/bin/stechec2-run-unwrapped $*
  '';

  stechec2-generator-wrapper = pkgs.writeShellScript "stechec2-generator-wrapper" ''
    export PYTHONPATH="${stechecPython}/${stechecPython.sitePackages}:${stechec-unwrapped}/lib/stechec2"
    exec ${stechecPython}/bin/python -m generator $*
  '';

in

pkgs.stdenv.mkDerivation {
  name = "prologin-stechec2";
  version = "1.0";

  passthru = stechec-unwrapped.passthru;

  builder = pkgs.writeShellScript "builder.sh" ''
    export PATH="${pkgs.coreutils}/bin"
    mkdir -p $out/bin
    cp -r ${stechec2-run-wrapper}  $out/bin/stechec2-run
    cp -r ${stechec2-generator-wrapper} $out/bin/stechec2-generator
    cp -r ${stechec-unwrapped}/* $out
  '';
}
