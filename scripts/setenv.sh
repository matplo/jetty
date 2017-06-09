#!/bin/bash

root_mod=`module -t avail 2>&1 | grep root | tail -n 1`
[ ! -z $root_mod ] && module load $root_mod
pythia_mod=`module -t avail 2>&1 | grep pythia | tail -n 1`
[ ! -z $pythia_mod ] && module load $pythia_mod
fastjet_mod=`module -t avail 2>&1 | grep fastjet | tail -n 1`
[ ! -z $fastjet_mod ] && module load $fastjet_mod

module list

function abspath()
{
  case "${1}" in
    [./]*)
    echo "$(cd ${1%/*}; pwd)/${1##*/}"
    ;;
    *)
    echo "${PWD}/${1}"
    ;;
  esac
}

savedir=$PWD

THISFILE=`abspath $BASH_SOURCE`
XDIR=`dirname $THISFILE`
if [ -L ${THISFILE} ];
then
    target=`readlink $THISFILE`
    XDIR=`dirname $target`
fi

THISDIR=$XDIR
XDIR=`dirname $XDIR`

export JETTY=$XDIR

if [ -z $PATH ]; then
	export PATH=$JETTY/bin
else
	export PATH=$JETTY/bin:$PATH
fi
