#!/bin/bash

module load jetty/default
echo $JETTYDIR
for e in 10 30 50 80 100 200 500 1000 1500 2000 2500 3000 3500 4000 4500 5000 6500 8000 9500
do
	#jettyExamplesExe --et --minbias --output=et$e.root --nev=1000 Beams:eCM=$e
	jettyExamplesExe --et --inel --output=et$e.root --nev=1000 Beams:eCM=$e
done

rm et_sum.root
hadd -f et_sum.root et*.root
root -l show.C -q
