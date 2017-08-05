#!/bin/bash

counter=0

module load jetty

for ib in `seq 1 35`
do
	ibhat=$((2+counter*counter))
	ibhatMax=$((2+(counter+1)*(counter+1)))
	echo $ibhat $ibhatMax
	jettyExamplesExe --pythia-wrapper --overwrite --hconfig=h_pythia_parts_g0.cfg --nev=500 --out=g0.root \
		PhaseSpace:pTHatMin=$ibhat PhaseSpace:pTHatMax=$ibhatMax \
		HardQCD:all=on PromptPhoton:all=off \
		Beams:eCM=5000
	counter=$((counter+1))
done
