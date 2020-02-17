import pathlib
import tempfile
from contextlib import contextmanager

from generator.game import Game
from generator.player import make_player


TEST_GAMES_DIR = pathlib.Path(__file__).parent / 'games'


def load_game(game_name):
    with (TEST_GAMES_DIR / (game_name + '.yml')).open() as f:
        return Game(f)


@contextmanager
def generate_player(game_name):
    with tempfile.TemporaryDirectory(prefix='stechec2-generator-test-') as td:
        game = load_game(game_name)
        make_player(game, td)
        yield pathlib.Path(td)
