#!/bin/bash

cd $(dirname $BASH_SOURCE)

[ ! -f ./bt.sh ] && wget https://raw.github.com/matplo/buildtools/master/bt.sh && chmod +x ./bt.sh
[ -f ./bt.sh ] && ./bt.sh BT_script=build.sh $@

