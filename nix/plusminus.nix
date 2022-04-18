{ lib, stechec2, ... }:

lib.mkStechec2Game {
  name = "plusminus";
  game = ../games/plusminus;
  stechec2 = stechec2;
  version = "1.0";
}
