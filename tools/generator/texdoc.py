from pathlib import Path

import jinja2
from .generator import Generator


def tex_escape(s):
    """Escape common problematic characters in TeX code like _, &."""
    if isinstance(s, jinja2.Markup):
        return s
    if not isinstance(s, str):
        return s

    # For more, see sphinx.util.texescape.
    tex_replacements = {
        '{': r'\{',
        '}': r'\}',
        '[': r'{[}',
        ']': r'{]}',
        '$': r'\$',
        '%': r'\%',
        '&': r'\&',
        '#': r'\#',
        '_': r'\_',
        '~': r'\textasciitilde{}',
        '^': r'\textasciicircum{}',
        '\\': r'\textbackslash{}',
    }
    s = s.translate(str.maketrans(tex_replacements))
    return jinja2.Markup(s)


def make_texdoc(game, out_dir: Path) -> None:
    """Generate the LaTeX documentation of a game"""

    gen = Generator(
        'texdoc',
        game=game,
        out_dir=out_dir,

        block_start_string='<%',
        block_end_string='%>',
        variable_start_string='<<',
        variable_end_string='>>',
        comment_start_string='<#',
        comment_end_string='#>',
        finalize=tex_escape,

        # Fix for Jinja2 < 2.11.0
        # Autoescape should be disabled since it's only for HTML tags, but it
        # fixes a bad behavior of the finalize callback which is passed
        # static template data in versions < 2.11.0.
        # Removing this tex-escapes the whole template instead of only the
        # expressions in affected versions.
        # https://github.com/pallets/jinja/pull/1080
        # https://stackoverflow.com/questions/8001986
        autoescape=True,
    )

    for tpl in ["apidoc.tex", "useapi.tex"]:
        gen.template(tpl)
