#!/bin/sh

# $1 = .la file in build tree (doesn't need to be installed)
# $2 = Name of extension
# $3 = Version of extension

lib=`sed -n "/dlname/s/^[^']*'\([^ ']*\).*$/\1/p" $1`
if [ -z "$lib" ]
then
    libBaseName=`basename $1 .la`
    case `uname` in
        CYGWIN*) lib="${libBaseName}.dll"   ;;
        Darwin*) lib="${libBaseName}.dylib" ;;
        HP-UX*)  lib="${libBaseName}.sl"    ;;
        *)       lib="${libBaseName}.so"    ;;
    esac
fi

# for non-release versions, convert the '~dev.' portion into something compliant
# with TCL’s version number rules
version=$(echo "$3" | sed 's/~dev\./b/g')

echo "package ifneeded $2 ${version} \"" >pkgIndex.tcl
case "$1" in
  *tk* )
    echo "	package require Tk 8.3" >>pkgIndex.tcl
    ;;
esac
echo "	load [file join \$dir $lib] $2\"" >>pkgIndex.tcl
