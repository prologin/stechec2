import jinja2
import os

from .filters import load_library


class Generator:
    def __init__(self, template_namespace, game, out_dir, **kwargs):
        self.game = game
        self.out_dir = out_dir
        template_folder = os.path.join(
            os.path.dirname(__file__), 'templates', template_namespace,
        )
        self.env = jinja2.Environment(
            loader=jinja2.FileSystemLoader(searchpath=template_folder),
            **kwargs
        )
        self.env.globals['stechec2_generated'] = (
            "This file was generated by stechec2-generator. DO NOT EDIT.")
        self.register_filters()

    def template(self, name, out_name=None, **params):
        if out_name is None:
            out_name = name
        tpl = self.env.get_template(name + '.jinja2')
        out = tpl.stream(game=self.game, **params)
        out.dump(os.path.join(self.out_dir, out_name))

    def register_filters(self):
        filter_library = load_library()
        for filter_name, filter in filter_library.items():
            self.env.filters[filter_name] = filter