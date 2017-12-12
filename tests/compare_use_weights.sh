#!/bin/bash

module use $HOME/software/hepsoft/modules
module use $HOME/devel/jetty/modules
module load hepsoft jetty

jetty_spectra_exe --spectra --hardQCD --nev=1000
jetty_spectra_exe --spectra --hardQCD --nev=1000 PhaseSpace:bias2Selection=on
