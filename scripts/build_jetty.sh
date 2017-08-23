#!/bin/bash

cd $(dirname $BASH_SOURCE)

#[ ! -f ./bt.sh ] && wget https://raw.github.com/matplo/buildtools/master/bt.sh && chmod +x ./bt.sh
# [ ! -f ./bt.sh ] && wget --no-check-certificate https://raw.github.com/matplo/buildtools/master/bt.sh && chmod +x ./bt.sh
if [[ ! -f ./bt.sh ]]; then
	git clone git@github.com:matplo/buildtools.git .bt
	ln -s ./.bt/bt.sh .
fi
# cp -v ~/devel/buildtools/bt.sh .

[ -f ./bt.sh ] && ./bt.sh BT_script=build.sh $@

