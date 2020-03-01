#!/usr/bin/env python3

import os


def generator_player_install(bld, game):
    project_root = os.path.dirname(str(bld.bldnode))

    bld.env.env = dict(os.environ)
    bld.env.env['PYTHONPATH'] = os.path.join(project_root, 'tools')
    bld(
        rule='python3 -m generator player ${SRC} ${TGT}',
        source='{}.yml'.format(game),
        target='player',
        always=True,
    )
    start_dir = bld.bldnode.find_dir('games/{}'.format(game))
    bld.install_files('${PREFIX}/share/stechec2/' + game,
                      start_dir.ant_glob('player/**/*'),
                      relative_trick=True, cwd=start_dir)
