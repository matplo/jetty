#!/bin/bash

counter=0

module load jetty

ibhat=40
ibhatMax=-1
echo $ibhat $ibhatMax
jettyExamplesExe --pythia-wrapper --overwrite --hconfig=h_pythia_parts_g0.cfg --nev=1 --out=g0.root \
	PhaseSpace:pTHatMin=$ibhat PhaseSpace:pTHatMax=$ibhatMax \
	HardQCD:all=on PromptPhoton:all=off \
	Beams:eCM=5000 $1
