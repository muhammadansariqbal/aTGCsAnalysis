// -*- C++ -*-
//
// Package:    WW-analysis/LeptonSystematicsProducer
// Class:      LeptonSystematicsProducer
// 
/**\class LeptonSystematicsProducer LeptonSystematicsProducer.cc WW-analysis/LeptonSystematicsProducer/plugins/LeptonSystematicsProducer.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Ivan Shvetsov
//         Created:  Thu, 15 Oct 2015 07:50:44 GMT
//
//


// system include files
#include <memory>
#include <vector>     

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/PatCandidates/interface/Muon.h"     
#include "DataFormats/Math/interface/LorentzVector.h"
#include "DataFormats/Common/interface/RefToBaseVector.h"
#include "DataFormats/Candidate/interface/LeafCandidate.h" 
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"  
#include "DataFormats/Math/interface/deltaR.h" 
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/EcalDetId/interface/EcalSubdetector.h"

namespace reco {
  typedef edm::Ptr<reco::GsfElectron> GsfElectronPtr;
  typedef edm::Ptr<reco::Muon> MuonPtr;
}

//
// class declaration
//

class LeptonSystematicsProducer : public edm::EDProducer {
   public:
      explicit LeptonSystematicsProducer(const edm::ParameterSet&);
      ~LeptonSystematicsProducer();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

   private:
      virtual void beginJob() override;
      virtual void produce(edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;
      edm::EDGetTokenT<edm::View<reco::Candidate> > leptonsToken;
      edm::EDGetTokenT<edm::View<reco::GenParticle> > genParticlesToken;
      std::string channel;
      std::string variation;
      std::string type;
      

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
LeptonSystematicsProducer::LeptonSystematicsProducer(const edm::ParameterSet& iConfig):
  variation(iConfig.getParameter<std::string>("variation"))
{
   channel = iConfig.getParameter<std::string>("channel");
   if (channel != "mu" && channel != "el") throw cms::Exception("InvalidValue") << "Invalid channel used, use el or mu" << std::endl; 
   type = iConfig.getParameter<std::string>("type");  
   if (type != "scale" && type != "resolution") throw cms::Exception("InvalidValue")  << "Invalid type of uncertainty used, use scale or resolution" << std::endl; 
   leptonsToken = consumes<edm::View<reco::Candidate> >(iConfig.getParameter<edm::InputTag>("leptonSource"));
   if (type == "resolution") genParticlesToken = mayConsume<edm::View<reco::GenParticle> >(iConfig.getParameter<edm::InputTag>("genSource"));
   produces<std::vector<reco::LeafCandidate>>();
  
  
}


LeptonSystematicsProducer::~LeptonSystematicsProducer()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called to produce the data  ------------
void
LeptonSystematicsProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;
   
   edm::Handle<edm::View<reco::Candidate> > leptons;
   
 
   iEvent.getByToken(leptonsToken, leptons);
   
   
   std::unique_ptr<std::vector<reco::LeafCandidate>> outCollection(new std::vector<reco::LeafCandidate> );

   reco::LeafCandidate lepton;
   lepton = (reco::LeafCandidate)leptons->at(0);

   auto leptonPtr = leptons -> ptrAt(0);
   reco::MuonPtr asmuonPtr(leptonPtr);
   reco::GsfElectronPtr aselectronPtr(leptonPtr);

    math::XYZTLorentzVector SmearedP4;   

//lepton scale uncertainties
     if (type == "scale") {
      
       //set the lepton uncertainty
       double LeptonUncertainty =0.;
       if (channel == "el") {
        LeptonUncertainty = 0.014;//Zprime AN 2015 222 v7, page 19, figure 12 http://cms.cern.ch/iCMS/jsp/db_notes/noteInfo.jsp?cmsnoteid=CMS%20AN-2015/222

        if (variation == "up") SmearedP4.SetPxPyPzE((1 + LeptonUncertainty)*(lepton.px()),(1 + LeptonUncertainty)*(lepton.py()), (1 + LeptonUncertainty)*lepton.pz()  ,(1 + LeptonUncertainty)*(lepton.energy()) ) ;
        else if (variation == "down") SmearedP4.SetPxPyPzE((1 - LeptonUncertainty)*(lepton.px()),(1 - LeptonUncertainty)*(lepton.py()), (1 - LeptonUncertainty)*lepton.pz()  ,(1 - LeptonUncertainty)*(lepton.energy()) ) ;
        else throw cms::Exception("InvalidValue") << "Invalid variation used in systematics, use up or down.";
      }
       else if (channel == "mu")  {
        LeptonUncertainty = 0.002;// 13 TeV uncertainty not yet available, use recommendation from Run I: if pt of muon < 200, uncertainty is 0.2%, else the uncertainty on the 1/pT bias is about 0.05 c/TeV : https://twiki.cern.ch/twiki/bin/view/CMS/MuonReferenceResolution#General_recommendations_for_muon
        if (asmuonPtr -> tunePMuonBestTrack() -> pt() > 200. ) {
               double extraEnUncertainty = ((asmuonPtr -> tunePMuonBestTrack() -> pt())/1000.)*0.05;
               LeptonUncertainty = sqrt(pow(LeptonUncertainty,2) + pow(extraEnUncertainty,2));
        }

        double muon_energy = sqrt((asmuonPtr -> tunePMuonBestTrack() -> pt())*(asmuonPtr -> tunePMuonBestTrack() -> pt()) + (asmuonPtr -> tunePMuonBestTrack() -> pz())*(asmuonPtr -> tunePMuonBestTrack() -> pz()) + lepton.mass() * lepton.mass());
        if (variation == "up") SmearedP4.SetPxPyPzE((1 + LeptonUncertainty)*(asmuonPtr -> tunePMuonBestTrack() -> px()),(1 + LeptonUncertainty)*(asmuonPtr -> tunePMuonBestTrack() -> py()), (1 + LeptonUncertainty)*(asmuonPtr -> tunePMuonBestTrack() -> pz())  ,(1 + LeptonUncertainty)*muon_energy ) ;
        else if (variation == "down") SmearedP4.SetPxPyPzE((1 - LeptonUncertainty)*(asmuonPtr -> tunePMuonBestTrack() -> px()),(1 - LeptonUncertainty)*(asmuonPtr -> tunePMuonBestTrack() -> py()), (1 - LeptonUncertainty)*(asmuonPtr -> tunePMuonBestTrack() -> pz())  ,(1 - LeptonUncertainty)*muon_energy ) ;
        else throw cms::Exception("InvalidValue")  << "Invalid variation used in systematics, use up or down.";
       }
       else throw cms::Exception("InvalidValue") << "Invalid channel used, use el or mu" << std::endl;
       
       lepton.setP4(SmearedP4);
    }

// lepton resolution uncertainties
    else if (type == "resolution") {
      edm::Handle<edm::View<reco::GenParticle> > genParticles;
      iEvent.getByToken(genParticlesToken, genParticles);
      bool isEB = false;
      double pt_gen = -1., pz_gen = -1. ;
      double pt = 0., pz = 0.;
      double pt_reco = 0., pz_reco = 0.;
      
     

      if (channel == "mu") {
        pt = asmuonPtr -> tunePMuonBestTrack() -> pt();
        pz = asmuonPtr -> tunePMuonBestTrack() -> pz();
        pt_reco = asmuonPtr -> tunePMuonBestTrack() -> pt();
        pz_reco = asmuonPtr -> tunePMuonBestTrack() -> pz();

        //do  matching 
        for (unsigned iGen = 0; iGen < genParticles -> size(); iGen ++){
        if ( deltaR(genParticles -> at(iGen), lepton) < 0.3 && lepton.pdgId() == genParticles -> at(iGen).pdgId() ) {
         pt_gen = genParticles -> at(iGen).pt();
         pz_gen = genParticles -> at(iGen).pz();
         }  
        }

        double MuonResUncertainty = 0.006; // https://twiki.cern.ch/twiki/bin/view/CMS/MuonReferenceResolution#General_recommendations_for_muon
        if (pt_gen > 0.  && variation == "up") {
          pt = pt_gen + (1. + MuonResUncertainty)*(pt_reco - pt_gen);
          pz = pz_gen + (1. + MuonResUncertainty)*(pz_reco - pz_gen);
        }
        else if (pt_gen > 0. && variation == "down") {
          pt = pt_gen + (1. - MuonResUncertainty)*(pt_reco - pt_gen);
          pz = pz_gen + (1. - MuonResUncertainty)*(pz_reco - pz_gen);
        }
        else if (pt_gen ==  -1. && pz_gen == -1. && ( variation == "up" || variation == "down" ) ) {
          pt = pt_reco;
          pz = pz_reco;
        }
        else throw cms::Exception("InvalidValue") << "Smth is wrong,  you have specied the wrong type of variation, probably ... Exit ..." << std::endl;

        SmearedP4.SetPx(pt * cos(asmuonPtr -> tunePMuonBestTrack()->phi()));
        SmearedP4.SetPy(pt * sin(asmuonPtr -> tunePMuonBestTrack()->phi()));
        SmearedP4.SetPz(pz);
        SmearedP4.SetE(sqrt(pt*pt + pz*pz + (lepton.mass()) * (lepton.mass()) ) );

       }
      else  if (channel == "el") {
        pt = aselectronPtr -> pt();
        pz = aselectronPtr -> pz();
        pt_reco = aselectronPtr -> pt();
        pz_reco = aselectronPtr -> pz();


        //do  matching 
        for (unsigned iGen = 0; iGen < genParticles -> size(); iGen ++){
        if ( deltaR(genParticles -> at(iGen), lepton) < 0.3 && lepton.pdgId() == genParticles -> at(iGen).pdgId() ) {
         pt_gen = genParticles -> at(iGen).pt();
         pz_gen = genParticles -> at(iGen).pz();
         }  
        }

        const reco::CaloClusterPtr& seed = aselectronPtr -> superCluster()->seed();
        isEB = ( seed->seed().subdetId() == EcalBarrel );
        double ElectronResUncertainty = (isEB ? 0.0116 : 0.0161 );//Zprime AN 2015 222 v7, page 15, table 8 http://cms.cern.ch/iCMS/jsp/db_notes/noteInfo.jsp?cmsnoteid=CMS%20AN-2015/222

         if (pt_gen > 0.  && variation == "up") {
          pt = pt_gen + (1. + ElectronResUncertainty)*(pt_reco - pt_gen);
          pz = pz_gen + (1. + ElectronResUncertainty)*(pz_reco - pz_gen);
        }
        else if (pt_gen > 0. && variation == "down") {
          pt = pt_gen + (1. - ElectronResUncertainty)*(pt_reco - pt_gen);
          pz = pz_gen + (1. - ElectronResUncertainty)*(pz_reco - pz_gen);
        }
        else if (pt_gen ==  -1. && pz_gen == -1. && ( variation == "up" || variation == "down" ) ) {
          pt = pt_reco;
          pz = pz_reco;
        }
        else throw cms::Exception("InvalidValue")  << "Invalid type of variation, use up or down" << std::endl;

       SmearedP4.SetPx(pt * cos(lepton.phi()));
       SmearedP4.SetPy(pt * sin(lepton.phi()));
       SmearedP4.SetPz(pz);
       SmearedP4.SetE(sqrt(pt*pt + pz*pz + (lepton.mass()) * (lepton.mass()) ) );
      }
      else throw cms::Exception("InvalidValue")  << "Invalid channel used. Please use mu or el." << std::endl;

     lepton.setP4(SmearedP4);

   }
    else throw cms::Exception("InvalidValue") <<  "Invalid type of uncertainty used, use scale or resolution" << std::endl;
   outCollection -> push_back(lepton);
  
   iEvent.put(std::move(outCollection));
     
}

// ------------ method called once each job just before starting event loop  ------------
void 
LeptonSystematicsProducer::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
LeptonSystematicsProducer::endJob() {
}

// ------------ method called when starting to processes a run  ------------
/*
void
LeptonSystematicsProducer::beginRun(edm::Run const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when ending the processing of a run  ------------
/*
void
LeptonSystematicsProducer::endRun(edm::Run const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when starting to processes a luminosity block  ------------
/*
void
LeptonSystematicsProducer::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when ending the processing of a luminosity block  ------------
/*
void
LeptonSystematicsProducer::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
LeptonSystematicsProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(LeptonSystematicsProducer);
