{ lib, stechec2, ... }:

lib.mkStechec2Game {
  name = "tictactoe";
  game = ../games/tictactoe;
  stechec2 = stechec2;
  version = "1.0";
}
