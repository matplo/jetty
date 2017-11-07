#!/bin/bash

module use /Users/ploskon/devel/jetty/modules /Users/ploskon/software/hepsoft/modules
module load jetty/1.0 hepsoft/default

for nghosts in 10 50 100 200 350 500 750 1000
do
	jetty-voronoi-test --file=pythia_gen_hepmc.dat --etamax=2.0 --test2 --nev=100 --nghost-eta=$nghosts --min-ghost-area=0.01
done
