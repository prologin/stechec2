#! /bin/sh

SUCCESS=
ERROR=
NOBUILD=

NB_TESTS=${1:-10000}

runtest() {
    lang=$1
    champion_file=$2

    echo "================================================"
    echo "Testing $lang interface"
    echo "================================================"

    cp champions/$champion_file $lang
    cd $lang
    if ! make; then
        NOBUILD="$NOBUILD $lang"
        cd ..
        return
    fi
    g++ -std=c++11 -g -Wall -Wextra -o runner ../runner.cc ./champion.so
    echo "Running test"
    if ! ./runner $NB_TESTS; then
        ERROR="$ERROR $lang"
    else
        SUCCESS="$SUCCESS $lang"
    fi
    cd ..
}

runtest c prologin.c
runtest php prologin.php
runtest cs prologin.cs
runtest java Prologin.java
runtest caml prologin.ml
runtest cxx prologin.cc
runtest python prologin.py
runtest python2 prologin-py2.py
runtest haskell Prologin.hs

echo
echo "---> RESULTS:"
echo "       BUILD FAILED: $NOBUILD"
echo "       FAILED: $ERROR"
echo "       SUCCEEDED: $SUCCESS"
