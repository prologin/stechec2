import pathlib
import tempfile
from contextlib import contextmanager

from generator.game import load_game
from generator.player import make_player


TEST_GAMES_DIR = pathlib.Path(__file__).parent / 'games'


def load_test_game(game_name):
    with (TEST_GAMES_DIR / (game_name + '.yml')).open() as f:
        return load_game(f)


@contextmanager
def generate_player(game):
    with tempfile.TemporaryDirectory(prefix='stechec2-generator-test-') as td:
        make_player(game, pathlib.Path(td))
        yield pathlib.Path(td)
