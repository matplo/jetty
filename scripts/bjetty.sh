#!/bin/bash

cd $(dirname $BASH_SOURCE)
BT_config=./jetty.cfg
source $HOME/devel/buildtools/bt.sh "$@" --build

function build()
{
	cd ${BT_build_dir}
    cmake -DCMAKE_INSTALL_PREFIX=${BT_install_dir} -DCMAKE_BUILD_TYPE=${BT_release_type} ${BT_src_dir}
    [ ${BT_clean} ] && make clean
    make -j $(n_cores) VERBOSE=$BT_verbose
    make install
}

run_build
