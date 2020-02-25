// -*- C++ -*-
//
// Package:    ExoDiBosonResonances/WLeptonicProducer
// Class:      WLeptonicProducer
// 
/**\class WLeptonicProducer WLeptonicProducer.cc ExoDiBosonResonances/WLeptonicProducer/plugins/WLeptonicProducer.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Thiago Tomei Fernandez
//         Created:  Tue, 19 Aug 2014 12:43:03 GMT
//
//


// system include files
#include <memory>
#include <algorithm>
// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/Candidate/interface/ShallowCloneCandidate.h"
#include "DataFormats/Candidate/interface/CompositeCandidate.h"
#include "DataFormats/Candidate/interface/CandidateFwd.h"
#include "DataFormats/Candidate/interface/CompositeCandidateFwd.h"
#include "CommonTools/CandUtils/interface/AddFourMomenta.h"
#include "CommonTools/Utils/interface/StringCutObjectSelector.h"
#include "TMath.h"

//
// class declaration
//

class WLeptonicProducer : public edm::EDProducer {
public:
  explicit WLeptonicProducer(const edm::ParameterSet&);
  ~WLeptonicProducer();
  
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
  
private:
  virtual void beginJob() override;
  virtual void produce(edm::Event&, const edm::EventSetup&) override;
  virtual void endJob() override;
  edm::EDGetTokenT<edm::View<reco::Candidate> > leptonToken_;
  edm::EDGetTokenT<edm::View<reco::Candidate> > metToken_;
  StringCutObjectSelector<reco::CompositeCandidate> select_;
  double MW_;
  math::XYZTLorentzVector getNeutrinoP4(const reco::Candidate& met, const reco::Candidate& lep, int lepType);
  //virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
  //virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
  //virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
  //virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
  
      // ----------member data ---------------------------
};

//
// constants, enums and typedefs
//


//
// static data member definitions
//

//
// constructors and destructor
//
WLeptonicProducer::WLeptonicProducer(const edm::ParameterSet& iConfig):
  leptonToken_ (consumes<edm::View<reco::Candidate> > (iConfig.getParameter<edm::InputTag>( "leptons" ) ) ),
  metToken_    (consumes<edm::View<reco::Candidate> > (iConfig.getParameter<edm::InputTag>( "MET" ) ) ),
  select_ (iConfig.getParameter<std::string>("cut") )
{
  //register your products
  produces<reco::CompositeCandidateCollection>();

  //now do what ever other initialization is needed
  MW_=80.385;
}


WLeptonicProducer::~WLeptonicProducer()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}

bool byPt (reco::CompositeCandidate cand1,reco::CompositeCandidate cand2)
{ 
  return (cand1.pt() > cand2.pt() ); 
  
}

//
// member functions
//

// ------------ method called to produce the data  ------------
void
WLeptonicProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;

   Handle<View<reco::Candidate> > lepton_h;
   Handle<View<reco::Candidate> > MET_h;

   iEvent.getByToken(leptonToken_, lepton_h);
   iEvent.getByToken(metToken_, MET_h);

   /// There is only one MET, so let's get that out of the way
   const reco::Candidate& METcand = MET_h->at(0);
   reco::CandidateBaseRef METBaseRef = MET_h->refAt(0);

   std::unique_ptr<reco::CompositeCandidateCollection> outCollection(new reco::CompositeCandidateCollection);
   


   
   /// Loop on the leptons and combine them with the MET
   for(size_t i = 0; i != lepton_h->size(); ++i) {
     const reco::Candidate& lepton = lepton_h->at(i);
     math::XYZTLorentzVector neutrinoP4 = getNeutrinoP4(METcand, lepton, 1);
     reco::ShallowCloneCandidate neutrino(METBaseRef, 0 , neutrinoP4);
     reco::CompositeCandidate WLeptonic;
     WLeptonic.addDaughter(lepton);
     WLeptonic.addDaughter(neutrino);
     AddFourMomenta addP4;
     addP4.set(WLeptonic);
     bool pass = select_(WLeptonic);
     if(pass) outCollection->push_back(WLeptonic);
   }

   std::sort(outCollection->begin(), outCollection->end(), byPt);
   iEvent.put(std::move(outCollection));
}

// ------------ method called once each job just before starting event loop  ------------
void 
WLeptonicProducer::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
WLeptonicProducer::endJob() {
}

// ------------ method called when starting to processes a run  ------------
/*
void
WLeptonicProducer::beginRun(edm::Run const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when ending the processing of a run  ------------
/*
void
WLeptonicProducer::endRun(edm::Run const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when starting to processes a luminosity block  ------------
/*
void
WLeptonicProducer::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when ending the processing of a luminosity block  ------------
/*
void
WLeptonicProducer::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
WLeptonicProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

math::XYZTLorentzVector
WLeptonicProducer::getNeutrinoP4(const reco::Candidate& met, const reco::Candidate& lep, int lepType){//lepType 0 ele, 1 mu

  double leppt = lep.pt();
  double lepphi = lep.phi();
  double lepeta = lep.eta();
  double lepenergy = lep.energy();
  //std::cout << "Producer quantities: " << leppt << "   " << lepeta << "   " << lepphi << "   " << lepenergy << "   ";

  double metpt = met.pt();
  double metphi = met.phi();

  double  px = metpt*cos(metphi);
  double  py = metpt*sin(metphi);
  double  pz = 0;
  double  pxl= leppt*cos(lepphi);
  double  pyl= leppt*sin(lepphi);
  double  pzl= leppt*sinh(lepeta);
  double  El = lepenergy;
  double  a = pow(MW_,2) + pow(px+pxl,2) + pow(py+pyl,2) - px*px - py*py - El*El + pzl*pzl;
  double  b = 2.*pzl;   
  double  A = b*b -4.*El*El;
  double  B = 2.*a*b;
  double  C = a*a-4.*(px*px+py*py)*El*El;

  ///////////////////////////pz for fnal
  double M_mu =  0; 

  //if(lepType==1)M_mu=0.105658367;//mu
  //if(lepType==0)M_mu=0.00051099891;//electron

  int type=2; // use the small abs real root

  a = MW_*MW_ - M_mu*M_mu + 2.0*pxl*px + 2.0*pyl*py;
  A = 4.0*(El*El - pzl*pzl);
  B = -4.0*a*pzl;
  C = 4.0*El*El*(px*px + py*py) - a*a;


  double tmproot = B*B - 4.0*A*C;
  //std::cout << (-B + sqrt(tmproot))/(2.0*A) << "   " << (-B - sqrt(tmproot))/(2.0*A)   << std::endl;
  
  if (tmproot<0) {
    //std::cout << "Complex root detected, taking real part..." << std::endl;
    pz = - B/(2*A); // take real part of complex roots
  }
  else {

    double tmpsol1 = (-B + sqrt(tmproot))/(2.0*A);
    double tmpsol2 = (-B - sqrt(tmproot))/(2.0*A);

    //std::cout << " Neutrino Solutions: " << tmpsol1 << ", " << tmpsol2 << std::endl;

    if (type == 0 ) {
      // two real roots, pick the one closest to pz of muon
      if (TMath::Abs(tmpsol2-pzl) < TMath::Abs(tmpsol1-pzl)) { pz = tmpsol2; }
      else { pz = tmpsol1; }
      // if pz is > 300 pick the most central root
      if ( abs(pz) > 300. ) {
	if (TMath::Abs(tmpsol1)<TMath::Abs(tmpsol2) ) { pz = tmpsol1; }
	else { pz = tmpsol2; }
      }
    }
    if (type == 1 ) {
      // two real roots, pick the one closest to pz of muon
      if (TMath::Abs(tmpsol2-pzl) < TMath::Abs(tmpsol1-pzl)) { pz = tmpsol2; }
      else {pz = tmpsol1; }
    }
    if (type == 2 ) {
      // pick the most central root.
      if (TMath::Abs(tmpsol1)<TMath::Abs(tmpsol2) ) { pz = tmpsol1; }
      else { pz = tmpsol2; }
    }
    /*if (type == 3 ) {
      // pick the largest value of the cosine
      TVector3 p3w, p3mu;
      p3w.SetXYZ(pxl+px, pyl+py, pzl+ tmpsol1);
      p3mu.SetXYZ(pxl, pyl, pzl );

      double sinthcm1 = 2.*(p3mu.Perp(p3w))/MW_;
      p3w.SetXYZ(pxl+px, pyl+py, pzl+ tmpsol2);
      double sinthcm2 = 2.*(p3mu.Perp(p3w))/MW_;

      double costhcm1 = sqrt(1. - sinthcm1*sinthcm1);
      double costhcm2 = sqrt(1. - sinthcm2*sinthcm2);

      if ( costhcm1 > costhcm2 ) { pz = tmpsol1; otherSol_ = tmpsol2; }
      else { pz = tmpsol2;otherSol_ = tmpsol1; }

      }*///end of type3

  }//endl of if real root

  //dont correct pt neutrino
  math::XYZTLorentzVector outP4(px,py,pz,sqrt(px*px+py*py+pz*pz));
  return outP4;

}//end neutrinoP4

//define this as a plug-in
DEFINE_FWK_MODULE(WLeptonicProducer);
