// -*- C++ -*-
//
// Package:    WW-analysis/METSystematicProducer
// Class:      METSystematicProducer
// 
/**\class METSystematicProducer METSystematicProducer.cc WW-analysis/METSystematicProducer/plugins/METSystematicProducer.cc

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

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/PatCandidates/interface/MET.h"     
#include "DataFormats/Math/interface/LorentzVector.h"

//
// class declaration
//

class METSystematicProducer : public edm::EDProducer {
   public:
      explicit METSystematicProducer(const edm::ParameterSet&);
      ~METSystematicProducer();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

   private:
      virtual void beginJob() override;
      virtual void produce(edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;
      edm::EDGetTokenT<std::vector<pat::MET> > metToken_;
      std::string uncertaintyType;
      //virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
      //virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
      //virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
      //virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
      std::map<std::string, pat::MET::METUncertainty> METUncertaintyMap = {{"JetResUp", pat::MET::JetResUp}, 
                                                                           {"JetResDown", pat::MET::JetResDown},
                                                                           {"JetEnUp", pat::MET::JetEnUp}, 
                                                                           {"JetEnDown", pat::MET::JetEnDown},
                                                                           {"MuonEnUp", pat::MET::MuonEnUp}, 
                                                                           {"MuonEnDown", pat::MET::MuonEnDown},
                                                                           {"ElectronEnUp", pat::MET::ElectronEnUp}, 
                                                                           {"ElectronEnDown", pat::MET::ElectronEnDown},
                                                                           {"TauEnUp", pat::MET::TauEnUp}, 
                                                                           {"TauEnDown", pat::MET::TauEnDown},
                                                                           {"UnclusteredEnUp", pat::MET::UnclusteredEnUp}, 
                                                                           {"UnclusteredEnDown", pat::MET::UnclusteredEnDown},
                                                                          };

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
METSystematicProducer::METSystematicProducer(const edm::ParameterSet& iConfig):
  metToken_(consumes<std::vector<pat::MET>>(iConfig.getParameter<edm::InputTag>("metSrc"))),
  uncertaintyType(iConfig.getParameter<std::string>("uncertaintyType"))
{
  produces<std::vector<pat::MET>>();
}


METSystematicProducer::~METSystematicProducer()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called to produce the data  ------------
void
METSystematicProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;
   //MET
   edm::Handle<std::vector<pat::MET> > metHandle;
   iEvent.getByToken(metToken_, metHandle);

   std::unique_ptr<std::vector<pat::MET>> outCollection(new std::vector<pat::MET> );

   pat::MET METShifted = metHandle->at(0);
   math::XYZTLorentzVector ShiftedP4 = METShifted.p4();
   ShiftedP4.SetPxPyPzE(METShifted.shiftedPx( METUncertaintyMap[uncertaintyType], pat::MET::Type1), METShifted.shiftedPy(  METUncertaintyMap[uncertaintyType], pat::MET::Type1), METShifted.pz(), METShifted.energy());
   METShifted.setP4(ShiftedP4);

   outCollection -> push_back(METShifted);
   iEvent.put(std::move(outCollection));
}

// ------------ method called once each job just before starting event loop  ------------
void 
METSystematicProducer::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
METSystematicProducer::endJob() {
}

// ------------ method called when starting to processes a run  ------------
/*
void
METSystematicProducer::beginRun(edm::Run const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when ending the processing of a run  ------------
/*
void
METSystematicProducer::endRun(edm::Run const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when starting to processes a luminosity block  ------------
/*
void
METSystematicProducer::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when ending the processing of a luminosity block  ------------
/*
void
METSystematicProducer::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
METSystematicProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(METSystematicProducer);
