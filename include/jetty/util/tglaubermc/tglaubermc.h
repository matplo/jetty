/*
 $Id: runglauber.C 134 2017-10-20 04:15:00Z loizides $
 -------------------------------------------------------------------------------------
 Latest documentation: https://arxiv.org/abs/1710.07098

 To run the code, you need to have the ROOT (http://root.cern.ch/drupal/)
 environment. On the root prompt, then enter
 root [0] gSystem->Load("libMathMore")
 root [1] .L runglauber_X.Y.C+
 (where X.Y denotes the version number).
 If you do not have libMathMore comment out "#define HAVE_MATHMORE" below.
 See the documentation for more information.
 -------------------------------------------------------------------------------------
 v3.0:
  Major update to include separate profile for protons and neutrons, placement of nucleon dof on lattice,
  as well as reweighted profiles for recentering, see https://arxiv.org/abs/1710.07098
 -------------------------------------------------------------------------------------
 v2.4:
  Minor update to include Xenon and fix of the TGlauberMC::Draw function,
  see https://arxiv.org/abs/1408.2549v4
 -------------------------------------------------------------------------------------
 v2.3:
  Small bugfixes, see https://arxiv.org/abs/1408.2549v3
 -------------------------------------------------------------------------------------
 v2.2:
  Minor update to provide higher harmonic eccentricities up to n=5, and the average
  nucleon--nucleon impact parameter (bNN) in tree output.
 -------------------------------------------------------------------------------------
 v2.1:
  Minor update to include more proton pdfs, see https://arxiv.org/abs/1408.2549v2
 -------------------------------------------------------------------------------------
 v2.0:
  First major update with inclusion of Tritium, Helium-3, and Uranium, as well as the
  treatment of deformed nuclei and Glauber-Gribov fluctuations of the proton in p+A
  collisions, see https://arxiv.org/abs/1408.2549v1
 -------------------------------------------------------------------------------------
 v1.1:
  First public release of the PHOBOS MC Glauber, see https://arxiv.org/abs/0805.4411
 -------------------------------------------------------------------------------------
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>
*/
#define HAVE_MATHMORE
#if !defined(__CINT__) || defined(__MAKECINT__)
#include <Riostream.h>
#include <TBits.h>
#include <TEllipse.h>
#include <TF1.h>
#include <TF2.h>
#include <TFile.h>
#include <TH2.h>
#include <TLine.h>
#include <TMath.h>
#include <TNamed.h>
#include <TNtuple.h>
#include <TObjArray.h>
#include <TRandom.h>
#include <TString.h>
#include <TSystem.h>
#include <TVector3.h>
#include <TRotation.h>
#ifdef HAVE_MATHMORE
 #include <Math/SpecFuncMathMore.h>
#endif
using namespace std;
#endif
#ifndef _runglauber_
#if !defined(__CINT__) || defined(__MAKECINT__)
#define _runglauber_ 3
#endif
#endif
//---------------------------------------------------------------------------------
TF1 *getNNProf(Double_t snn=67.6, Double_t omega=0.4, Double_t G=1);
//---------------------------------------------------------------------------------
void runAndSaveNtuple(const Int_t n,
                      const char *sysA        = "Pb",
                      const char *sysB        = "Pb",
                      const Double_t signn    = 67.6,
                      const Double_t sigwidth = -1,
                      const Double_t mind     = 0.4,
		      const Double_t omega    = -1,
                      const Double_t noded    = -1,
                      const char *fname       = 0);
//---------------------------------------------------------------------------------
void runAndSaveNucleons(const Int_t n,
                        const char *sysA        = "Pb",
                        const char *sysB        = "Pb",
                        const Double_t signn    = 67.6,
                        const Double_t sigwidth = -1,
                        const Double_t mind     = 0.4,
                        const Bool_t verbose    = 0,
                        const char *fname       = 0);
//---------------------------------------------------------------------------------
void runAndSmearNtuple(const Int_t n,
                       const Double_t sigs  = 0.4,
                       const char *sysA     = "p",
                       const char *sysB     = "Pb",
                       const Double_t signn = 67.6,
                       const Double_t mind  = 0.4,
                       const char *fname    = 0);
