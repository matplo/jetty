# following pythia8 examples main69.cc
# and http://home.thep.lu.se/~torbjorn/talks/HeleniusPoeticB18.pdf

# jetty_spectra_exe --spectra Beams:idA=11 Beams:idB=2212 Init:showChangedSettings=on PDF:lepton2gamma=on Photon:Q2max=1.0 Photon:Wmin=10.0 PhaseSpace:pTHatMin=2.0 Photon:ProcessType=0 Beams:eA=20 Beams:eB=250 Beams:frameType=2 --hardQCD SpaceShower:dipoleRecoil --nev=1000

eic_base_settings="Beams:idA=11 Beams:idB=2212 Init:showChangedSettings=on PDF:lepton2gamma=on Photon:Q2max=1.0 Photon:Wmin=10.0 Photon:ProcessType=0 Beams:eA=20 Beams:eB=250 Beams:frameType=2 SpaceShower:dipoleRecoil --hardQCD"
hard_bias="PhaseSpace:bias2Selection=on PhaseSpace:bias2SelectionPow=3. PhaseSpace:bias2SelectionRef=2."

# jetty_spectra_exe --spectra ${eic_base_settings} ${hard_bias} --nev=10000
jetty_eic_exe --eic ${eic_base_settings} ${hard_bias} --nev=10000

