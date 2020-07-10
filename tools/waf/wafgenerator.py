#!/usr/bin/env python3

import os


def generator_player_install(bld, game):
    bld.env.TOOLSDIR = os.path.join(bld.top_dir, 'tools')
    bld(
        rule=('PYTHONPATH=${TOOLSDIR} '
              'python3 -m generator player ${SRC} ${TGT}'),
        source='{}.yml'.format(game),
        target='player',
        always=True,
    )
    bld.install_files(
        '${PREFIX}/share/stechec2/' + game,
        bld.bldnode.ant_glob('games/{}/player/**/*'.format(game)),
        relative_trick=True,
        cwd=bld.bldnode.find_dir('games/' + game),
    )
