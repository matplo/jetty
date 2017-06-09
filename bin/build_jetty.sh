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

if [ -z $JETTY ]; then
  exit
fi

if [ -d $JETTY ]; then
  cd $JETTY

  if is_arg_set "realclean" ;
  then
    rm -rf $JETTY/include/* $JETTY/lib/*
    for ex in jetty
    do
      rm -rf $JETTY/bin/$ex
    done
  fi

  for pack in src
  do
    bdir=$JETTY/.build/$pack
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
    cmake -DCMAKE_INSTALL_PREFIX=$JETTY $debug $JETTY/$pack
    is_arg_set "clean" && make clean
    is_arg_set "verbose" && verbose="VERBOSE=1"
    make $verbose && make install
  done
fi
cd $savedir