//---------------------------------------------------------------------------------
class TGlauNucleon : public TObject
{
  protected:
    Double32_t fX;            //Position of nucleon
    Double32_t fY;            //Position of nucleon
    Double32_t fZ;            //Position of nucleon
    Int_t      fType;         //0 = neutron, 1 = proton
    Bool_t     fInNucleusA;   //=1 from nucleus A, =0 from nucleus B
    Int_t      fNColl;        //Number of binary collisions
    Double32_t fEn;           //Energy it has
  public:
    TGlauNucleon() : fX(0), fY(0), fZ(0), fInNucleusA(0), fNColl(0), fEn(0) {}
    virtual   ~TGlauNucleon() {}
    void       Collide()                                  {++fNColl;}
    Double_t   Get2CWeight(Double_t x) const              {return 2.*(0.5*(1-x)+0.5*x*fNColl);}
    Double_t   GetEnergy()             const              {return fEn;}
    Int_t      GetNColl()              const              {return fNColl;}
    Int_t      GetType()               const              {return fType;}
    Double_t   GetX()                  const              {return fX;}
    Double_t   GetY()                  const              {return fY;}
    Double_t   GetZ()                  const              {return fZ;}
    Bool_t     IsNeutron()             const              {return (fType==0);}
    Bool_t     IsInNucleusA()          const              {return fInNucleusA;}
    Bool_t     IsInNucleusB()          const              {return !fInNucleusA;}
    Bool_t     IsProton()              const              {return (fType==1);}
    Bool_t     IsSpectator()           const              {return !fNColl;}
    Bool_t     IsWounded()             const              {return fNColl>0;}
    void       Reset()                                    {fNColl=0;}
    void       RotateXYZ(Double_t phi, Double_t theta);
    void       RotateXYZ_3D(Double_t psiX, Double_t psiY, Double_t psiZ);
    void       SetEnergy(Double_t en)                     {fEn = en;}
    void       SetType(Bool_t b)                          {fType = b;}
    void       SetInNucleusA()                            {fInNucleusA=1;}
    void       SetInNucleusB()                            {fInNucleusA=0;}
    void       SetXYZ(Double_t x, Double_t y, Double_t z) {fX=x; fY=y; fZ=z;}
    ClassDef(TGlauNucleon,4) // TGlauNucleon class
};
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
class TGlauNucleus : public TNamed
{
  private:
    Int_t      fN;                   //Number of nucleons
    Int_t      fZ;                   //Number of protons
    Double_t   fR;                   //Parameters of function
    Double_t   fA;                   //Parameters of function (fA+fZ=fN)
    Double_t   fW;                   //Parameters of function
    Double_t   fR2;                  //Parameters of function (for p and n separately)
    Double_t   fA2;                  //Parameters of function (for p and n separately)
    Double_t   fW2;                  //Parameters of function (for p and n separately)
    Double_t   fBeta2;               //Beta2 (deformed nuclei)
    Double_t   fBeta4;               //Beta4 (deformed nuclei)
    Double_t   fMinDist;             //Minimum separation distance
    Double_t   fNodeDist;            //Average node distance (set to <=0 if you do not want the "crystal lattice")
    Double_t   fSmearing;            //Node smearing (relevant if fNodeDist>0)
    Int_t      fRecenter;            //=1 by default (0=no recentering, 1=recenter all, 2=recenter displacing only one nucleon, 3=recenter by rotation)
    Int_t      fLattice;             //=0 use HCP by default (1=PCS, 2=BCC, 3=FCC)
    Double_t   fSmax;                //Maximum magnitude of cms shift tolerated (99, ie all by default)
    Int_t      fF;                   //Type of radial distribution
    Int_t      fTrials;              //Store trials needed to complete nucleus
    Int_t      fNonSmeared;          //Store number of non-smeared-node nucleons
    TF1*       fFunc1;               //!Probability density function rho(r)
    TF1*       fFunc2;               //!Probability density function rho(r) -> if set 1 is for p, 2 is for n
    TF2*       fFunc3;               //!Probability density function rho(r,theta) for deformed nuclei
    TObjArray* fNucleons;            //!Array of nucleons
    Double_t   fPhiRot;              //!Angle phi for nucleus
    Double_t   fThetaRot;            //!Angle theta for nucleus
    Double_t   fXRot;                //!Angle around X axis for nucleus
    Double_t   fYRot;                //!Angle around Y axis for nucleus
    Double_t   fZRot;                //!Angle around Z axis for nucleus
    Double_t   fHe3Arr[6000][3][3];  //!Array of events, 3 nucleons, 3 coordinates
    Int_t      fHe3Counter;          //!Event counter
    TBits     *fIsUsed;              //!Bits for lattice use
    Double_t   fMaxR;                //!maximum radius (15fm)
    void       Lookup(const char* name);
    Bool_t     TestMinDist(Int_t n, Double_t x, Double_t y, Double_t z) const;
  public:
    TGlauNucleus(const char* iname="Pb", Int_t iN=0, Double_t iR=0, Double_t ia=0, Double_t iw=0, TF1* ifunc=0);
    virtual ~TGlauNucleus();
    using      TObject::Draw;
    void       Draw(Double_t xs, Int_t colp, Int_t cols);
    Double_t   GetA()             const {return fA;}
    TF1*       GetFunc1()         const {return GetFuncP();}
    TF1*       GetFunc2()         const {return GetFuncN();}
    TF2*       GetFunc3()         const {return GetFuncDef();}
    TF1*       GetFuncP()         const {return fFunc1;}
    TF1*       GetFuncN()         const {return fFunc2;}
    TF2*       GetFuncDef()       const {return fFunc3;}
    Double_t   GetMinDist()       const {return fMinDist;}
    Int_t      GetN()             const {return fN;}
    Double_t   GetNodeDist()      const {return fNodeDist;}
    TObjArray *GetNucleons()      const {return fNucleons;}
    Int_t      GetRecenter()      const {return fRecenter;}
    Double_t   GetR()             const {return fR;}
    Double_t   GetPhiRot()        const {return fPhiRot;}
    Double_t   GetThetaRot()      const {return fThetaRot;}
    Int_t      GetTrials()        const {return fTrials;}
    Int_t      GetNonSmeared()    const {return fNonSmeared;}
    Double_t   GetShiftMax()      const {return fSmax;}
    Double_t   GetW()             const {return fW;}
    Double_t   GetXRot()          const {return fXRot;}
    Double_t   GetYRot()          const {return fYRot;}
    Double_t   GetZRot()          const {return fZRot;}
    void       SetA(Double_t ia, Double_t ia2=-1);
    void       SetNodeDist(Double_t nd) {fNodeDist=nd;}
    void       SetLattice(Int_t i)      {fLattice=i;}
    void       SetMinDist(Double_t min) {fMinDist=min;}
    void       SetN(Int_t in)           {fN=in;}
    void       SetR(Double_t ir, Double_t ir2=-1);
    void       SetRecenter(Int_t b)     {fRecenter=b;}
    void       SetShiftMax(Double_t s)  {fSmax=s;}
    void       SetSmearing(Double_t s)  {fSmearing=s;}
    void       SetW(Double_t iw);
    TVector3  &ThrowNucleons(Double_t xshift=0.);
    ClassDef(TGlauNucleus,6) // TGlauNucleus class
};
//---------------------------------------------------------------------------------
class TGlauberMC : public TNamed
{
  public:
    class Event {
      public:
        Float_t Npart;       //Number of wounded (participating) nucleons in current event
        Float_t Ncoll;       //Number of binary collisions in current event
        Float_t Nhard;       //Number of hard collisions in current event (based on fHardFrac)
        Float_t B;           //[0,0,16] Impact parameter (b)
        Float_t BNN;         //[0,0,16] Average NN impact parameter
        Float_t Ncollpp;     //Ncoll pp
        Float_t Ncollpn;     //Ncoll pn
        Float_t Ncollnn;     //Ncoll nn
        Float_t VarX;        //[0,0,16] Variance of x of wounded nucleons
        Float_t VarY;        //[0,0,16] Variance of y of wounded nucleons
        Float_t VarXY;       //[0,0,16] Covariance of x and y of wounded nucleons
        Float_t NpartA;      //Number of wounded (participating) nucleons in Nucleus A
        Float_t NpartB;      //Number of wounded (participating) nucleons in Nucleus B
        Float_t AreaW;       //[0,0,16] area defined by width of participants
        Float_t Psi1;        //[0,0,16] psi1
        Float_t Ecc1;        //[0,0,16] eps1
        Float_t Psi2;        //[0,0,16] psi2
        Float_t Ecc2;        //[0,0,16] eps2
        Float_t Psi3;        //[0,0,16] psi3
        Float_t Ecc3;        //[0,0,16] eps3
        Float_t Psi4;        //[0,0,16] psi4
        Float_t Ecc4;        //[0,0,16] eps4
        Float_t Psi5;        //[0,0,16] psi5
        Float_t Ecc5;        //[0,0,16] eps5
        Float_t AreaA;       //[0,0,16] area defined by "and" of participants
        Float_t AreaO;       //[0,0,16] area defined by "or" of participants
        Float_t X0;          //[0,0,16] production point in x
        Float_t Y0;          //[0,0,16] production point in y
        Float_t Phi0;        //[0,0,16] direction in phi
        Float_t Length;      //[0,0,16] length in phi0
        Float_t MeanX;       //[0,0,16] <x> of wounded nucleons
        Float_t MeanY;       //[0,0,16] <y> of wounded nucleons
        Float_t MeanX2;      //[0,0,16] <x^2> of wounded nucleons
        Float_t MeanY2;      //[0,0,16] <y^2> of wounded nucleons
        Float_t MeanXY;      //[0,0,16] <xy> of wounded nucleons
        Float_t MeanXSystem; //[0,0,16] <x> of all nucleons
        Float_t MeanYSystem; //[0,0,16] <y> of all nucleons
        Float_t MeanXA;      //[0,0,16] <x> of nucleons in nucleus A
        Float_t MeanYA;      //[0,0,16] <y> of nucleons in nucleus A
        Float_t MeanXB;      //[0,0,16] <x> of nucleons in nucleus B
        Float_t MeanYB;      //[0,0,16] <y> of nucleons in nucleus B
        Float_t PhiA;        //[0,0,16] phi angle nucleus A
        Float_t ThetaA;      //[0,0,16] theta angle nucleus B
        Float_t PhiB;        //[0,0,16] phi angle nucleus B
        Float_t ThetaB;      //[0,0,16] theta angle nucleus B
        void    Reset()      {Npart=0;Ncoll=0;Nhard=0;B=0;BNN=0;Ncollpp=0;Ncollpn=0;Ncollnn=0;VarX=0;VarY=0;VarXY=0;NpartA=0;NpartB=0;AreaW=0;
                              Psi1=0;Ecc1=0;Psi2=0;Ecc2=0;Psi3=0;Ecc3=0;Psi4=0;Ecc4=0;Psi5=0;Ecc5=0;
                              AreaA=0;AreaO=0;X0=0;Y0=0;Phi0=0;Length=0;
                              MeanX=0;MeanY=0;MeanX2=0;MeanY2=0;MeanXY=0;MeanXSystem=0;MeanYSystem=0;MeanXA=0;MeanYA=0;MeanXB=0;MeanYB=0;
                              PhiA=0;ThetaA=0;PhiB=0;ThetaB=0;} // order must match that given in vars below
        ClassDef(TGlauberMC::Event, 1)
    };
  protected:
    TGlauNucleus  fANucleus;       //Nucleus A
    TGlauNucleus  fBNucleus;       //Nucleus B
    Double_t      fXSect;          //Nucleon-nucleon cross section
    Double_t      fXSectOmega;     //StdDev of Nucleon-nucleon cross section
    Double_t      fXSectLambda;    //Jacobian from tot to inelastic (Strikman)
    Double_t      fXSectEvent;     //Event value of Nucleon-nucleon cross section
    TObjArray*    fNucleonsA;      //Array of nucleons in nucleus A
    TObjArray*    fNucleonsB;      //Array of nucleons in nucleus B
    TObjArray*    fNucleons;       //Array which joins Nucleus A & B
    Int_t         fAN;             //Number of nucleons in nucleus A
    Int_t         fBN;             //Number of nucleons in nucleus B
    TNtuple*      fNt;             //Ntuple for results (created, but not deleted)
    Int_t         fEvents;         //Number of events with at least one collision
    Int_t         fTotalEvents;    //All events within selected impact parameter range
    Double_t      fBmin;           //Minimum impact parameter to be generated
    Double_t      fBmax;           //Maximum impact parameter to be generated
    Double_t      fHardFrac;       //Fraction of cross section used for Nhard (def=0.65)
    Int_t         fDetail;         //Detail to store (99=all by default)
    Bool_t        fCalcArea;       //If true calculate overlap area via grid (slow, off by default)
    Bool_t        fCalcLength;     //If true calculate path length (slow, off by default)
    Int_t         fMaxNpartFound;  //Largest value of Npart obtained
    Double_t      fPsiN[10];       //Psi N
    Double_t      fEccN[10];       //Ecc N
    Double_t      f2Cx;            //Two-component x
    TF1          *fPTot;           //Cross section distribution
    TF1          *fNNProf;         //NN profile (hard-sphere == 0 by default)
    Event         fEv;             //Glauber event (results of calculation stored in tree)
    Bool_t        fBC[999][999];   //Array to record binary collision
    Bool_t        CalcResults(Double_t bgen);
    Bool_t        CalcEvent(Double_t bgen);
  public:
    TGlauberMC(const char* NA = "Pb", const char* NB = "Pb", Double_t xsect = 42, Double_t xsectsigma=0);
    virtual            ~TGlauberMC() {Reset();}
    Double_t            CalcDens(TF1 &prof, Double_t xval, Double_t yval) const;
    void                Draw(Option_t* option="");
    Double_t            GetB()                 const {return fEv.B;}
    Double_t            GetBNN()               const {return fEv.BNN;}
    Double_t            GetBmin()              const {return fBmin;}
    Double_t            GetBmax()              const {return fBmax;}
    Double_t            GetEcc(Int_t i=2)      const {return fEccN[i];}
    const Event        &GetEvent()             const {return fEv;}
    Double_t            GetHardFrac()          const {return fHardFrac;}
    Int_t               GetNcoll()             const {return fEv.Ncoll;}
    Int_t               GetNcollpp()           const {return fEv.Ncollpp;}
    Int_t               GetNcollpn()           const {return fEv.Ncollpn;}
    Int_t               GetNcollnn()           const {return fEv.Ncollnn;}
    Int_t               GetNpart()             const {return fEv.Npart;}
    Int_t               GetNpartA()            const {return fEv.NpartA;}
    Int_t               GetNpartB()            const {return fEv.NpartB;}
    Int_t               GetNpartFound()        const {return fMaxNpartFound;}
    TObjArray          *GetNucleons();
    TGlauNucleus*       GetNucleusA()                {return &fANucleus;}
    const TGlauNucleus* GetNucleusA()          const {return &fANucleus;}
    TGlauNucleus*       GetNucleusB()                {return &fBNucleus;}
    const TGlauNucleus* GetNucleusB()          const {return &fBNucleus;}
    TNtuple*            GetNtuple()            const {return fNt;}
    Double_t            GetMeanX()             const {return fEv.MeanX;}
    Double_t            GetMeanY()             const {return fEv.MeanY;}
    Double_t            GetMeanXParts()        const {return fEv.MeanX;}
    Double_t            GetMeanYParts()        const {return fEv.MeanY;}
    Double_t            GetMeanXSystem()       const {return fEv.MeanXSystem;}
    Double_t            GetMeanYSystem()       const {return fEv.MeanYSystem;}
    Double_t            GetPsi(Int_t i=2)      const {return fPsiN[i];}
    Double_t            GetSx2()               const {return fEv.VarX;}
    Double_t            GetSy2()               const {return fEv.VarY;}
    Double_t            GetSxy()               const {return fEv.VarXY;}
    Double_t            GetTotXSect()          const;
    Double_t            GetTotXSectErr()       const;
    TF1*                GetXSectDist()         const {return fPTot;}
    Double_t            GetXSectEvent()        const {return fXSectEvent;}
    Bool_t              IsBC(Int_t i, Int_t j) const {return fBC[i][j];}
    Bool_t              NextEvent(Double_t bgen=-1);
    void                Reset()                      {delete fNt; fNt=0; }
    void                Run(Int_t nevents,Double_t b=-1);
    void                Set2Cx(Double_t x)           {f2Cx = x;}
    void                SetHardFrac(Double_t f)      {fHardFrac=f;}
    void                SetBmin(Double_t bmin)       {fBmin = bmin;}
    void                SetBmax(Double_t bmax)       {fBmax = bmax;}
    void                SetCalcLength(Bool_t b)      {fCalcLength = b;}
    void                SetCalcArea(Bool_t b)        {fCalcArea = b;}
    void                SetDetail(Int_t d)           {fDetail = d;}
    void                SetLattice(Int_t i)          {fANucleus.SetLattice(i); fBNucleus.SetLattice(i);}
    void                SetMinDistance(Double_t d)   {fANucleus.SetMinDist(d); fBNucleus.SetMinDist(d);}
    void                SetNNProf(TF1 *f1)           {fNNProf = f1;}
    void                SetNodeDistance(Double_t d)  {fANucleus.SetNodeDist(d); fBNucleus.SetNodeDist(d);}
    void                SetRecenter(Int_t b)         {fANucleus.SetRecenter(b); fBNucleus.SetRecenter(b);}
    void                SetSmearing(Double_t s)      {fANucleus.SetSmearing(s); fBNucleus.SetSmearing(s);}
    void                SetShiftMax(Double_t s)      {fANucleus.SetShiftMax(s); fBNucleus.SetShiftMax(s);}
    const char         *Str()                  const {return Form("gmc-%s%s-snn%.1f-md%.1f-nd%.1f-rc%d-smax%.1f",fANucleus.GetName(),fBNucleus.GetName(),fXSect,fBNucleus.GetMinDist(),fBNucleus.GetNodeDist(),fBNucleus.GetRecenter(),fBNucleus.GetShiftMax());}
    static void         PrintVersion()               {cout << "TGlauberMC " << Version() << endl;}
    static const char  *Version()                    {return "v3.0 (trunk)";}
    ClassDef(TGlauberMC,6) // TGlauberMC class
};
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
