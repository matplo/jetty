#!/bin/bash

cd $(dirname $BASH_SOURCE)

[ ! -f ./bt.sh ] && wget https://raw.github.com/matplo/buildtools/master/bt.sh
[ ! -f ./bt.sh ] && echo "[i] no bt.sh - stop here." && exit 1

export BT_config=./build_jetty.cfg
source ~/devel/buildtools/bt.sh "$@"
#source ./bt.sh "$@"

function build()
{
	cd ${BT_build_dir}
	echo "[i] building sources at ${BT_src_dir}"
    cmake -DCMAKE_INSTALL_PREFIX=${BT_install_dir} -DCMAKE_BUILD_TYPE=${BT_build_type} ${BT_src_dir}
    [ ${BT_clean} ] && make clean
    make -j $(n_cores) VERBOSE=$BT_verbose
    make install
}

exec_build_tool
