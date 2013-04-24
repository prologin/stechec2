# -*- encoding: utf-8 -*-

from concours.stechec import models
from django import forms
from django.conf import settings
from django.forms import widgets
from django.utils.html import escape, conditional_escape
from itertools import chain, groupby

class ChampionUploadForm(forms.Form):
    name = forms.CharField(max_length=64, required=True, label="Nom du champion")
    tarball = forms.FileField(required=True, label="Archive des sources (.tgz)")
    comment = forms.CharField(required=True, widget=forms.widgets.Textarea(), label="Commentaire")

class ChampionField(forms.Field):
    def clean(self, value):
        super(ChampionField, self).clean(value)
        try:
            return models.Champion.objects.get(
                id=int(value.strip()),
                deleted=False,
                status='ready'
            )
        except ValueError:
            raise forms.ValidationError("Numéro de champion invalide.")
        except models.Champion.DoesNotExist:
            raise forms.ValidationError("Champion inexistant")

class MapSelect(widgets.Select):
    def render_options(self, choices, selected_choices):
        def render_option(map):
            title = map.name
            official = map.official
            attrs = (map.id in selected_choices) and ' selected="selected"' or '' 
            if official:
                attrs += ' class="award"'

            return '<option value="%d"%s>%s</option>' % (
                map.id, attrs,
                conditional_escape(title)
            )
        selected_choices = set(v for v in selected_choices)
        output = []
        for author, maps in chain(self.choices, choices):
            output.append('<optgroup label="%s">' % escape(author))
            for map_id, map in maps:
                output.append(render_option(map))
            output.append('</optgroup>')
        return '\n'.join(output)

class MatchCreationForm(forms.Form):
    def __init__(self, *args, **kwargs):
        super(MatchCreationForm, self).__init__(*args, **kwargs)

        self.champions = []
        for i in range(1, settings.STECHEC_NPLAYERS + 1):
            f = ChampionField(label="Champion %d" % i)
            self.fields['champion_%d' % i] = f
            self.champions.append(f)

        self.fields['map'] = forms.ChoiceField(required=True, widget=MapSelect(attrs={'class': 'mapselect'}), label="Map utilisée")
        self.fields['map'].choices = [
            (author, [(map.id, map) for map in maps])
            for author, maps in groupby(
                models.Map.objects.order_by('author__username', 'name'),
                lambda map: map.author
            )
        ]

    def clean_map(self):
        try:
            value = models.Map.objects.get(id=self.cleaned_data['map'])
        except models.Map.DoesNotExist:
            raise ValidationError("Cette carte n'existe pas")
        return value

class MapCreationForm(forms.Form):
    name = forms.CharField(max_length=64, required=True, label="Nom de la map")
    contents = forms.CharField(required=True, widget=forms.widgets.Textarea(), label="Contenu")
