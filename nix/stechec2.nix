{ cppzmq
, gcovr
, gflags
, gtest
, pkg-config
, stdenv
, symlinkJoin
, wafHook
, writeShellScriptBin
, zeromq
, python3
}:

let
  stechecPython = python3.withPackages (ps: [ ps.pyyaml ps.jinja2 ] );

  stechec-unwrapped = stdenv.mkDerivation rec {
    pname = "prologin-stechec2-unwrapped";
    version = "1.0";

    src = ./..;
    passthru.src = ./..;

    nativeBuildInputs = [
      pkg-config
      wafHook
    ];

    buildInputs = [
      zeromq
      cppzmq
      gflags
      stechecPython
    ];

    checkInputs = [
      gtest
      gcovr
    ];

    passthru.stechecPython = stechecPython;
    passthru.deps = {
      inherit nativeBuildInputs buildInputs checkInputs;
    };

    fixupPhase = ''
      mv $out/bin/stechec2-run $out/bin/stechec2-run-unwrapped
      rm $out/bin/stechec2-generator
    '';
  };

  stechec2-run-wrapper = writeShellScriptBin "stechec2-run" ''
    export PYTHONPATH="${stechecPython}/${stechecPython.sitePackages}"
    exec ${stechecPython}/bin/python ${stechec-unwrapped}/bin/stechec2-run-unwrapped $*
  '';

  stechec2-generator-wrapper = writeShellScriptBin "stechec2-generator" ''
    export PYTHONPATH="${stechecPython}/${stechecPython.sitePackages}:${stechec-unwrapped}/lib/stechec2"
    exec ${stechecPython}/bin/python -m generator $*
  '';

in
symlinkJoin rec {
  pname = "prologin-stechec2";
  version = "1.0";
  name = "${pname}-${version}";

  passthru = stechec-unwrapped.passthru;

  paths = [
    stechec-unwrapped
    stechec2-generator-wrapper
    stechec2-run-wrapper
  ];
}
