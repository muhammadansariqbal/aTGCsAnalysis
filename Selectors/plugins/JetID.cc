// -*- C++ -*-
//
// Package:    ExoDiBosonResonances/jetID
// Class:      jetID
// 
/**\class jetID jetID.cc ExoDiBosonResonances/jetID/plugins/jetID.cc

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
#include "DataFormats/PatCandidates/interface/Jet.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"


#include "FWCore/ParameterSet/interface/ParameterSet.h"

//
// class declaration
//

class jetID : public edm::EDFilter {
   public:
      explicit jetID(const edm::ParameterSet&);
      ~jetID();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

   private:
      virtual void beginJob() override;
      virtual bool filter(edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;
    
      bool FilterFlag;
      std::string ID;
      edm::EDGetTokenT<edm::View<pat::Jet> > Jets_;
      
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
jetID::jetID(const edm::ParameterSet& iConfig):
   FilterFlag(iConfig.getParameter<bool>( "filter_flag" )  ),
   ID(iConfig.getParameter<std::string>( "ID" )  ),
   Jets_(consumes<edm::View<pat::Jet> > (iConfig.getParameter<edm::InputTag>( "jets_src" ) ) )
   
{
   //now do what ever initialization is needed
      produces<std::vector<pat::Jet>>();

}


jetID::~jetID()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called on each new Event  ------------
bool jetID::filter(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;
   

   Handle<edm::View<pat::Jet> > Jets;
     
    std::unique_ptr<std::vector<pat::Jet> > JetsSelected(new std::vector<pat::Jet>);
   
   iEvent.getByToken(Jets_, Jets);
   
   bool pass = !FilterFlag;    
   for (unsigned int iJet = 0; iJet < Jets -> size(); iJet ++)
   {
      pat::Jet pfjet = Jets -> at(iJet);
      double NHF = pfjet.neutralHadronEnergyFraction(); 
      double NEMF = pfjet.neutralEmEnergyFraction();
      double CHF = pfjet.chargedHadronEnergyFraction();
      double MUF = pfjet.muonEnergyFraction();
      double CEMF = pfjet.chargedEmEnergyFraction();
      double NumConst = pfjet.chargedMultiplicity()+pfjet.neutralMultiplicity();        
      double CHM = pfjet.chargedMultiplicity(); 
      double eta = pfjet.eta();

      bool looseJetID = (NHF<0.99 && NEMF<0.99 && NumConst>1 && MUF<0.8) && ((abs(eta)<=2.4 && CHF>0 && CHM>0 && CEMF<0.99) || abs(eta)>2.4);
      bool tightJetID = (NHF<0.90 && NEMF<0.90 && NumConst>1 && MUF<0.8) && ((abs(eta)<=2.4 && CHF>0 && CHM>0 && CEMF<0.90) || abs(eta)>2.4);
   
      if(ID == "tight" && tightJetID)	
        {
          JetsSelected -> push_back( Jets -> at(iJet));
          pass = true;
        }
      else if (ID == "loose" && looseJetID)
      {
        JetsSelected -> push_back( Jets -> at(iJet)); 
        pass = true;
      } 

      else if (ID != "tight" && ID != "loose")std::cout << "Smth is going wrong ... " << std::endl;
   }
  
   iEvent.put(std::move(JetsSelected));
   
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
jetID::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
jetID::endJob() {
}

// ------------ method called when starting to processes a run  ------------
/*
void
jetID::beginRun(edm::Run const&, edm::EventSetup const&)
{ 
}
*/
 
// ------------ method called when ending the processing of a run  ------------
/*
void
jetID::endRun(edm::Run const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when starting to processes a luminosity block  ------------
/*
void
jetID::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when ending the processing of a luminosity block  ------------
/*
void
jetID::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
jetID::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}
//define this as a plug-in
DEFINE_FWK_MODULE(jetID);
