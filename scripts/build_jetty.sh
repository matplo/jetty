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

savedir=$PWD

if [ -z $JETTYDIR ]; then
  echo "[e] JETTYDIR dir not set..."
  exit
fi

if [ -d $JETTYDIR ]; then
  cd $JETTYDIR

  if is_arg_set "realclean" ;
  then
    rm -rf $JETTYDIR/include/* $JETTYDIR/lib/* $JETTYDIR/bin/*
  fi

  for pack in src
  do
    bdir=$JETTYDIR/.build/$pack
    if is_arg_set "realclean" ;
    then
    	rm -rf $bdir
    fi
    mkdir -p $bdir
    cd $bdir
    debug="-DCMAKE_BUILD_TYPE=Release"
    #cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo
    #is_arg_set "o3" && export CXXFLAGS=-O3 # Release does it
    is_arg_set "debug" && debug=-DCMAKE_BUILD_TYPE=Debug
    cmake -DCMAKE_INSTALL_PREFIX=$JETTYDIR $debug $JETTYDIR/$pack
    is_arg_set "clean" && make clean
    is_arg_set "verbose" && verbose="VERBOSE=1"
    make $verbose && make install
  done
fi
cd $savedir
