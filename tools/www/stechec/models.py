# -*- coding: utf-8 -*-

from django.conf import settings
from django.contrib.auth import models as auth
from django.core.urlresolvers import reverse
from django.db import models

import os.path
import re
import xmlrpc.client

stripper_re = re.compile(r'\033\[.*?m')
def strip_ansi_codes(t):
    return stripper_re.sub('', t)

class Map(models.Model):
    author = models.ForeignKey(auth.User, verbose_name="auteur")
    name = models.CharField("nom", max_length=100)
    official = models.BooleanField("officielle", default=False)
    ts = models.DateTimeField("date", auto_now_add=True)

    @property
    def path(self):
        contest_dir = os.path.join(settings.STECHEC_ROOT, settings.STECHEC_CONTEST)
        maps_dir = os.path.join(contest_dir, "maps")
        return os.path.join(maps_dir, str(self.id))

    @property
    def contents(self):
        return open(self.path).read()

    @contents.setter
    def contents(self, value):
        open(self.path, 'w').write(value)

    def get_absolute_url(self):
        return reverse("map-detail", kwargs={"pk": self.id})

    def __unicode__(self):
        return "%s, de %s%s" % (self.name, self.author,
                                 " (officielle)" if self.official else "")

    class Meta:
        ordering = ["-official", "-ts"]
        verbose_name = "map"
        verbose_name_plural = "maps"

class Champion(models.Model):
    STATUS_CHOICES = (
        ('new', 'En attente de compilation'),
        ('pending', 'En cours de compilation'),
        ('ready', 'Compilé et prêt'),
        ('error', 'Erreur de compilation'),
    )

    name = models.CharField("nom", max_length=100)
    author = models.ForeignKey(auth.User, verbose_name="auteur")
    status = models.CharField("statut", choices=STATUS_CHOICES,
                              max_length=100, default="new")
    deleted = models.BooleanField("supprimé", default=False)
    comment = models.TextField("commentaire")
    ts = models.DateTimeField("date", auto_now_add=True)

    @property
    def directory(self):
        contest_dir = os.path.join(settings.STECHEC_ROOT, settings.STECHEC_CONTEST)
        champions_dir = os.path.join(contest_dir, "champions")
        return os.path.join(champions_dir, self.author.username, str(self.id))

    @property
    def compilation_log(self):
        this_dir = self.directory
        try:
            return open(os.path.join(this_dir, "compilation.log")).read().decode('iso-8859-15')
        except Exception as e:
            return str(e)

    def get_absolute_url(self):
        return reverse('champion-detail', kwargs={'pk': self.id})

    def get_delete_url(self):
        return reverse('champion-delete', kwargs={'pk': self.id})

    def __unicode__(self):
        return "%s, de %s" % (self.name, self.author)

    class Meta:
        ordering = ['-ts']
        verbose_name = "champion"
        verbose_name_plural = "champions"

class Tournament(models.Model):
    name = models.CharField("nom", max_length=100)
    ts = models.DateTimeField("date", auto_now_add=True)
    players = models.ManyToManyField(Champion, verbose_name="participants",
                                     through="TournamentPlayer")
    maps = models.ManyToManyField(Map, verbose_name="maps",
                                     through="TournamentMap")

    def __unicode__(self):
        return "%s, %s" % (self.name, self.ts)

    class Meta:
        ordering = ['-ts']
        verbose_name = "tournoi"
        verbose_name_plural = "tournois"

class TournamentPlayer(models.Model):
    champion = models.ForeignKey(Champion, verbose_name="champion")
    tournament = models.ForeignKey(Tournament, verbose_name="tournoi")
    score = models.IntegerField("score", default=0)

    def __unicode__(self):
        return "%s pour tournoi %s" % (self.champion, self.tournament)

    class Meta:
        ordering = ["-tournament", "-score"]
        verbose_name = "participant à un tournoi"
        verbose_name_plural = "participants à un tournoi"

class TournamentMap(models.Model):
    map = models.ForeignKey(Map, verbose_name="map")
    tournament = models.ForeignKey(Tournament, verbose_name="tournoi")

    def __unicode__(self):
        return "%s pour tournoi %s" % (self.map, self.tournament)

    class Meta:
        ordering = ["-tournament"]
        verbose_name = "map utilisée dans un tournoi"
        verbose_name_plural = "maps utilisées dans un tournoi"

class Match(models.Model):
    STATUS_CHOICES = (
        ('creating', 'En cours de création'),
        ('new', 'En attente de lancement'),
        ('pending', 'En cours de calcul'),
        ('done', 'Terminé'),
    )

    author = models.ForeignKey(auth.User, verbose_name="lancé par")
    status = models.CharField("statut", choices=STATUS_CHOICES, max_length=100,
                              default="creating")
    tournament = models.ForeignKey(Tournament, verbose_name="tournoi",
                                   null=True, blank=True)
    players = models.ManyToManyField(Champion, verbose_name="participants",
                                     through="MatchPlayer")
    options = models.CharField("options", max_length=500)
    ts = models.DateTimeField("date", auto_now_add=True)

    @property
    def directory(self):
        contest_dir = os.path.join(settings.STECHEC_ROOT, settings.STECHEC_CONTEST)
        matches_dir = os.path.join(contest_dir, "matches")
        hi_id = "%03d" % (self.id / 1000)
        low_id = "%03d" % (self.id % 1000)
        return os.path.join(matches_dir, hi_id, low_id)

    @property
    def log(self):
        log_path = os.path.join(self.directory, "server.log")
        try:
            t = open(log_path).read().decode('iso-8859-15')
            return strip_ansi_codes(t)
        except Exception as e:
            return str(e)

    @property
    def dump(self):
        dump_path = os.path.join(self.directory, "dump.json.gz")
        return open(dump_path, "rb").read()

    @property
    def options_dict(self):
        opts = {}
        for line in self.options.split('\n'):
            if '=' not in line:
                continue
            name, value = line.split('=', 1)
            opts[name.strip()] = value.strip()
        return opts

    @options_dict.setter
    def options_dict(self, value):
        self.options = '\n'.join('%s=%s' % t for t in value.items())

    @property
    def map(self):
        return self.options_dict.get('map', '')

    @map.setter
    def map(self, value):
        d = self.options_dict
        d['map'] = value
        self.options_dict = d

    @property
    def is_done(self):
        return self.status == 'done'

    def get_absolute_url(self):
        return reverse('match-detail', kwargs={'pk': self.id})

    def __unicode__(self):
        return "%s (par %s)" % (self.ts, self.author)

    class Meta:
        ordering = ["-ts"]
        verbose_name = "match"
        verbose_name_plural = "matches"

class MatchPlayer(models.Model):
    champion = models.ForeignKey(Champion, verbose_name="champion")
    match = models.ForeignKey(Match, verbose_name="match")
    score = models.IntegerField("score", default=0)

    @property
    def log(self):
        filename = "log-champ-%d-%d.log" % (self.id, self.champion.id)
        try:
            t = open(os.path.join(self.match.directory, filename)).read().decode('iso-8859-15')
            return strip_ansi_codes(t)
        except Exception as e:
            return str(e)

    def __unicode__(self):
        return "%s pour match %s" % (self.champion, self.match)

    class Meta:
        ordering = ["-match"]
        verbose_name = "participant à un match"
        verbose_name_plural = "participants à un match"

def master_status():
    rpc = xmlrpc.client.ServerProxy(settings.STECHEC_MASTER)
    return rpc.status()
