#!/usr/bin/env python
import os
import sys

def setup_pythia():
	import subprocess
	pythia8libdir = None
	try:
		pythia8libdir = subprocess.check_output(['pythia8-config', "--libdir"]).strip('\n')
	except:
		print '[e] unable to run pythia8-config'
	if pythia8libdir:
		sys.path.insert(0, pythia8libdir)
		print sys.path
	else:
		exit(1)
	return pythia8libdir

setup_pythia()
# Import the Pythia module.
import pythia8
pythia = pythia8.Pythia()
pythia.readString("Beams:eCM = 8000.")
pythia.readString("HardQCD:all = on")
pythia.readString("PhaseSpace:pTHatMin = 20.")
pythia.init()
mult = pythia8.Hist("charged multiplicity", 100, -0.5, 799.5)
# Begin event loop. Generate event. Skip if error. List first one.
for iEvent in range(0, 100):
    if not pythia.next(): continue
    # Find number of all final charged particles and fill histogram.
    nCharged = 0
    for prt in pythia.event:
        if prt.isFinal() and prt.isCharged(): nCharged += 1
    mult.fill(nCharged)
# End of event loop. Statistics. Histogram. Done.
pythia.stat();
print(mult)
