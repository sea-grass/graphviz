#######################################################################
# Copyright (c) 2011 AT&T Intellectual Property
# All rights reserved. This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v1.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v10.html
#
# Contributors: Details at http://www.graphviz.org/
#######################################################################

ARCH=$(shell uname -m)
PREFIX=/Applications/Graphviz.app/Contents/Resources
DESTDIR=$(realpath .)/build/Destdir

# build tools

MKDIR=mkdir -p
PRODUCTBUILD=productbuild
PKGBUILD=pkgbuild
XCODEBUILD=xcodebuild

#
# Graphviz.app build variables
#

GV_DIR=..
GV_APP=Graphviz.app
GV_PKG=graphviz-$(ARCH).pkg

#
# targets
#

$(GV_DIR)/$(GV_PKG): build/$(GV_PKG)
	cp $< $@
	@echo
	@echo ========================================================
	@echo The macOS installer package built: $(GV_PKG)
	@echo
	@echo To install locally on this macOS host:
	@echo open $(GV_PKG)
	@echo
	@echo To distribute for installation on other hosts, note
	@echo that $(GV_PKG) is not signed, and therefore may
	@echo require removing the quarantine extended attribute:
	@echo xattr -d com.apple.quarantine $(GV_PKG)
	@echo ========================================================
	@echo

.PHONY: clean
clean:
	rm -rf build $(GV_DIR)/$(GV_PKG)

.PHONY: distclean
distclean: clean
	rm -f Distribution.xml Info.plist

#
# Graphviz App installer package
#
# Note: the check function in Distribution.xml specifies the minimum macOS
# version, which should match the Graphviz.app project's deployment target.
#
# Note: the Component.plist file specifies BundleIsRelocatable = false,
# requiring that the bundle be installed in /Applications. Otherwise,
# the macOS installer may find an existing Graphviz.app registered and
# acceptable. If so, the installer WILL NOT install Graphviz.app into
# /Applications, but DOES set the existing app's user/group to root/wheel!!
#

build/$(GV_PKG): Distribution.xml build/app.pkg
	@echo
	@echo BUILDING GRAPHVIZ INSTALLER...
	@echo
	$(PRODUCTBUILD) --package-path build --distribution $< $@

build/app.pkg: Component.plist $(DESTDIR)$(PREFIX)/bin/dot build/Scripts/postinstall
	@echo
	@echo PACKAGING GRAPHVIZ APP...
	@echo
	$(PKGBUILD) --root $(DESTDIR) --scripts build/Scripts --identifier com.att.graphviz --component-plist $< $@

build/Scripts/postinstall:
	@echo
	@echo SCRIPTING POSTINSTALL...
	@echo
	$(MKDIR) $(@D)
	echo "#!/bin/sh" >$@
	echo "logger -is -t \"Graphviz Install\" \"register dot plugins\"" >>$@
	echo "$(PREFIX)/bin/dot -c" >>$@
	if [[ $(PREFIX) != /usr/local ]]; then\
 echo 'echo "$(PREFIX)/bin" >/etc/paths.d/graphviz' >>$@ ; fi
	chmod 755 $@

$(DESTDIR)$(PREFIX)/bin/dot: $(DESTDIR)/Applications/$(GV_APP)/Contents/MacOS/Graphviz
	@echo
	@echo BUILDING GRAPHVIZ...
	@echo
	$(MAKE) -C $(GV_DIR) install DESTDIR=$(DESTDIR)

$(DESTDIR)/Applications/$(GV_APP)/Contents/MacOS/Graphviz: *.m *.h English.lproj/*.xib $(GV_DIR)/cmd/dot/dot
	@echo
	@echo BUILDING GRAPHVIZ APP...
	@echo
	$(XCODEBUILD) -project graphviz.xcodeproj install ARCHS=$(ARCH) DSTROOT=$(DESTDIR)

$(GV_DIR)/cmd/dot/dot: $(GV_DIR)/Makefile
	@echo
	@echo BUILDING GRAPHVIZ...
	@echo
	$(MAKE) -C $(GV_DIR)

$(GV_DIR)/Makefile Distribution.xml: $(GV_DIR)/configure
	@echo
	@echo CONFIGURING GRAPHVIZ...
	@echo
	cd $(GV_DIR) && ./configure --prefix=$(PREFIX)
