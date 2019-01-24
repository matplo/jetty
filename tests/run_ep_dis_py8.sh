# following pythia8 examples main69.cc

# jetty_spectra_exe --spectra Beams:idA=11 Beams:idB=2212 Init:showChangedSettings=on PDF:lepton2gamma=on Photon:Q2max=1.0 Photon:Wmin=10.0 PhaseSpace:pTHatMin=2.0 Photon:ProcessType=0 Beams:eA=20 Beams:eB=250 Beams:frameType=2 --hardQCD SpaceShower:dipoleRecoil --nev=1000

mp_eic_base_settings="Beams:idA=11 Beams:idB=2212 Init:showChangedSettings=on PDF:lepton2gamma=on Photon:Q2max=1.0 Photon:Wmin=10.0 Photon:ProcessType=0 Beams:eA=20 Beams:eB=250 Beams:frameType=2 --hardQCD"
hard_bias="PhaseSpace:bias2Selection=on PhaseSpace:bias2SelectionPow=3. PhaseSpace:bias2SelectionRef=2."

# http://home.thep.lu.se/~torbjorn/talks/HeleniusPoeticB18.pdf
# -> https://arxiv.org/abs/1710.00391
mp_space_showers="SpaceShower:dipoleRecoil=on PDF:lepton=off TimeShower:QEDshowerByL=off SpaceShower:pTmaxMatch=2"
# the latter two come from pythia manual http://home.thep.lu.se/Pythia/pythia82html/SpacelikeShowers.html
# check the example: $PYTHIA8DIR/share/Pythia8/examples/main36.cc
# consider additional settings:
# // Set up DIS process within some phase space.
# // Neutral current (with gamma/Z interference).
# pythia.readString("WeakBosonExchange:ff2ff(t:gmZ) = on");
# // Uncomment to allow charged current.
# //pythia.readString("WeakBosonExchange:ff2ff(t:W) = on");
# // Phase-space cut: minimal Q2 of process.
# pythia.settings.parm("PhaseSpace:Q2Min", Q2min);

# jetty_spectra_exe --spectra ${eic_base_settings} ${hard_bias} --nev=10000

eic_base_settings="Beams:idA=11 Beams:idB=2212 Init:showChangedSettings=on Beams:eA=20 Beams:eB=250 Beams:frameType=2"
eic_lowQ2="HardQCD:all=on PDF:lepton2gamma=on Photon:Q2max=1. Photon:Wmin=10. PhaseSpace:pTHatMin=2. PhotonParton:all=on Photon:ProcessType=0" #Main:timesAllowErrors=10000
eic_dis="WeakBosonExchange:ff2ff(t:gmZ)=on PhaseSpace:Q2Min=10 SpaceShower:pTmaxMatch=2 PDF:lepton=off TimeShower:QEDshowerByL=off"


nev=${1}
[ -z "$1" ] && nev=10000
#jetty_eic_exe --eic ${mp_eic_base_settings} ${hard_bias} ${space_showers} --nev=${nev}

# jetty_eic_exe --eic --task-prefix=lowQ2_ ${eic_base_settings} ${eic_lowQ2} --nev=${nev} ${@:2}
jetty_eic_exe --eic --task-prefix=dis_ ${eic_base_settings} ${eic_dis} --nev=${nev} ${@:2}
