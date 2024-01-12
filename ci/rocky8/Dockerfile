FROM rockylinux:8

COPY requirements.txt ./requirements.txt

# Development tools
RUN dnf -y --setopt install_weak_deps=false groupinstall 'Development Tools' \
# for dnf config-manager plugin
&& dnf install -y --setopt install_weak_deps=false python3-dnf-plugins-core \
&& dnf install -y \
# Install epel-release rpm (Needed for libglade2-devel. Must be a separate step)
https://dl.fedoraproject.org/pub/epel/epel-release-latest-8.noarch.rpm \
&& dnf config-manager --set-enabled powertools \
# Centos build utilities
&& dnf -y --setopt install_weak_deps=false install \
libtool-ltdl-devel \
libedit-devel \
ksh \
cmake \
# security-hardened alternatives of packages
rocky-release-security \
# Option glut
freeglut-devel \
# Option gts
# Option swig
swig \
# Command smyra
libglade2-devel \
# Command gvedit
qt5-devel \
ncurses-devel \
# Plugin library devil
# Plugin library gd
gd-devel \
# Documentation
ghostscript \
# Plugin library
ghostscript-devel \
# Plugin library gtk
gtk2-devel \
# Plugin library lasi
# Plugin library pangocairo
# Plugin library poppler
poppler-glib-devel \
poppler-qt5 \
# Plugin library rsvg
librsvg2-devel \
# Plugin library webp
# Language extension gv_sharp & gv_ruby
# Language extension gv_go
# Language extension gv_guile
guile-devel \
# Language extension gv_java
java-devel \
# Language extension gv_lua
lua-devel \
# Language extension gv_perl
perl \
perl-ExtUtils-Embed \
# Language extension gv_php
php-devel \
openssl-devel \
libxml2-devel \
# Language extension gv_python3
python3-devel \
# Language extension gv_ruby
ruby-devel \
# Language extension gv_tcl
tcl-devel \
# Requred in redhat/graphviz.spec.fedora.in
libICE-devel \
libSM-devel \
libXaw-devel \
tk-devel \
# Test utilities
python3-pip \
python3-setuptools \
# install Python dependencies
&& python3 -m pip install --user --upgrade pip \
&& python3 -m pip install --user --requirement requirements.txt \
# Clean up
&& dnf clean all && rm -rf /var/cache/dnf
