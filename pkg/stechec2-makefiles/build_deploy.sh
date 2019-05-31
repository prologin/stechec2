#!/bin/bash

set -e
set -x

makepkg -f
all_pkgs=(stechec2-makefiles*.pkg.tar.xz)
latest_pkg=${all_pkgs[-1]}
scp $latest_pkg root@rhfs01:
ssh root@rhfs01 -- pacman --noconfirm -U $latest_pkg --root /export/nfsroot_staging
ssh root@rhfs01 -- /root/sadm/rfs/commit_staging.sh rhfs01 rhfs23 rhfs45 rhfs67 rhfs89
