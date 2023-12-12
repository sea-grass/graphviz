#######################################################################
# Copyright (c) 2011 AT&T Intellectual Property
# All rights reserved. This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v1.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v10.html
#
# Contributors: Details at http://www.graphviz.org/
#######################################################################

ARCH:=$(shell uname -m)

# build directories

GV_DIR=../..
PREFIX=@prefix@
DESTDIR=$(abs_srcdir)/Destdir

# build tools

MKDIR=mkdir -p
RM_LA=find $(GV_DIR) -name "*.la" -delete

PKGBUILD=pkgbuild
PRODUCTBUILD=productbuild
XCODEBUILD=xcodebuild

#
# Graphviz.app build variables
#

GV_APP=Graphviz.app
GV_APP_BIN=Release/$(GV_APP)/Contents/MacOS/Graphviz
GV_PKG=graphviz-$(ARCH).pkg

#
# targets
#

all: $(GV_DIR)/$(GV_PKG)
	@echo
	@echo - Graphviz installer package created: $(GV_PKG)
	@echo - To install locally on this macOS $(ARCH) host:
	@echo open $(GV_PKG)
	@echo
	@echo - The installer package is not signed. Therefore,
	@echo - installation on other hosts may require that the
	@echo - quarantine extended attribute be removed, e.g.
	@echo xattr -d com.apple.quarantine $(GV_PKG)
	@echo

.PHONY: clean
clean:
	rm -f $(GV_DIR)/$(GV_PKG) dot.pkg app.pkg
	rm -rf $(DESTDIR) Release Scripts EagerLinkingTBDs SharedPrecompiledHeaders XCBuildData graphviz.build

distclean: clean
	rm -f Makefile Makefile.in Distribution.xml

#
# Graphviz App installer package
#
# Distribution.xml specifies a minimum deployment target for macOS. This should
# match MACOSX_DEPLOYMENT_TARGET in macosx/graphviz.xcodeproj/project.pbxproj.
#

$(GV_DIR)/$(GV_PKG): Distribution.xml dot.pkg app.pkg
	@echo
	@echo BUILDING GRAPHVIZ INSTALLER...
	@echo
	$(PRODUCTBUILD) --resources $(GV_DIR) --distribution $< $@

app.pkg: $(GV_APP_BIN)
	@echo
	@echo PACKAGING GRAPHVIZ APP...
	@echo
	$(PKGBUILD) --root Release/$(GV_APP) --install-location /Applications/$(GV_APP) --identifier com.att.graphviz.app $@

$(GV_APP_BIN): ../*.m ../*.h
	@echo
	@echo BUILDING GRAPHVIZ APP...
	@echo
	$(XCODEBUILD) -project ../graphviz.xcodeproj -configuration Release ARCHS=$(ARCH)

dot.pkg: $(DESTDIR)$(PREFIX)/bin/dot Scripts/postinstall
	@echo
	@echo PACKAGING GRAPHVIZ DOT...
	@echo
	if [[ "$(PREFIX)" != "/usr/local" ]]; then\
 $(MKDIR) $(DESTDIR)/etc/paths.d;\
 echo "$(PREFIX)/bin" >$(DESTDIR)/etc/paths.d/graphviz; fi
	$(PKGBUILD) --root $(DESTDIR) --install-location / --identifier com.att.graphviz.dot --scripts Scripts $@

Scripts/postinstall:
	@echo
	@echo SCRIPTING POSTINSTALL...
	@echo
	$(MKDIR) $(@D)
	echo "#!/bin/sh" >$@
	echo "logger -is -t \"Graphviz Install\" \"register dot plugins\"" >>$@
	echo "$(PREFIX)/bin/dot -c" >>$@
	chmod 755 $@

#
# install graphviz into a staging destination directory for packaging.
#

$(DESTDIR)$(PREFIX)/bin/dot: $(GV_DIR)/Makefile
	@echo
	@echo BUILDING GRAPHVIZ...
	@echo
	$(MAKE) DESTDIR=$(DESTDIR) -C $(GV_DIR) install
	$(RM_LA)

