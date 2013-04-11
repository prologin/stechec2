#! /bin/sh

SUCCESS=
ERROR=
NOBUILD=

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
    g++ -std=c++11 -o runner ../runner.cc ./champion.so
    echo "Running test"
    if ! ./runner; then
        ERROR="$ERROR $lang"
    else
        SUCCESS="$SUCCESS $lang"
    fi
    cd ..
}

runtest caml prologin.ml
runtest cxx prologin.cc
runtest python prologin.py

echo
echo "---> RESULTS:"
echo "       BUILD FAILED: $NOBUILD"
echo "       FAILED: $ERROR"
echo "       SUCCEEDED: $SUCCESS"
