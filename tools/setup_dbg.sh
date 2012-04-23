#! /bin/sh

cp ~/stechec2/bin/stechec2-client ~/stechec2
cp ~/stechec2/bin/stechec2-server ~/stechec2
cp ~/stechec2/lib/libtictactoe.so ~/stechec2

cd ~/stechec2/tictactoe/cxx/
make distclean all
cd ~/stechec2
cp ~/stechec2/tictactoe/cxx/champion.so ~/stechec2

sudo cp ~/stechec2/libtictactoe.so /usr/lib/
