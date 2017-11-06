#!/bin/bash

module use /Users/ploskon/devel/jetty/modules /Users/ploskon/software/hepsoft/modules
module load jetty/1.0 hepsoft/default

jettyGenPythiaHepMC --minbias --nev=10000

for etamax in 1.0 2.0 3.0 4.0
do
	jetty-voronoi-test --file=pythia_gen_hepmc.dat --etamax=${etamax}
done
