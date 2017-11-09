#!/bin/bash

module use /Users/ploskon/devel/jetty/modules /Users/ploskon/software/hepsoft/modules
module load jetty/1.0 hepsoft/default

[ ! -e pythia_gen_hepmc.dat ] && jettyGenPythiaHepMC --minbias --nev=10000

#for etamax in 1.0 2.0 3.0 4.0
#do
#	jetty-voronoi-test --file=pythia_gen_hepmc.dat --etamax=${etamax} --test2 --nev=1000 --nghost-eta=200
#done

#for ecm in 1000 2500 5000 7000
for ecm in 7000
do
	output_file="pythia_gen_hepmc_${ecm}.dat"
	[ ! -e "${output_file}" ] && jettyGenPythiaHepMC --minbias --nev=1000 Beams:eCM=${ecm} --output=${output_file}
	etamax=1.0
	jetty-voronoi-test --file=${output_file} --etamax=${etamax} --test2 --nev=1000 --nghost-eta=0 --output="voronoi_ecm_${ecm}.root"
	jetty-voronoi-test --file=${output_file} --etamax=${etamax} --test2 --nev=1000 --nghost-eta=0 --output="voronoi_ecm_${ecm}.root" --random
	output_file="pythia_gen_hepmc_${ecm}_jets.dat"
	[ ! -e "${output_file}" ] && jettyGenPythiaHepMC --nev=1000 Beams:eCM=${ecm} --output=${output_file} HardQCD:all=on PhaseSpace:pTHatMin=100
	jetty-voronoi-test --file=${output_file} --etamax=${etamax} --test2 --nev=1000 --nghost-eta=0 --output="voronoi_ecm_${ecm}_jets.root"
	jetty-voronoi-test --file=${output_file} --etamax=${etamax} --test2 --nev=1000 --nghost-eta=0 --output="voronoi_ecm_${ecm}_jets.root" --random

	output_file="pythia_gen_hepmc_${ecm}_jets2.dat"
	[ ! -e "${output_file}" ] && jettyGenPythiaHepMC --nev=1000 Beams:eCM=${ecm} --output=${output_file} HardQCD:all=on PhaseSpace:pTHatMin=1000
	jetty-voronoi-test --file=${output_file} --etamax=${etamax} --test2 --nev=1000 --nghost-eta=0 --output="voronoi_ecm_${ecm}_jets2.root"
	jetty-voronoi-test --file=${output_file} --etamax=${etamax} --test2 --nev=1000 --nghost-eta=0 --output="voronoi_ecm_${ecm}_jets2.root" --random
done
