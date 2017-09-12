#!/bin/bash

counter=0

module load jetty

jettySpectraExe --spectra --config=${G0MCDIR}/results/gen_hard_spectra/pythia.cmnd --hconfig=${G0MCDIR}/results/gen_hard_spectra/h_pythia_parts_g0.cfg --nev=10000 \
                Beams:eCM=5000 \
                --z0 WeakZ0:gmZmode=0 --no-high-mHat --out=z0.root
