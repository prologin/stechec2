masternodedir = $(pkglibdir)/master-node
masternodecfgdir = $(sysconfdir)/stechec

dist_masternode_DATA = \
	__init__.py \
	__main__.py \
	paths.py \
	task.py \
	utils.py \
	worker.py

dist_bin_SCRIPTS = master-node

dist_masternodecfg_DATA = master-node.yml

EXTRA_DIST = master-node.in master-node.yml.in paths.py.in
