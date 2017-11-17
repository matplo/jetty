#!/bin/bash

jetty_examples_exe --pythia-pool --inel --nev=10 --fixed-energy MultipartonInteractions:allowRescatter=on MultipartonInteractions:rescatterMode=4 $@
