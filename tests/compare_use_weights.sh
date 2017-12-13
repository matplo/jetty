#!/bin/bash

module use $HOME/software/hepsoft/modules
module use $HOME/devel/jetty/modules
module use $HOME/devel/g0mc/modules
module load hepsoft jetty g0mc

jetty_spectra_exe Beams:eCM=5000. --spectra --hconfig=h_pythia_parts_g0.cfg PhaseSpace:pTHatMin=2. --out=un_weighted.root --hardQCD --nev=1000
jetty_spectra_exe Beams:eCM=5000. --spectra --hconfig=h_pythia_parts_g0.cfg PhaseSpace:pTHatMin=20. --out=un_weighted.root --hardQCD --nev=1000
jetty_spectra_exe Beams:eCM=5000. --spectra --hconfig=h_pythia_parts_g0.cfg PhaseSpace:pTHatMin=100. --out=un_weighted.root --hardQCD --nev=1000
jetty_spectra_exe Beams:eCM=5000. --spectra --hconfig=h_pythia_parts_g0.cfg PhaseSpace:pTHatMin=500. --out=un_weighted.root --hardQCD --nev=1000

jetty_spectra_exe Beams:eCM=5000. --spectra --hconfig=h_pythia_parts_g0.cfg PhaseSpace:pTHatMin=2. --out=weighted.root --hardQCD --nev=10000 PhaseSpace:bias2Selection=on
jetty_spectra_exe Beams:eCM=5000. --spectra --hconfig=h_pythia_parts_g0.cfg PhaseSpace:pTHatMin=20. --out=weighted.root --hardQCD --nev=10000 PhaseSpace:bias2Selection=on
jetty_spectra_exe Beams:eCM=5000. --spectra --hconfig=h_pythia_parts_g0.cfg PhaseSpace:pTHatMin=100. --out=weighted.root --hardQCD --nev=10000 PhaseSpace:bias2Selection=on

jetty_spectra_exe Beams:eCM=200. --spectra --hconfig=h_pythia_parts_g0.cfg PhaseSpace:pTHatMin=1.0 --out=un_weighted.root --hardQCD --nev=1000
jetty_spectra_exe Beams:eCM=200. --spectra --hconfig=h_pythia_parts_g0.cfg PhaseSpace:pTHatMin=2.0 --out=un_weighted.root --hardQCD --nev=1000
jetty_spectra_exe Beams:eCM=200. --spectra --hconfig=h_pythia_parts_g0.cfg PhaseSpace:pTHatMin=5.0 --out=un_weighted.root --hardQCD --nev=1000
jetty_spectra_exe Beams:eCM=200. --spectra --hconfig=h_pythia_parts_g0.cfg PhaseSpace:pTHatMin=10.0 --out=un_weighted.root --hardQCD --nev=1000
jetty_spectra_exe Beams:eCM=200. --spectra --hconfig=h_pythia_parts_g0.cfg PhaseSpace:pTHatMin=20.0 --out=un_weighted.root --hardQCD --nev=1000
jetty_spectra_exe Beams:eCM=200. --spectra --hconfig=h_pythia_parts_g0.cfg PhaseSpace:pTHatMin=1.0 --out=weighted.root --hardQCD --nev=10000 PhaseSpace:bias2Selection=on

jetty_spectra_exe Beams:eCM=5000. --spectra --hconfig=h_pythia_parts_g0.cfg --z0 PhaseSpace:pTHatMin=0.1 --out=un_weighted.root --nev=1000
jetty_spectra_exe Beams:eCM=5000. --spectra --hconfig=h_pythia_parts_g0.cfg --z0 PhaseSpace:pTHatMin=0.1 --out=weighted.root --nev=10000 PhaseSpace:bias2Selection=on
jetty_spectra_exe Beams:eCM=5000. --spectra --hconfig=h_pythia_parts_g0.cfg --photons --promptPhoton PhaseSpace:pTHatMin=0.1 --out=weighted.root --nev=10000 PhaseSpace:bias2Selection=on
