#!/bin/bash

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
    g++ -std=c++14 -g -Wall -Wextra -o runner ../runner.cc ./champion.so
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
runtest rust prologin.rs
runtest haskell Prologin.hs

echo
echo "---> RESULTS:"
echo "       BUILD FAILED: $NOBUILD"
echo "       FAILED: $ERROR"
echo "       SUCCEEDED: $SUCCESS"

# Inconsistency checks

assertfails() {
    conf=$1
    shift
    needed=( "$@" )
    env_path="$(mktemp -d --suffix=-test-stechec-generator)"

    output="$(stechec2-generator player "$conf" "$env_path" 2>&1)"
    rm -r "$env_path"

    for string in "${needed[@]}"; do
        if [[ $output != *$string* ]]; then
            echo -e "$conf:\tshould have failed with '$string'; it did not."
            return
        fi
    done

    echo -e "$conf:\ttest passed"
}

echo
assertfails "./test-fail-multi-args.yml" "barfoo" "already defined"
assertfails "./test-fail-arg-type.yml" "sometype" "conflicts with type name"
assertfails "./test-fail-unknown-type.yml" "whatever" "does not denote a valid type"
