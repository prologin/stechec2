#! /usr/bin/env bash
# This file is part of Stechec.
#
# Copyright (c) 2011 Pierre Bourdon <pierre.bourdon@prologin.org>
# Copyright (c) 2011 Association Prologin <info@prologin.org>
#
# Stechec is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Stechec is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Stechec.  If not, see <http://www.gnu.org/licenses/>.

if [ "$#" -ne 2 ]; then
    echo >&2 "Usage: $0 <stechec-root> <champion-dir>"
    exit 1
fi

stechec_root=$1
champion_dir=$2

makefile_dir=$stechec_root/makefile
compil_dir=`mktemp -d /tmp/stechec_compil_XXXXXX`
champion_tarball=$champion_dir/champion.tgz
compil_log=$champion_dir/compilation.log

(
    # We should not preempt the contestant tasks
    renice 5 $$ &>/dev/null

    if ! test -f "$champion_tarball"; then
        echo >&2 "*** Unable to locate the champion.tgz file. Upload failed?"
        exit 2
    fi
    echo "Champion tarball found. Extracting to $compil_dir."

    cd "$compil_dir"
    echo "Files contained in the tarball:"
    tar xzvf "$champion_tarball" 2>&1 | sed 's/^.*$/- &/'

    if test "${PIPESTATUS[0]}" -ne 0; then
        echo >&2 "*** Extraction failed. Corrupted tarball?"
        exit 1
    fi

    #cp "$makefile_dir/stechec_main.cc" .

    has_ext() {
        ext=$1
        [ "`echo *.$ext`" != "*.$ext" ]
    }

    makefile=Makefile-cxx
    has_ext c && makefile=Makefile-c
    has_ext pas && makefile=Makefile-pascal
    has_ext java && makefile=Makefile-java
    has_ext ml && makefile=Makefile-caml
    has_ext cs && makefile=Makefile-cs
    has_ext py && makefile=Makefile-python
    has_ext php && makefile=Makefile-php

    lang=${makefile##Makefile-}
    echo "Compiling the champion (detected language: $lang)."
    make -f "$makefile_dir/$makefile" MFPATH="$makefile_dir" all 2>&1 \
        | sed 's/^.*$/    &/'
    res=${PIPESTATUS[0]}
    echo

    if test "$res" -ne 0; then
        echo >&2 "*** Compilation failed. Try again :)"
        echo >&2 "*** If this compiles on your side, contact an organizer"
        exit 1
    fi

    echo "Checking the compilation result:"
    if [ ! -f "champion.so" ]; then
        echo >&2 "*** Champion library not found. This should not happen."
        exit 1
    fi
    ls -gGlah champion.so 2>&1 | sed 's/^.*$/    &/'
    echo

    reqs=$(make -f "$makefile_dir/$makefile" MFPATH="$makefile_dir" \
               list-run-reqs)
    echo "Copying the champion files."
    for f in $reqs; do
        cp -v "$f" "$champion_dir/$f"
    done 2>&1 | sed 's/^.*$/    &/'
    echo

    echo 'Success!'
) 2>&1 | tee "$compil_log"

res=${PIPESTATUS[0]}
rm -rf "$compil_dir"
exit $res
