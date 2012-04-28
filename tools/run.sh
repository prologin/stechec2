#!/bin/bash

if [[ $# -lt 1 || "$1" == "--help" ]]; then
    cat <<EOF
Usage: run <config-file>

Examples:
  run toto.ini       to run a match, using configuration file 'toto.ini'

Report bugs to <serveur@prologin.org>
EOF
    exit 0
elif [ "$1" == "--version" ]; then
    cat <<EOF
run 0.0.1

Copyright (C) 2012, Prologin.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

Written by Maxime Audouin.
EOF
    exit 0
fi


clis=`grep '\\[stechec2.client.' $1`

stechec2-server -c "$1" &
pid=$!
pids="$pid"
for i in `echo "$clis"`; do
    echo "client : $i"
    name=`echo "$i" | cut -d . -f 3 | cut -d ] -f 1`;
    echo "name : $name"
    stechec2-client -c "$1" -n "$name" &
    pids="$pids $!"
done

# if the user wants to abort on 'wait', try to kill the server
# to avoid future port binding error
kill_server()
{
    for i in `echo "$pids"`; do
	kill $i
    done
}

trap "kill_server" INT

wait $pid
