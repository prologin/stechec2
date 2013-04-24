# -*- encoding: utf-8 -*-

from stechec import forms
from concours.stechec import models
from django.conf import settings
from django.core.urlresolvers import reverse
from django.db.models import Max, Min
from django.http import HttpResponseRedirect, HttpResponse
from django.shortcuts import render_to_response, get_object_or_404
from django.template import RequestContext
from django.views.generic import DetailView, ListView

import os
import os.path

class ChampionView(DetailView):
    context_object_name = "champion"
    model = models.Champion
    template_name = "champion-detail.html"

    @property
    def can_see_log(self):
        ch = self.get_object()
        return self.request.user == ch.author or self.request.user.is_staff

    def get_context_data(self, **kwargs):
        context = super(ChampionView, self).get_context_data(**kwargs)
        context['can_see_log'] = self.can_see_log
        return context

class ChampionsListView(ListView):
    context_object_name = "champions"
    paginate_by = 50
    template_name = "champions-list.html"

    def get_context_data(self, **kwargs):
        context = super(ChampionsListView, self).get_context_data(**kwargs)
        context['show_for_all'] = self.show_for_all
        context['explanation_text'] = self.explanation_text
        return context

class AllChampionsView(ChampionsListView):
    queryset = models.Champion.objects.filter(deleted=False)
    explanation_text = 'Voici la liste de tous les champions participants actuellement.'
    show_for_all = True

class MyChampionsView(ChampionsListView):
    explanation_text = 'Voici la liste de tous vos champions participants actuellement.'
    show_for_all = False

    def get_queryset(self):
        user = self.request.user
        return models.Champion.objects.filter(deleted=False, author=user)

class MatchesListView(ListView):
    context_object_name = "matches"
    paginate_by = 100
    template_name = "matches-list.html"

    def get_context_data(self, **kwargs):
        context = super(MatchesListView, self).get_context_data(**kwargs)
        context['explanation_text'] = self.explanation_text
        context['show_creator'] = self.show_creator
        matches = []
        for m in context['matches']:
            try:
                map_id = int(m.map.split('/')[-1])
                map_name = models.Map.objects.get(pk=map_id).name
            except Exception:
                map_name = m.map
            matches.append((m, map_name))
        context['matches'] = matches
        return context

class MatchView(DetailView):
    context_object_name = "match"
    template_name = "match-detail.html"
    queryset = models.Match.objects.annotate(Max('matchplayer__score')).annotate(Min('matchplayer__id'))

class AllMatchesView(MatchesListView):
    queryset = models.Match.objects.all()
    explanation_text = "Voici la liste de tous les matches ayant été réalisés."
    show_creator = True

class MyMatchesView(MatchesListView):
    explanation_text = "Voici la liste des matches que vous avez lancé."
    show_creator = False

    def get_queryset(self):
        user = self.request.user
        return models.Match.objects.filter(author=user)

class AllMapsView(ListView):
    context_object_name = "maps"
    paginate_by = 100
    template_name = "maps-list.html"
    queryset = models.Map.objects.order_by('-official', '-id')

class MapView(DetailView):
    context_object_name = "map"
    template_name = "map-detail.html"
    model = models.Map

def new_champion(request):
    if request.method == 'POST':
        form = forms.ChampionUploadForm(request.POST, request.FILES)
        if form.is_valid():
            champion = models.Champion(
                name = form.cleaned_data['name'],
                author = request.user,
                status = 'new',
                comment = form.cleaned_data['comment']
            )
            champion.save()

            os.makedirs(champion.directory)
            fp = open(os.path.join(champion.directory, 'champion.tgz'), 'w')
            for chunk in form.cleaned_data['tarball'].chunks():
                fp.write(chunk)
            fp.close()
            return HttpResponseRedirect(champion.get_absolute_url())
    else:
        form = forms.ChampionUploadForm()

    return render_to_response('champion-new.html', {'form': form},
                              context_instance=RequestContext(request))

def delete_champion(request, pk):
    ch = get_object_or_404(models.Champion, pk=pk, author=request.user)
    if request.method == 'POST':
        ch.deleted = True
        ch.save()
        return HttpResponseRedirect(reverse("champions-my"))
    return render_to_response('champion-delete.html',
                              context_instance=RequestContext(request))

def new_match(request):
    if request.method == 'POST':
        form = forms.MatchCreationForm(request.POST)
        if form.is_valid():
            match = models.Match(
                author=request.user,
                status='creating',
                tournament=None,
                options=''
            )
            match.map = form.cleaned_data['map'].path
            match.save()

            for i in range(1, settings.STECHEC_NPLAYERS + 1):
                champ = form.cleaned_data['champion_%d' % i]
                player = models.MatchPlayer(
                    champion=champ,
                    match=match
                )
                player.save()

            match.status = 'new'
            match.save()
            return HttpResponseRedirect(match.get_absolute_url())
    else:
        form = forms.MatchCreationForm()

    return render_to_response('match-new.html', {'form': form},
                              context_instance=RequestContext(request))

def match_dump(request, pk):
    match = get_object_or_404(models.Match, pk=pk)
    h = HttpResponse(match.dump, mimetype="application/stechec-dump")
    h['Content-Disposition'] = 'attachment; filename=dump-%s.json.gz' % pk
    return h

def new_map(request):
    if request.method == 'POST':
        form = forms.MapCreationForm(request.POST)
        if form.is_valid():
            map = models.Map(
                author = request.user,
                name = form.cleaned_data['name'],
                official = False
            )
            map.save()
            map.contents = form.cleaned_data['contents']
            return HttpResponseRedirect(map.get_absolute_url())
    else:
        form = forms.MapCreationForm()

    return render_to_response('map-new.html', {'form': form},
                              context_instance=RequestContext(request))

def master_status(request):
    status = models.master_status()
    status = [(h, p, (100 * (m - s)) / m) for (h, p, s, m) in status]
    status.sort()
    return render_to_response('master-status.html', {'status': status},
                              context_instance=RequestContext(request))
