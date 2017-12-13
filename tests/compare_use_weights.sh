#!/bin/bash

module use $HOME/software/hepsoft/modules
module use $HOME/devel/jetty/modules
module use $HOME/devel/g0mc/modules
module load hepsoft jetty g0mc

jetty_spectra_exe --spectra --hconfig=h_pythia_parts_g0.cfg PhaseSpace:pTHatMin=20. --out=un_weighted.root --hardQCD --nev=1000
jetty_spectra_exe --spectra --hconfig=h_pythia_parts_g0.cfg PhaseSpace:pTHatMin=100. --out=un_weighted_100.root --hardQCD --nev=1000
jetty_spectra_exe --spectra --hconfig=h_pythia_parts_g0.cfg PhaseSpace:pTHatMin=20. --out=weighted.root --hardQCD --nev=10000 PhaseSpace:bias2Selection=on
