#!/bin/bash

module load gcc
module load cmake
module load boost
module use /project/projectdirs/alice/software/modulefiles

root_mod=`module -t avail 2>&1 | grep root | tail -n 1`
[ ! -z $root_mod ] && module load $root_mod
pythia_mod=`module -t avail 2>&1 | grep pythia8 | tail -n 1`
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

export JETTYDIR=$XDIR

if [ -z $PATH ]; then
	export PATH=$JETTYDIR/bin:$JETTYDIR/scripts
else
	export PATH=$JETTYDIR/bin:$JETTYDIR/scripts:$PATH
fi

if [ -z $LD_LIBRARY_PATH ]; then
    export LD_LIBRARY_PATH=$JETTYDIR/lib
else
    export LD_LIBRARY_PATH=$JETTYDIR/lib:$LD_LIBRARY_PATH
fi

if [ -z $DYLD_LIBRARY_PATH ]; then
    export DYLD_LIBRARY_PATH=$JETTYDIR/lib
else
    export DYLD_LIBRARY_PATH=$JETTYDIR/lib:$DYLD_LIBRARY_PATH
fi
