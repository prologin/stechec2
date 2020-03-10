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
    start_dir = bld.srcnode.find_dir('games/{}'.format(game))
    bld.install_files('${PREFIX}/share/stechec2/' + game,
                      start_dir.ant_glob('player/**/*'),
                      relative_trick=True, cwd=start_dir)
