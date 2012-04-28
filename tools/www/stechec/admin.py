from concours.stechec import models as stechec
from django.contrib import admin

admin.site.register(stechec.Map)
admin.site.register(stechec.Champion)
admin.site.register(stechec.Tournament)
admin.site.register(stechec.TournamentPlayer)
admin.site.register(stechec.TournamentMap)
admin.site.register(stechec.Match)
admin.site.register(stechec.MatchPlayer)
