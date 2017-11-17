#!/bin/bash

BT_name=jetty
BT_version=1.0
[ ! $(host_pdsf) ] && add_prereq_module_paths ~/software/hepsoft/modules
[ $(host_pdsf) ] && add_prereq_module_paths /project/projectdirs/alice/ploskon/software/hepsoft/modules
add_prereq_modules hepsoft
BT_install_dir=${BT_script_dir}/../${BT_name}_${BT_version}
BT_build_type=Release
BT_src_dir=${BT_script_dir}/../src
BT_module_dir=${BT_script_dir}/../modules/${BT_name}
BT_do_preload_modules="no"

BT_disable_download=yes

function build()
{
	cd ${BT_build_dir}
	echo "[i] building sources at ${BT_src_dir}"
    cmake -DCMAKE_INSTALL_PREFIX=${BT_install_dir} -DCMAKE_BUILD_TYPE=${BT_build_type} ${BT_src_dir}
    make -j $(n_cores) VERBOSE=$BT_verbose && make install
    # make -j VERBOSE=$BT_verbose && make install
}
