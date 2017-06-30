#!/bin/bash

module load jetty/default
echo $JETTYDIR
for e in 100 # 200 500 1000 1500 2000 3000 5000
do
	jettyExamplesExe --et --minbias --output=et$e.root --nev=1000 Beams:eCM=$e
done
