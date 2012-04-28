from django.conf.urls.defaults import patterns, include, url
from django.contrib import admin

import concours.stechec.urls

admin.autodiscover()

urlpatterns = patterns('',
    url(r'^admin/doc/', include('django.contrib.admindocs.urls')),
    url(r'^admin/', include(admin.site.urls)),
    url(r'^', include(concours.stechec.urls)),
)
