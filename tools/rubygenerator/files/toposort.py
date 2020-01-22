#!/usr/bin/env python3
# -*- encoding: utf-8 -*-
# SPDX-License-Identifier: GPL-2.0-or-later

import subprocess
import sys

def fname_to_mod(fname):
    return fname.lower().split('.')[0]

def parse_deps(raw):
    out = {}
    for l in raw:
        if not l:
            continue
        fname, deps = l.split(':')
        if not deps.strip():
            deps = []
        else:
            deps = deps.strip().split(' ')
        mod = fname_to_mod(fname)
        deps = [dep.lower() for dep in deps]
        out[mod] = deps
    return out

def toposort(deps):
    out = []
    def handle(mod, d):
        if mod not in out:
            for dep in d:
                try:
                    d = deps[dep]
                except KeyError:
                    continue
                handle(dep, d)
            out.append(mod)

    for mod, d in deps.items():
        handle(mod, d)

    return out

if __name__ == '__main__':
    files = sys.argv[1:]
    pipe = subprocess.Popen(["ocamldep", "-modules"] + files,
                            stdout=subprocess.PIPE)
    out = pipe.communicate()[0].decode('utf-8')
    if pipe.returncode != 0:
        sys.exit(pipe.returncode)
    raw_deps = out.split('\n')
    deps = parse_deps(raw_deps)
    order = toposort(deps)
    print(' '.join(m + '.o' for m in order))
