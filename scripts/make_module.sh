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
version=default
while getopts ":v:cm" opt; do
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
mkdir -p $outdir
modfile=$outdir/$version

if [ $domake == 0 ]; then
	echo "[i] this is a dry run use -m for actual modfile creation"
	modfile=$outdir/${version}_test
fi
echo "[i] working with "$modfile

cp $XDIR/config/module_template $modfile
sed -i "" -e "s|par1|$JETTYDIR|g" $modfile
sed -i "" -e "s|par2|$version|g" $modfile
# sed -i "" -e "s/par1/${JETTYDIR}/" $modfile
# sed -i "" -e "s/par2/${version}/" $modfile

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
