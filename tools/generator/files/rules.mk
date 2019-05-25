# -*- Makefile -*-
#
# Common Makefile for C, C++, Ocaml, Haskell and Java.
#
# Copyright (C) 2007 Freebox S.A.
# Copyright (C) 2007 Prologin.
#

# Path of the Makefile, relative to where it was included from.
MFPATH ?= ../includes

# ==============================================================================
# verbose settings
# ==============================================================================

ifdef V
  quiet =
  Q =
else
  quiet = quiet_
  Q = @
endif

ifndef NOCOLORS
  quiet_cmd_cc        = [1;32mcc     $< -> $@[0m
  quiet_cmd_cxx       = [1;32mcxx    $< -> $@[0m
  quiet_cmd_mcs       = [1;34mcs     $^ -> $@[0m
  quiet_cmd_java      = [1;34mjava   $< -> $@[0m
  quiet_cmd_ocaml     = [1;33mcaml   $< -> $@[0m
  quiet_cmd_ocamlo    = [1;33mcaml   $< -> $@[0m
  quiet_cmd_hsc2hs    = [1;32mhsc2hs $< -> $@[0m
  quiet_cmd_ghc       = [1;33mghc    $^ -> $@[0m
  quiet_cmd_rustc     = [1;33mrustc  $^ -> $@[0m
  quiet_cmd_ld_shared = [1;36mlib    $@[0m
  quiet_cmd_clean     = [1;35mclean[0m
  quiet_cmd_distclean = [1;35mdistclean[0m
  quiet_cmd_tar       = [1;35mtar champion.tgz[0m
else
  quiet_cmd_cc        = CC      $@
  quiet_cmd_cxx       = CXX     $@
  quiet_cmd_mcs       = CS      $@
  quiet_cmd_java      = JAVA    $@
  quiet_cmd_ocaml     = OCAML   $@
  quiet_cmd_ocamlo    = OCAML   $@
  quiet_cmd_hsc2hs    = HSC2HS  $@
  quiet_cmd_ghc       = GHC     $@
  quiet_cmd_rustc     = RUSTC   $@
  quiet_cmd_ld_shared = LINK    $@
  quiet_cmd_clean     = CLEAN   objects
  quiet_cmd_distclean = CLEAN   targets
  quiet_cmd_tar       = TAR     champion.tgz
endif

cmd = $(if $($(quiet)cmd_$(1)),@echo '$(if $(quiet),  )$($(quiet)cmd_$(1))';) $(cmd_$(1))
cmd2 = $(if $($(quiet)cmd_$(1)),echo '$(if $(quiet),  )$($(quiet)cmd_$(1))';) $(cmd_$(1))

exists = $(if $(shell test -e $(1) && echo exists),$(1),)

# ==============================================================================
# build environment
# ==============================================================================

CC     = $(CROSS)gcc
CXX    = $(CROSS)g++
mcs    = MONO_SHARED_DIR=/tmp mcs </dev/null
CPP    = $(CROSS)cpp
JAVAC  = javac
OCAMLC = $(CROSS)ocamlc
GHC    = $(CROSS)ghc
HSC2HS = $(CROSS)hsc2hs
RUSTC  = $(CROSS)rustc
LD     = $(CROSS)ld

OCAML_LIBS     = -L`ocamlc -where` -Wl,-R`ocamlc -where` -lcamlrun_shared -lcurses -lm
OCAML_CFLAGS   = -O2 -I`ocamlc -where`

HASKELL_CFLAGS = -O2 -I`$(GHC) --print-libdir`/include

LANG_FILE      = _lang
DIST_FILE      = champion.tgz

ifeq ($(STECHEC_LANG),haskell)
	LINK_CMD = ghc
else
	LINK_CMD = ld_shared
endif

# ==============================================================================
# build commands
# ==============================================================================

define get_objs
  $(1)-objs := $$(foreach s,$$(filter %.$(2),$$($(1)-srcs)),$$(s:.$(2)=.o)) $(value $(1)-objs)
endef

define get_ocaml_objs
  $(1)-mlsrcs := $$(filter %.ml,$$($(1)-srcs))
  $(1)-camlobjs := $$(shell python3 -S $(MFPATH)/toposort.py $$($(1)-mlsrcs))
  ifneq ($$($(1)-camlobjs),)
    $(1)-objs := $(1)-caml.o $(value $(1)-objs)
    $(1)-cflags := $$($(1)-cflags) $$(OCAML_CFLAGS)
    $(1)-cxxflags := $$($(1)-cxxflags) $$(OCAML_CFLAGS)
    $(1)-ldflags := $$($(1)-ldflags) $$(OCAML_LIBS)
  endif
  cleanfiles := $$($(1)-camlobjs) $$($(1)-camlobjs:.o=.cmo) $$($(1)-camlobjs:.o=.cmi) $$(cleanfiles)

  $(1)-mlisrcs := $$($(1)-mlsrcs:.ml=.mli)
  $(1)-dists := $$($(1)-dists) $$(foreach mli,$$($(1)-mlisrcs),$$(call exists,$$(mli)))

  $(1)-caml.o: override _CAMLFLAGS = $$($(1)-camlflags)
  $(1)-caml.o: $$($(1)-camlobjs:.o=.cmi) $$($(1)-camlobjs:.o=.cmo)
	$$(call cmd,ocamlo)
endef

define get_haskell_objs
  $(1)-hsc-src := $$(filter %.hsc,$$($(1)-srcs))
  $(1)-hs-src := $$(filter %.hs,$$($(1)-srcs)) $$($(1)-hsc-src:.hsc=.hs)

  ifneq ($$(strip $$($(1)-hs-src)),)
    $(1)-deps := $$($(1)-hs-src)
    $(1)-cxxflags := $$($(1)-cxxflags) $$(HASKELL_CFLAGS)
  endif

  cleanfiles := $$($(1)-hs-src:.hs=.hi) $$($(1)-hs-src:.hs=.o) $(1).so $$($(1)-hsc-src:.hsc=.hs) $$(cleanfiles)
endef

define get_csclass
  src := $$(filter %.cs,$$($(1)-srcs))
  ifneq ($$(src),)
    _targets := $$(_targets) $(1)-prologin.dll

$(1)-prologin.dll: $$(src)
	$$(call cmd,mcs)
	$(Q)$(mcs) -out:$$@ $$($(1)-csflags) $$^
  endif
endef

define get_jclass
  src := $$(wildcard *.class)
  # Before the compilation, targets don't exist yet, so use the list of source
  # files instead. We will get an incomplete list of targets, but these will be
  # enough for compiling purposes.
  ifeq ($$(src),)
    src := $$(filter %.java,$$($(1)-srcs))
    src := $$(src:.java=.class)
  endif
  _targets := $$(_targets) $$(src)
endef

define build_lib
  _obj := $$($(1)-objs)
  _objs := $$(_objs) $$(_obj)

  $(1).so: override _LDFLAGS = $$($(1)-ldflags)
  $(1).so: override _LDLIBS = $$($(1)-ldlibs)
  $(1).so: override _CPPFLAGS = $$($(1)-cppflags)
  $(1).so: override _CFLAGS = $$($(1)-cflags)
  $(1).so: override _CXXFLAGS = $$($(1)-cxxflags)
  $(1).so: override _RUSTCFLAGS = $$($(1)-rustcflags)
  ifdef $(1)-deps
    $(1).so: $$($(1)-deps)
  endif

$(1).so: $$(_obj)
	$$(call cmd,$(LINK_CMD))
endef


c_flags     = $(_CFLAGS) -MD -MP -MF $(@D)/.$(@F).d
cxx_flags   = $(_CXXFLAGS) -std=c++14 -MD -MP -MF $(@D)/.$(@F).d
cpp_flags   = $(_CPPFLAGS)
rustc_flags = $(_RUSTCFLAGS) --edition 2018 -C relocation-model=pic -C panic=abort \
              --crate-type=staticlib --emit link,dep-info=$(@D)/.$(@F).d
cmd_cc      = $(CC) $(c_flags) $(cpp_flags) -fPIC -c $< -o $@
cmd_cxx     = $(CXX) $(cxx_flags) $(cpp_flags) -fPIC -c $< -o $@
cmd_java    = $(JAVAC) $(java_flags) $<
cmd_ocaml   = $(OCAMLC) $(_CAMLFLAGS) -c $< -o $@
cmd_ocamlo  = $(OCAMLC) -output-obj $(_CAMLFLAGS) $(filter %.cmo,$^) -o $@
cmd_hsc2hs  = $(HSC2HS) $< -o $@
cmd_ghc     = $(GHC) -pgml $(CXX) -O9 -dynamic --make -shared -fPIC \
              -L`$(GHC) --print-libdir` -lHSrts-ghc`$(GHC) --numeric-version` \
              $(filter %.hs %.o %.a,$^) -o $@
cmd_rustc   = $(RUSTC) $(rustc_flags) $< -o $@

ld_flags      = $(_LDFLAGS)
cmd_ld_shared = $(CXX) $(ld_flags) $(filter %.o %.a,$^) $(_LDLIBS) -shared -o $@

cmd_clean     = $(RM) $(_cleanfiles)
cmd_distclean = $(RM) $(_dcleanfiles)
cmd_tar       = tar czf champion.tgz $(_dist) $(LANG_FILE)

lib_TARGETS := $(lib_TARGETS)

_targets    := $(foreach l,$(lib_TARGETS),$(l).so)

$(foreach t,$(lib_TARGETS),$(eval $(call get_objs,$(t),c)))
$(foreach t,$(lib_TARGETS),$(eval $(call get_objs,$(t),cc)))
$(foreach t,$(lib_TARGETS),$(eval $(call get_objs,$(t),cpp)))
$(foreach t,$(lib_TARGETS),$(eval $(call get_objs,$(t),rs)))
$(foreach t,$(lib_TARGETS),$(eval $(call get_ocaml_objs,$(t))))
$(foreach t,$(lib_TARGETS),$(eval $(call get_haskell_objs,$(t))))
$(foreach t,$(lib_TARGETS),$(eval $(call get_jclass,$(t))))
$(foreach t,$(lib_TARGETS),$(eval $(call get_csclass,$(t))))

$(foreach t,$(lib_TARGETS),$(eval $(call build_lib,$(t))))

_dist        := $(foreach t,$(lib_TARGETS),$($(t)-dists) $(filter-out ../%,$($(t)-srcs)))
_deps        := $(foreach f,$(_objs),$(dir $(f)).$(notdir $(f)).d)
_cleanfiles  := $(cleanfiles) $(_objs) $(_deps)
_dcleanfiles := $(_targets) champion.tgz *.class
_run_reqs    := $(_targets) $(foreach t,$(lib_TARGETS),$($(t)-dists))

# ==============================================================================
# rules
# ==============================================================================

.PHONY: all clean tar dist dist-upload distclean getmap
.DEFAULT_GOAL :=

all: $(_targets)

clean:
	@for i in $(_cleanfiles); do \
		if test -e "$$i"; then \
			$(call cmd2,clean); \
			break; \
		fi; \
	done

distclean: clean
	@for i in $(_dcleanfiles); do \
		if test -e "$$i"; then \
			$(call cmd2,distclean); \
			break; \
		fi; \
	done

list-run-reqs:
	@echo '$(_run_reqs)'

%.o: %.c
	$(call cmd,cc)

%.o: %.cc
	$(call cmd,cxx)

%.o: %.cpp
	$(call cmd,cxx)

%.o: %.rs
	$(call cmd,rustc)

%.class: %.java
	$(call cmd,java)

%.cmi: %.mli
	$(call cmd,ocaml)

%.cmo: %.ml %.mli %.cmi
	$(call cmd,ocaml)

%.cmi %.cmo: %.ml
	$(call cmd,ocaml)

%.hs: %.hsc
	$(call cmd,hsc2hs)

$(DIST_FILE): $(_dist)
	@echo $(STECHEC_LANG) > $(LANG_FILE)
	$(call cmd,tar)
	@rm $(LANG_FILE)

tar: $(DIST_FILE)

dist: $(DIST_FILE)

dist-upload: $(DIST_FILE)
	@echo "Uploading $^..."
	@/var/prologin/venv/bin/python -m prologin.concours.api champion upload $^

getmap:
	@echo -n "map id: "
	@read id && /var/prologin/venv/bin/python -m prologin.concours.api map get "$${id}"

-include $(_deps)
