#
# Makefile for a Video Disk Recorder plugin
#
# $Id: Makefile 2.13 2012/12/21 11:36:15 kls Exp $

# The official name of this plugin.
# This name will be used in the '-P...' option of VDR to load the plugin.
# By default the main source file also carries this name.

PLUGIN = vdrboblight

### The version number of this plugin (taken from the main source file):

#VERSION = $(shell grep 'static const char \*VERSION *=' vdrboblight.h | awk '{ print $$6 }' | sed -e 's/[";]//g')
VERSION := $(shell git describe --abbrev=4 --dirty --always)

### The directory environment:

# Use package data if installed...otherwise assume we're under the VDR source directory:
PKGCFG  = $(if $(VDRDIR),$(shell pkg-config --variable=$(1) $(VDRDIR)/vdr.pc),$(shell pkg-config --variable=$(1) vdr || pkg-config --variable=$(1) ../../../vdr.pc))
LIBDIR  = $(call PKGCFG,libdir)
LOCDIR  = $(call PKGCFG,locdir)
PLGCFG  = $(call PKGCFG,plgcfg)
#
TMPDIR ?= /tmp

### The compiler options:

export CFLAGS   = $(call PKGCFG,cflags)
export CXXFLAGS = $(call PKGCFG,cxxflags)

### Allow user defined options to overwrite defaults:

-include $(PLGCFG)

### The version number of VDR's plugin API:

APIVERSION = $(call PKGCFG,apiversion)
ifeq ($(strip $(APIVERSION)),)
APIVERSION = $(shell grep 'define APIVERSION ' $(VDRDIR)/config.h | awk '{ print $$3 }' | sed -e 's/"//g')
NOCONFIG := 1
endif

# backward compatibility with VDR version < 1.7.34
API1733 := $(shell if [ "$(APIVERSION)" \< "1.7.34" ]; then echo true; fi; )

ifdef API1733

VDRSRC = $(VDRDIR)
ifeq ($(strip $(VDRSRC)),)
VDRSRC := ../../..
endif
LIBDIR = $(VDRSRC)/PLUGINS/lib

ifndef NOCONFIG
CXXFLAGS = $(call PKGCFG,cflags)
CXXFLAGS += -g -O3 -Wall -Werror=overloaded-virtual -Wno-parentheses -fPIC
else
-include $(VDRSRC)/Make.global
-include $(VDRSRC)/Make.config
endif

export CXXFLAGS
endif

### The name of the distribution archive:

ARCHIVE = $(PLUGIN)-$(VERSION)
PACKAGE = vdr-$(ARCHIVE)

### The name of the shared object file:

SOFILE = libvdr-$(PLUGIN).so

### Includes and Defines (add further entries here):

ifdef API1733
INCLUDES += -I$(VDRSRC)/include
endif

DEFINES += -DPLUGIN_NAME_I18N='"$(PLUGIN)"'
### Define GITVERSION used in vdrboblight.c
DEFINES += -DGITVERSION='"$(VERSION)"'

### The object files (add further files here):

OBJS = $(patsubst %.c,%.o,$(wildcard *.c))

### The main target:

ifdef API1733
all: install
else
all: $(SOFILE) $(DEVPLUGTARGETS)
endif

### Implicit rules:

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $(DEFINES) $(INCLUDES) -o $@ $<

### Dependencies:

MAKEDEP = $(CXX) -MM -MG
DEPFILE = .dependencies
$(DEPFILE): Makefile
	@$(MAKEDEP) $(CXXFLAGS) $(DEFINES) $(INCLUDES) $(OBJS:%.o=%.c) > $@

-include $(DEPFILE)

### Targets:

$(SOFILE): $(OBJS) $(FFDECSA)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -shared $(OBJS) $(DECSALIB) -o $@


install-lib: $(SOFILE)
	install -D $^ $(DESTDIR)$(LIBDIR)/$^.$(APIVERSION)

install: install-lib $(DEVPLUGINSTALL)

dist: clean
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@mkdir $(TMPDIR)/$(ARCHIVE)
	@cp -a * $(TMPDIR)/$(ARCHIVE)
	@tar czf $(PACKAGE).tgz -C $(TMPDIR) $(ARCHIVE)
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@echo Distribution package created as $(PACKAGE).tgz

clean:
	@-rm -f $(OBJS) $(DEPFILE) *.so *.tgz core* *~ $(PODIR)/*.mo $(PODIR)/*.pot 
	@-rm -f libvdr-$(PLUGIN).so libvdr-$(PLUGIN).so.$(APIVERSION)
