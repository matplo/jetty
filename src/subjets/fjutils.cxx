#include "fjutils.h"
#include <fastjet/contrib/SoftDrop.hh>

namespace fj = fastjet;

namespace JettyFJUtils
{
	std::vector<fj::PseudoJet> soft_drop_jets(std::vector<fj::PseudoJet> jets, double z_cut, double beta, double Rjet)
	{
		// http://fastjet.hepforge.org/svn/contrib/contribs/RecursiveTools/tags/1.0.0/example_softdrop.cc
		// give the soft drop groomer a short name
		// Use a symmetry cut z > z_cut R^beta
		// By default, there is no mass-drop requirement
		fj::contrib::SoftDrop sd(beta, z_cut, Rjet);
		// cout << "SoftDrop groomer is: " << sd.description() << endl;

		std::vector<fj::PseudoJet> soft_dropped;
		for (unsigned ijet = 0; ijet < jets.size(); ijet++)
		{
				// Run SoftDrop and examine the output
				fj::PseudoJet sd_jet   = sd(jets[ijet]);
				// cout << endl;
				// cout << "original    jet: " << jets[ijet] << endl;
				// cout << "SoftDropped jet: " << sd_jet << endl;

				assert(sd_jet      != 0); //because soft drop is a groomer (not a tagger), it should always return a soft-dropped jet

				// cout << "  delta_R between subjets: " << sd_jet.structure_of<fj::contrib::SoftDrop>().delta_R() << endl;
				// cout << "  symmetry measure(z):     " << sd_jet.structure_of<fj::contrib::SoftDrop>().symmetry() << endl;
				// cout << "  mass drop(mu):           " << sd_jet.structure_of<fj::contrib::SoftDrop>().mu() << endl;

				sd_jet.set_user_index(ijet);
				soft_dropped.push_back(sd_jet);
		}
		return soft_dropped;
	}

	std::vector<double> zs(const fastjet::PseudoJet &j)
	{
		std::vector<double> zs;
		if (j.has_constituents())
		{
			for (auto c : fj::sorted_by_pt(j.constituents()))
			{
				double z = c.perp() / j.perp();
				zs.push_back(z);
			}
		}
		return zs;
	}

	double radial_moment(const fastjet::PseudoJet jet)
	{
		// http://jets.physics.harvard.edu/qvg/
		double rm_g = 0;
		std::vector<fastjet::PseudoJet> jc = jet.constituents();
		for (unsigned int ic = 0; ic < jc.size(); ic++)
		{
			rm_g += jc[ic].perp() / jet.perp() * jc[ic].delta_R(jet);
		}
		return rm_g;
	}

	std::vector<double> radial_moments(const std::vector<fastjet::PseudoJet> jets)
	{
		std::vector<double> vgs;
		for (unsigned ijet = 0; ijet < jets.size(); ijet++)
		{
			double rm_g = radial_moment(jets[ijet]);
			vgs.push_back(rm_g);
		}
		return vgs;
	}

	SJInfo::SJInfo(const double &sj_r, const fastjet::JetAlgorithm sj_algor,
			       const double sd_z_cut, const double sd_beta, const double sd_r_jet,
			       fj::PseudoJet* j)
		: fastjet::PseudoJet::UserInfoBase()
		, _jet(j)
		, _sj_r(sj_r)
		, _sj_algor(sj_algor)
		, _sj_jet_def(0x0)
		, _sj_cs(0x0)
		, _sd_z_cut(sd_z_cut)
		, _sd_beta(sd_beta)
		, _sd_Rjet(sd_r_jet)
		, _sj()
		, _z()
		, _rm(-1)
		, _sd(0x0)
		, _sd_jet()
	{
		;
	}

	SJInfo::SJInfo(fj::PseudoJet* j,
	               const double &sj_r, const fastjet::JetAlgorithm sj_algor,
			       const double sd_z_cut, const double sd_beta, const double sd_r_jet)
		: fastjet::PseudoJet::UserInfoBase()
		, _jet(j)
		, _sj_r(sj_r)
		, _sj_algor(sj_algor)
		, _sj_jet_def(0x0)
		, _sj_cs(0x0)
		, _sd_z_cut(sd_z_cut)
		, _sd_beta(sd_beta)
		, _sd_Rjet(sd_r_jet)
		, _sj()
		, _z()
		, _rm(-1)
		, _sd(0x0)
		, _sd_jet()
	{
		;
	}

	SJInfo::SJInfo(const double sd_z_cut, const double sd_beta, const double sd_r_jet,
	               fj::PseudoJet* j)
		: fastjet::PseudoJet::UserInfoBase()
		, _jet(j)
		, _sj_r(0.15)
		, _sj_algor(fastjet::antikt_algorithm)
		, _sj_jet_def(0x0)
		, _sj_cs(0x0)
		, _sd_z_cut(sd_z_cut)
		, _sd_beta(sd_beta)
		, _sd_Rjet(sd_r_jet)
		, _sj()
		, _z()
		, _rm(-1)
		, _sd(0x0)
		, _sd_jet()
	{
		;
	}

	SJInfo::~SJInfo()
	{
		reset_jet(0x0);
	}

	void SJInfo::reset_jet(fastjet::PseudoJet* j)
	{
		delete _sj_jet_def;
		_sj_jet_def = 0x0;
		delete _sj_cs;
		_sj_cs = 0x0;

		delete _sd;
		_sd = 0x0;

		_jet = j;

		_rm = -1;
	}

	void SJInfo::_do_soft_drop()
	{
		if (_sd == 0x0 && _jet != 0x0) // do not recalculate
		{
			_sd = new fj::contrib::SoftDrop(_sd_beta, _sd_z_cut, _sd_Rjet);
			const fj::contrib::SoftDrop &sd = *_sd;
			_sd_jet   = sd(*_jet);
			assert(_sd_jet      != 0); //because soft drop is a groomer (not a tagger), it should always return a soft-dropped jet
		}
	}

	std::vector<fj::PseudoJet> SJInfo::subjets()
	{
		_do_subjets();
		return _sj;
	}

	void SJInfo::_do_subjets()
	{
		if (_jet != 0x0)
		{
			if (_jet->has_constituents() && _sj_cs == 0x0) // do not recalculate
			{
				_sj_jet_def = new fj::JetDefinition(_sj_algor, _sj_r);
				_sj_cs      = new fj::ClusterSequence(_jet->constituents(), *_sj_jet_def);
				_sj         = fj::sorted_by_pt(_sj_cs->inclusive_jets());
			}
		}
	}

	double SJInfo::rm()
	{
		if (_rm < 0 && _jet != 0x0)
		{
			if (_jet->has_constituents())
			{
				_rm = 0;
				// http://jets.physics.harvard.edu/qvg/
				std::vector<fastjet::PseudoJet> jc = _jet->constituents();
				for (unsigned int ic = 0; ic < jc.size(); ic++)
				{
					_rm += jc[ic].perp() / _jet->perp() * jc[ic].delta_R(*_jet);
				}
			}
		}
		return _rm;
	}

	std::vector<double> SJInfo::z()
	{
		if (_jet != 0x0)
		{
			if (_jet->has_constituents() && _z.size() < 1)
			{
				for (auto c : fj::sorted_by_pt(_jet->constituents()))
				{
					double z = c.perp() / _jet->perp();
					_z.push_back(z);
				}
			}
		}
		return _z;
	}
}
