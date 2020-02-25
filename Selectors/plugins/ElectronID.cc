// -*- C++ -*-
//
// Package:    ExoDiBosonResonances/ElectronID
// Class:      ElectronID
// 
/**\class ElectronID ElectronID.cc ExoDiBosonResonances/ElectronID/plugins/ElectronID.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Ivan Shvetsov
//         Created:  Mon, 17 Nov 2014 13:35:20 GMT
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDFilter.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/PatCandidates/interface/Electron.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"


#include "FWCore/ParameterSet/interface/ParameterSet.h"

//
// class declaration
//

class ElectronID : public edm::EDFilter {
   public:
      explicit ElectronID(const edm::ParameterSet&);
      ~ElectronID();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

   private:
      virtual void beginJob() override;
      virtual bool filter(edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;
      
      edm::EDGetTokenT<edm::ValueMap<bool>  > ValueMaps_;
      bool FilterFlag;
      edm::EDGetTokenT<edm::View<pat::Electron> > Electrons_;
      
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
ElectronID::ElectronID(const edm::ParameterSet& iConfig):
   ValueMaps_(consumes<edm::ValueMap<bool> > (iConfig.getParameter<edm::InputTag>( "ValueMap_src" ) ) ),
   FilterFlag(iConfig.getParameter<bool>( "filter_flag" )  ),
   Electrons_(consumes<edm::View<pat::Electron> > (iConfig.getParameter<edm::InputTag>( "electron_src" ) ) )
   
{
   //now do what ever initialization is needed
      produces<std::vector<pat::Electron>>();

}


ElectronID::~ElectronID()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called on each new Event  ------------
bool ElectronID::filter(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;
   
   Handle <edm::ValueMap <bool> > ValueMaps;
   Handle<edm::View<pat::Electron> > Electrons;
     
    std::unique_ptr<std::vector<pat::Electron> > ElectronsSelected(new std::vector<pat::Electron>);
   
   iEvent.getByToken( ValueMaps_ , ValueMaps);
   iEvent.getByToken(Electrons_, Electrons);
   
   bool pass = !FilterFlag;    
   for (unsigned int iElectron = 0; iElectron < Electrons -> size(); iElectron ++)
   {
      
     if((*ValueMaps)[Electrons -> ptrAt(iElectron)]) 
     { 
	pass = true;
	ElectronsSelected -> push_back( Electrons -> at(iElectron));
	
     }
   }
  
   iEvent.put(std::move(ElectronsSelected));
   
#ifdef THIS_IS_AN_EVENT_EXAMPLE
   Handle<ExampleData> pIn;
   iEvent.getByLabel("example",pIn);
#endif

#ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
   ESHandle<SetupData> pSetup;
   iSetup.get<SetupRecord>().get(pSetup);
#endif
   return pass;
}

// ------------ method called once each job just before starting event loop  ------------
void 
ElectronID::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
ElectronID::endJob() {
}

// ------------ method called when starting to processes a run  ------------
/*
void
ElectronID::beginRun(edm::Run const&, edm::EventSetup const&)
{ 
}
*/
 
// ------------ method called when ending the processing of a run  ------------
/*
void
ElectronID::endRun(edm::Run const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when starting to processes a luminosity block  ------------
/*
void
ElectronID::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when ending the processing of a luminosity block  ------------
/*
void
ElectronID::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
ElectronID::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}
//define this as a plug-in
DEFINE_FWK_MODULE(ElectronID);
