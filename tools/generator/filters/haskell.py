from functools import partial

from . import register_filter
from .common import generic_comment


haskell_comment = register_filter(
    partial(generic_comment, start='-- '),
    name='haskell_comment',
)
