// -*- C++ -*-
//
// Package:    WW-analysis/WLepLorentzVectorSystematics
// Class:      WLepLorentzVectorSystematics
// 
/**\class WLepLorentzVectorSystematics WLepLorentzVectorSystematics.cc WW-analysis/WLepLorentzVectorSystematics/plugins/WLepLorentzVectorSystematics.cc

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
#include "DataFormats/Candidate/interface/CompositeCandidate.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "DataFormats/Candidate/interface/LeafCandidate.h"
#include <map>
#include <utility>
#include <string>
#include <vector>


//
// class declaration
//

class SystematicsHelper {
  std::vector< edm::EDGetTokenT<std::vector<reco::CompositeCandidate> > > tokens;
  std::vector< edm::EDGetTokenT<std::vector<pat::MET> > > metTokens;
  edm::EDGetTokenT<edm::View<reco::LeafCandidate>> TokenResUp;
  edm::EDGetTokenT<edm::View<reco::LeafCandidate>> TokenResDown;
  edm::EDGetTokenT<edm::View<reco::LeafCandidate>> TokenEnUp;
  edm::EDGetTokenT<edm::View<reco::LeafCandidate>> TokenEnDown;
	std::vector<std::string> ListOfSystematics;
public:
    SystematicsHelper(){}
    SystematicsHelper(std::string channel, edm::ConsumesCollector iC){
	ListOfSystematics.push_back("UnclusteredEn");
	ListOfSystematics.push_back("JetEn");
  ListOfSystematics.push_back("LeptonEn");
  ListOfSystematics.push_back("LeptonRes");
  ListOfSystematics.push_back("JetRes");
  tokens.resize(2 * ListOfSystematics.size());
  metTokens.resize(2 * ListOfSystematics.size());
  int iSystUp, iSystDown;
  for (unsigned int iSyst = 0; iSyst < ListOfSystematics.size(); iSyst ++) {	
     	iSystUp = 2*iSyst;
     	iSystDown = 2*iSyst + 1;
      if (ListOfSystematics.at(iSyst) != "LeptonEn" ){
        tokens.at(iSystUp)  =  iC.mayConsume<std::vector<reco::CompositeCandidate>>(edm::InputTag("Wto" + channel + "nu" + ListOfSystematics.at(iSyst) + "Up"));
        tokens.at(iSystDown) = iC.mayConsume<std::vector<reco::CompositeCandidate>>(edm::InputTag("Wto" + channel + "nu" + ListOfSystematics.at(iSyst) + "Down"));

        if (ListOfSystematics.at(iSyst) != "LeptonRes") {
          metTokens.at(iSystUp)  =  iC.mayConsume<std::vector<pat::MET>>(edm::InputTag("patPFMetT1" + ListOfSystematics.at(iSyst) + "Up"));
          metTokens.at(iSystDown) = iC.mayConsume<std::vector<pat::MET>>(edm::InputTag("patPFMetT1" + ListOfSystematics.at(iSyst) + "Down"));
        }
      }
      else if (ListOfSystematics.at(iSyst) == "LeptonEn" ){
          std::string name;
          if ( channel == "el") name = "ElectronEn";
          else if ( channel == "mu") name = "MuonEn";
          else std::cerr << "Invalid channel, use el or mu" <<  std::endl;
          tokens.at(iSystUp)  =  iC.mayConsume<std::vector<reco::CompositeCandidate>>(edm::InputTag("Wto" + channel + "nu" + name + "Up"));
          tokens.at(iSystDown) = iC.mayConsume<std::vector<reco::CompositeCandidate>>(edm::InputTag("Wto" + channel + "nu" + name + "Down"));

          metTokens.at(iSystUp)  =  iC.mayConsume<std::vector<pat::MET>>(edm::InputTag("patPFMetT1" + name + "Up"));
          metTokens.at(iSystDown) = iC.mayConsume<std::vector<pat::MET>>(edm::InputTag("patPFMetT1" + name + "Down"));
      }
      else {
        std::cerr << "smth is going wrong... stopping ..." << std::endl;
        exit(0);
      }
      if (ListOfSystematics.at(iSyst) == "LeptonEn" && channel == "el") {
          TokenEnUp =  iC.mayConsume<edm::View<reco::LeafCandidate>>(edm::InputTag("bestElectronEnUp"));
          TokenEnDown =  iC.mayConsume<edm::View<reco::LeafCandidate>>(edm::InputTag("bestElectronEnDown"));
      }

      if (ListOfSystematics.at(iSyst) == "LeptonEn" && channel == "mu") {
          TokenEnUp =  iC.mayConsume<edm::View<reco::LeafCandidate>>(edm::InputTag("bestMuonEnUp"));
          TokenEnDown =  iC.mayConsume<edm::View<reco::LeafCandidate>>(edm::InputTag("bestMuonEnDown"));
      }

      if (ListOfSystematics.at(iSyst) == "LeptonRes" && channel == "el") {
          TokenResUp =  iC.mayConsume<edm::View<reco::LeafCandidate>>(edm::InputTag("bestElectronResUp"));
          TokenResDown =  iC.mayConsume<edm::View<reco::LeafCandidate>>(edm::InputTag("bestElectronResDown"));
      }

      if (ListOfSystematics.at(iSyst) == "LeptonRes" && channel == "mu") {
          TokenResUp =  iC.mayConsume<edm::View<reco::LeafCandidate>>(edm::InputTag("bestMuonResUp"));
          TokenResDown =  iC.mayConsume<edm::View<reco::LeafCandidate>>(edm::InputTag("bestMuonResDown"));
      }
    }
}

   std::map<std::string, math::XYZTLorentzVector> getWLepSystematicsLoretzVectors(const edm::Event& iEvent){
   std::vector<edm::Handle<std::vector<reco::CompositeCandidate> > > WLeps;
   WLeps.resize(2 * ListOfSystematics.size());
   
   std::map<std::string, math::XYZTLorentzVector>  SystMap;

   //loop over available systematics, not everything is supported at the moment!
   for (unsigned int iSyst = 0; iSyst < ListOfSystematics.size(); iSyst ++) {	
   	
   	//get objects from the event
   	iEvent.getByToken(tokens.at( 2*iSyst), WLeps.at(2*iSyst));
   	iEvent.getByToken(tokens.at( 2*iSyst + 1), WLeps.at(2*iSyst + 1));

   	//if there is leptonically decaying W get 4-vecton and insert into map 
   	if (WLeps.at(2*iSyst) -> size() > 0 && WLeps.at(2*iSyst + 1) -> size() > 0) {
   		reco::CompositeCandidate WLepSystUp = (WLeps.at(2*iSyst)) -> at(0); 
   		reco::CompositeCandidate WLepSystDown = (WLeps.at(2*iSyst + 1)) -> at(0); 
   		math::XYZTLorentzVector P4Up = WLepSystUp.p4();
   		math::XYZTLorentzVector P4Down = WLepSystDown.p4();
   		SystMap.insert( std::pair<std::string,math::XYZTLorentzVector>(ListOfSystematics.at(iSyst) + "Up",P4Up) );
   		SystMap.insert( std::pair<std::string,math::XYZTLorentzVector>(ListOfSystematics.at(iSyst) + "Down",P4Down) );
  	}
   }
   


   	return SystMap;


   }


   std::map<std::string, math::XYZTLorentzVector> getLeptonSystematicsLoretzVectors(const edm::Event& iEvent){
   edm::Handle<edm::View<reco::LeafCandidate>>  LeptonEnUp;
   edm::Handle<edm::View<reco::LeafCandidate>>  LeptonEnDown;
   
   edm::Handle<edm::View<reco::LeafCandidate>>  LeptonResUp;
   edm::Handle<edm::View<reco::LeafCandidate>>  LeptonResDown; 
   
   std::map<std::string, math::XYZTLorentzVector>  LeptonSystMap;

   iEvent.getByToken(TokenEnUp, LeptonEnUp);
   iEvent.getByToken(TokenEnDown, LeptonEnDown);
   iEvent.getByToken(TokenResUp, LeptonResUp);
   iEvent.getByToken(TokenResDown, LeptonResDown);

   reco::LeafCandidate _LeptonEnUp    = LeptonEnUp -> at(0); 
   reco::LeafCandidate _LeptonEnDown  = LeptonEnDown -> at(0);
   reco::LeafCandidate _LeptonResUp   = LeptonResUp -> at(0);
   reco::LeafCandidate _LeptonResDown = LeptonResDown -> at(0); 
   

  math::XYZTLorentzVector P4EnUp = _LeptonEnUp.p4();  
  math::XYZTLorentzVector P4EnDown = _LeptonEnDown.p4();  
  math::XYZTLorentzVector P4ResUp = _LeptonResUp.p4();  
  math::XYZTLorentzVector P4ResDown = _LeptonResDown.p4(); 

  LeptonSystMap.insert( std::pair<std::string,math::XYZTLorentzVector>("LeptonEnUp", P4EnUp) );
  LeptonSystMap.insert( std::pair<std::string,math::XYZTLorentzVector>("LeptonEnDown", P4EnDown) );

  LeptonSystMap.insert( std::pair<std::string,math::XYZTLorentzVector>("LeptonResUp", P4ResUp) );
  LeptonSystMap.insert( std::pair<std::string,math::XYZTLorentzVector>("LeptonResDown", P4ResDown) );

  return LeptonSystMap;
}

  std::map<std::string, math::XYZTLorentzVector> getMetSystematicsLoretzVectors(const edm::Event& iEvent){

     std::vector<edm::Handle<std::vector<pat::MET> > > mets;
     mets.resize(2*ListOfSystematics.size());

     std::map<std::string, math::XYZTLorentzVector>  SystMap;

     for (unsigned int iSyst = 0; iSyst < ListOfSystematics.size(); iSyst ++) {

      if (ListOfSystematics.at(iSyst) == "LeptonRes")
        continue;

      // get shifted MET objects from the event
      iEvent.getByToken(metTokens.at(2*iSyst), mets.at(2*iSyst));
      iEvent.getByToken(metTokens.at(2*iSyst + 1), mets.at(2*iSyst + 1));

      // insert shifted MET into map
      if (mets.at(2*iSyst)->size() > 0 && mets.at(2*iSyst + 1)->size() > 0) {
        pat::MET metSystUp = (mets.at(2*iSyst))->at(0);
        pat::MET metSystDown = (mets.at(2*iSyst + 1))->at(0);
        math::XYZTLorentzVector P4Up = metSystUp.p4();
        math::XYZTLorentzVector P4Down = metSystDown.p4();
        SystMap.insert( std::pair<std::string, math::XYZTLorentzVector> (ListOfSystematics.at(iSyst) + "Up", P4Up) );
        SystMap.insert( std::pair<std::string, math::XYZTLorentzVector> (ListOfSystematics.at(iSyst) + "Down", P4Down) );
      }
    }

    return SystMap;
  }
};