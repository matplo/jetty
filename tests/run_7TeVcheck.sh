#!/bin/bash

module load jetty/default
jettyExamplesExe --pythia --inel Beams:eCM=7000 --output=7TeV.root --nev=10000
