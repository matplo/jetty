#!/bin/bash

BT_name=jetty
BT_version=1.0
BT_module_paths="/devel/hepsoft/modules ~/Software/hepsoft/modules"
BT_modules="boost cmake root fastjet"
BT_install_dir=~/software/${BT_name}/${BT_version}
BT_build_type=Release
BT_src_dir=${BT_script_dir}/../src
BT_module_dir=~/software/${BT_name}/modules/${BT_name}

function download()
{
	separator "download"
	echo "[i] no dowload installing from the source directly..."
}

function build()
{
	cd ${BT_build_dir}
	echo "[i] building sources at ${BT_src_dir}"
    cmake -DCMAKE_INSTALL_PREFIX=${BT_install_dir} -DCMAKE_BUILD_TYPE=${BT_build_type} ${BT_src_dir}
    [ ${BT_clean} ] && make clean
    make -j $(n_cores) VERBOSE=$BT_verbose
    make install
}
