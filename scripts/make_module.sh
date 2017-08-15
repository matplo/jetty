#!/bin/bash

args=$@
function is_arg_set
{
        for i in $args ; do
            if [[ $i == $1 ]] ; then
                return 0 #this is true
            fi
        done
        return 1 #this is false
}

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

function os_linux()
{
        _system=$(uname -a | cut -f 1 -d " ")
        if [ $_system == "Linux" ]; then
                echo "yes"
        else
                echo
        fi
}

function os_darwin()
{
        _system=$(uname -a | cut -f 1 -d " ")
        if [ $_system == "Darwin" ]; then
                echo "yes"
        else
                echo
        fi
}

function host_pdsf()
{
        _system=$(uname -n | cut -c 1-4)
        if [ $_system == "pdsf" ]; then
                echo "yes"
        else
                echo
        fi
}

function sedi()
{
        [ $(os_darwin) ] && sed -i " " $@
        [ $(os_linux)  ] && sed -i'' $@
}

function n_cores()
{
        local _ncores="1"
        [ $(os_darwin) ] && local _ncores=$(system_profiler SPHardwareDataType | grep "Number of Cores" | cut -f 2 -d ":" | sed 's| ||')
        [ $(os_linux) ] && local _ncores=$(lscpu | grep "CPU(s):" | head -n 1 | cut -f 2 -d ":" | sed 's| ||g')
        #[ ${_ncores} -gt "1" ] && retval=$(_ncores-1)
        echo ${_ncores}
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

if [ -z $JETTYDIR ]; then
	JETTYDIR=$XDIR
fi

copy=0
domake=0
dopatch=0
version=default
while getopts ":v:cmp" opt; do
  case $opt in
    v)
      version=$OPTARG
      ;;
    c)
	  copy=1
      ;;
    m)
	  domake=1
	  ;;
    p)
    dopatch=1
    ;;
    \?)
      echo "[e] Invalid option: -$OPTARG" >&2
      exit 1
      ;;
    :)
      echo "[e] Option -$OPTARG requires an argument." >&2
      exit 1
      ;;
  esac
done

outdir=$JETTYDIR/modules/jetty
modfile=$outdir/$version

if [ $domake == 1 ] && [ $dopatch == 1 ]; then
  echo "[e] flags -m and -p are not be combined"
  exit 1
fi

if [ $dopatch == 1 ]; then
  modfiledot=$outdir/.$version
  if [ -f $modfiledot ]; then
    rm -rfv $modfile
    cp -v $modfiledot $modfile
  else
    echo "[e] template to patch does not exist: $modfiledot"
    exit 1
  fi
  if [ -f $modfile ]; then
    sedi "s|JDIR2PATCH|$JETTYDIR|g" $modfile
    echo "[i] patched $modfile"
  else
 echo "[e] nothing patched. $modfile does not exist."
  fi
fi

if [ $copy == 1 ] && [ $domake == 0 ]; then
  if [ -f $modfile ]; then
    mkdir -p $HOME/privatemodules/jetty
    cp -v $modfile $HOME/privatemodules/jetty/
    exit 0
  else
    echo "[e] no file copied. $modfile does not exist."
  fi
  exit 0
fi

[ $dopatch == 1 ] && exit 0

if [ $domake == 0 ]; then
	echo "[i] this is a dry run use -m [-v <version>] for actual modfile creation"
	modfile=$outdir/${version}_test
fi

if [ $copy == 0 ] && [ $domake == 0 ]; then
  echo "[i] this is a dry run use -c [-v <version>] for copying existing files to your $HOME/privatemodules"
  echo
fi

echo "[i] working with "$modfile

mkdir -p $outdir
cp $XDIR/config/module_template $modfile
sedi "s|par1|$JETTYDIR|g" $modfile
sedi "s|par2|$version|g" $modfile
# sedi "s/par1/${JETTYDIR}/" $modfile
# sedi "s/par2/${version}/" $modfile

echo "if { [ module-info mode load ] } {" >> $modfile
mpaths=`module -t avail 2>&1 | grep : | sed "s|:||g"`
for mp in $mpaths
do
	echo "module use $mp" >> $modfile
done

# grep -v jetty is to set a minimal prevention from recursive loads...
loaded=`module -t list 2>&1 | grep -v Current | grep -v jetty | grep -v use.own`
for m in $loaded
do
	#echo "prereq $m" >> $modfile
	echo "module load $m" >> $modfile
done
echo "}" >> $modfile

if [ $domake == 0 ]; then
	cat $modfile
	rm $modfile
fi

if [ $domake == 1 ]; then
	ls -ltr $modfile
	if [ $copy == 1 ]; then
		mkdir -p $HOME/privatemodules/jetty
		cp -v $modfile $HOME/privatemodules/jetty/
	fi
fi
