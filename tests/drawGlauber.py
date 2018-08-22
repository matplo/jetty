#!/usr/bin/env python

import os
import argparse
import tutils
import ROOT as t
import dlist
import random
import math
import tqdm

def load_libs():
	os.environ['LD_LIBRARY_PATH'] = os.path.expandvars('$BOOSTDIR/lib:$ROOTDIR/lib')
	sldpaths = ['./', '$PWD', '/usr/lib', '$ROOTDIR/lib', '$BOOSTDIR/lib', '$JETTYDIR/lib']
	for sldpath in sldpaths:
		s_path = os.path.expandvars(sldpath)
		t.gSystem.AddDynamicPath(s_path)
	# print t.gSystem.GetDynamicPath()
	libs = ['libboost_thread.dylib',
			'libjetty_util',
			'libjetty_util_rstream',
			'libjetty_util_pythia',
			'libjetty_util_tglaubermc']
	for slib in libs:
		tutils.load_lib(slib, False)


def drawGlauber():
	# just fooling around with drawing glauber collisions

	_c = 1. # 299792458.0
	_gamma_RHIC = 200. / 2. / (0.998 * _c * _c) + 1.
	_gamma_LHC = 5020. / 2. / (0.998 * _c * _c) + 1.
	_gamma = _gamma_LHC

	_r_n = 0.1

	_b = random.random() * 15.
	_nev = int(random.random() * 100) + 1

	_sigma = 54.
	print 'impact parameter:', _b, 'sigma:', _sigma

	tg = t.TGlauberMC()
	tg.Run(_nev, _b)
	nucleons = tg.GetNucleons()

	nxy_A = []
	nxy_B = []

	for n in nucleons:
		if n.IsInNucleusA():
			nxy_A.append([n.GetX(), n.GetY()])
		if n.IsInNucleusB():
			nxy_B.append([n.GetX(), n.GetY()])

	_sigma_r = _sigma / math.pi / 10.
	_sigma_r_half = _sigma_r / 2.

	collisions = []
	for nA in nxy_A:
		for nB in nxy_B:
			_dx = nA[0] - nB[0]
			_dy = nA[1] - nB[1]
			_d = math.sqrt(_dx*_dx + _dy*_dy)
			if _d < (_sigma_r):
				collisions.append([nA, nB, _d])

	print '[i] number of collisions:', len(collisions)

	grA = dlist.make_graph('grA', nxy_A)
	grB = dlist.make_graph('grB', nxy_B)

	tc = t.TCanvas('drawGlauber', 'drawGlauber', 1000, 500)
	tc.Divide(2, 1)

	hxy_A = t.TH2F('hxy_A', 'hxy_A;x (fm); y (fm)', 100, -20, 20, 100, -20, 20)
	# for i,c in enumerate(collisions):
	pbar = tqdm.tqdm(collisions)
	for c in pbar:
		# print '\r', i, len(collisions), int(100.*(i*1.) / (len(collisions) * 1.)), '        ',
		for ibx in range(1, hxy_A.GetNbinsX()):
			for iby in range(1, hxy_A.GetNbinsY()):
				_x = hxy_A.GetXaxis().GetBinCenter(ibx)
				_y = hxy_A.GetYaxis().GetBinCenter(iby)
				contents = hxy_A.GetBinContent(ibx, iby)
				_dA_dx = _x - c[0][0]
				_dA_dy = _y - c[0][1]
				_dA = math.sqrt(_dA_dx * _dA_dx + _dA_dy * _dA_dy)
				_dB_dx = _x - c[1][0]
				_dB_dy = _y - c[1][1]
				_dB = math.sqrt(_dB_dx * _dB_dx + _dB_dy * _dB_dy)
				if _dA < (_sigma_r_half) and _dB < (_sigma_r_half):
					hxy_A.SetBinContent(ibx, iby, contents + 1)

	tc.cd(1)
	hxy_A.Draw('colz')
	t.gPad.SetLogz()

	tc.cd(2)
	hxy_A.Draw('colz')
	t.gPad.SetLogz()
	grA.SetMarkerSize(2.2)
	grA.SetMarkerStyle(20)
	grA.SetMarkerColorAlpha(2, 0.3)
	grA.Draw('same P')

	grB.SetMarkerSize(2.2)
	grB.SetMarkerStyle(20)
	grB.SetMarkerColorAlpha(4, 0.3)
	grB.Draw('same P')

	tc.Update()
	tutils.gList.append(hxy_A)
	tutils.gList.append(grA)
	tutils.gList.append(grB)
	tutils.gList.append(tc)


if __name__ == '__main__':
	tutils.setup_basic_root(False)

	parser = argparse.ArgumentParser(description='another starter not much more...', prog=os.path.basename(__file__))
	parser.add_argument('-b', '--batch', help='batchmode - do not end with IPython prompt', action='store_true')
	parser.add_argument('-i', '--prompt', help='end with IPython prompt', action='store_true')
	args = parser.parse_args()

	load_libs()
	drawGlauber()

	if not args.batch:
		tutils.run_app()
	if args.prompt:
		tutils.run_app()
