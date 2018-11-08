#ifndef JettyFJUtils_hh
#define JettyFJUtils_hh

#include <fastjet/PseudoJet.hh>
#include <fastjet/ClusterSequence.hh>
#include <fastjet/contrib/SoftDrop.hh>

namespace JettyFJUtils
{
	std::vector<fastjet::PseudoJet> soft_drop_jets(std::vector<fastjet::PseudoJet> jets, double z_cut = 0.1, double beta = 2.0, double Rjet = 1.0);
	std::vector<double> zs(const fastjet::PseudoJet &j);
	double radial_moment(const fastjet::PseudoJet jet);
	std::vector<double> radial_moments(const std::vector<fastjet::PseudoJet> &jets);

	class LundEntries
	{
	public:
		LundEntries(const fastjet::PseudoJet &j);
		virtual ~LundEntries();
		std::vector<double> LogzDeltaR()  {return _logzDeltaR;}
		std::vector<double> Log1oDeltaR() {return _log1oDeltaR;}
		std::vector<double> deltaR() 	  {return _deltaR;}
		std::vector<double> z() 	 	  {return _z;}
		std::vector<double> pt1() 	 	  {return _pt1;}
		std::vector<double> pt2() 	 	  {return _pt2;}
		std::vector<int>    lpdg() 		  {return _lpdg;}
		fastjet::PseudoJet *CAjet() 	  {return _cajet;}
	private:
		std::vector<double>  _logzDeltaR;
		std::vector<double>  _log1oDeltaR;
		std::vector<double>  _e;
		std::vector<double>  _m;
		std::vector<unsigned int> _nc;
		std::vector<double>  _deltaR;
		std::vector<double>  _z;
		std::vector<double>  _pt1;
		std::vector<double>  _pt2;
		std::vector<int> 	 _lpdg;

		fastjet::PseudoJet 	 *_cajet;
		LundEntries();
	};

	class SJInfo : public fastjet::PseudoJet::UserInfoBase
	{
	public:
		SJInfo(const double &sj_r = 0.15, const fastjet::JetAlgorithm sj_algor = fastjet::antikt_algorithm,
		       const double sd_z_cut = 0.1, const double sd_beta = 0.0, const double sd_r_jet = 0.4, // default SoftDrop HI CMS setting
		       fastjet::PseudoJet* j = 0x0);

		SJInfo(fastjet::PseudoJet* j,
		       const double &sj_r = 0.15, const fastjet::JetAlgorithm sj_algor = fastjet::antikt_algorithm,
		       const double sd_z_cut = 0.1, const double sd_beta = 0.0, const double sd_r_jet = 0.4);

		SJInfo(const double sd_z_cut, const double sd_beta, const double sd_r_jet,
		       fastjet::PseudoJet* j = 0x0);

		virtual ~SJInfo();

		void reset_jet(fastjet::PseudoJet *j);

		double sd_dR() {_do_soft_drop(); return _sd_jet.structure_of<fastjet::contrib::SoftDrop>().delta_R();}
		double sd_zg() {_do_soft_drop(); return _sd_jet.structure_of<fastjet::contrib::SoftDrop>().symmetry();}
		double sd_mu() {_do_soft_drop(); return _sd_jet.structure_of<fastjet::contrib::SoftDrop>().mu();}

		std::vector<double> z();
		std::vector<double> c_pt();
		std::vector<double> c_phi();
		std::vector<double> c_eta();

		double rm();
		std::vector<fastjet::PseudoJet> subjets();

		bool has_at_least_2_subjets() {_do_subjets(); if (_sj.size() < 2) return false; return true;}
		double lead_sj_z() {_do_subjets(); if (_sj.size() > 0) return _sj[0].perp()/_jet->perp(); return -1.0;}
		double sub_lead_sj_z() {_do_subjets(); if (_sj.size() > 1) return _sj[1].perp()/_jet->perp(); return -1.0;}

	private:

		void _do_soft_drop();
		void _do_subjets();

		fastjet::PseudoJet*				_jet;

		double 							_sj_r;
		fastjet::JetAlgorithm 			_sj_algor;
		fastjet::JetDefinition* 		_sj_jet_def;
		fastjet::ClusterSequence* 		_sj_cs;
		std::vector<fastjet::PseudoJet> _sj;

		std::vector<double> 			_z;
		std::vector<double> 			_c_pt;
		std::vector<double> 			_c_phi;
		std::vector<double> 			_c_eta;

		double 							_rm;

		double 							_sd_z_cut;
		double 							_sd_beta;
		double 							_sd_Rjet;
		fastjet::contrib::SoftDrop* 	_sd;
		fastjet::PseudoJet 				_sd_jet;

	};
}

#endif // JettyFJUtils_hh
