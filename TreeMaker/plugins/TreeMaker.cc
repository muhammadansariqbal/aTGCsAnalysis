// system include files
#include <iostream>
#include <memory>
#include <map>
#include <vector>
#include <algorithm> 
#include "boost/algorithm/string.hpp"
// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/Common/interface/View.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"
#include "JetMETCorrections/Objects/interface/JetCorrectionsRecord.h"
#include "FWCore/Framework/interface/EventSetup.h"


#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "PhysicsTools/Utilities/interface/LumiReWeighting.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "CondFormats/JetMETObjects/interface/FactorizedJetCorrector.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/LHERunInfoProduct.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/BTauReco/interface/CATopJetTagInfo.h"
#include "DataFormats/EcalDetId/interface/EcalSubdetector.h"

#include "TTree.h"
#include "TFile.h"
#include "TH2F.h"
#include "TLorentzVector.h"
#include "TRandom.h"

#include "DecayChannel.h"
#include "GenWUtils.h"
#include "DecayClass.h"
#include "Particle.h"
#include "SystematicsHelper.h"
#include "PU.h"
#include "PDFVariationMap.h"
#include "ScaleFactorHelper.h"
#include "BTagHelper.h"
#include "JetResolutionSmearer.h"
#include "EleTriggerEff.h"

namespace reco {
  typedef edm::Ptr<reco::Muon> MuonPtr;
  typedef edm::Ptr<reco::GsfElectron> ElectronPtr;
}
//
// class declaration
//

class TreeMaker : public edm::EDAnalyzer {
public:
  explicit TreeMaker(const edm::ParameterSet&);
  ~TreeMaker();
  //static void fillDescriptions(edm::ConfigurationDescriptions & descriptions);
  
private:
  virtual void beginJob() override;
  virtual void beginRun(const edm::Run&, const edm::EventSetup&) override;
  virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
  virtual void endRun(edm::Run const& iEvent, edm::EventSetup const&) override;
  virtual void endJob() override;

  virtual float getPUPPIweight(float, float); 
  virtual float getSmearingFactor(float sf, float unc, float resolution, const pat::Jet & jet, const edm::View<reco::GenJet> & genJets, int variation, float drMax, float relResMax, bool usePuppiPt);
  // we need all these 3 overloaded methods as we use different 4-vector classes
  // math::XYZTLorentzVector is really a ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> >
  virtual void saveDibosonMass(const ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<double> > & leptonicV_p4, const ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<double> > & hadronicV_p4, double & massVar);
  virtual void saveDibosonMass(math::XYZTLorentzVector & leptonicV_p4, const ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<double> > & hadronicV_p4, double & massVar);
  virtual void saveDibosonMass(math::XYZTLorentzVector & leptonicV_p4, math::XYZTLorentzVector & hadronicV_p4, double & massVar);
  virtual bool decaysHadronic(const reco::Candidate*);
 
  // ----------member data ---------------------------
  TTree* outTree_;

  //event info
  int nevent, run, lumi;
  
  //number of primary vertices
  int nPV;
  double gnPV;

  double PUweight;
  double btagWeight, btagWeight_BTagUp, btagWeight_BTagDown, btagWeight_MistagUp, btagWeight_MistagDown;
  double genWeight;
  double LeptonSF, LeptonSF_Up, LeptonSF_Down;
  double rho_;
  double totWeight, totWeight_BTagUp, totWeight_BTagDown, totWeight_MistagUp, totWeight_MistagDown, totWeight_LeptonIDUp, totWeight_LeptonIDDown;
  double VTagSF;
  double topPtSF;
  Particle Wboson_lep, METCand, Electron, Muon, Lepton;
  double m_pruned;

  //Decay Info (gen level)
  DecayClass WDecayClass;
  //gen info

  double Wplus_gen_pt, Wplus_gen_eta, Wplus_gen_phi, Wplus_gen_mass;
  double Wminus_gen_pt, Wminus_gen_eta, Wminus_gen_phi, Wminus_gen_mass;
  ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<double>> genHadronicVp4, genLeptonicVp4, genDibosonp4;
  double genPtV, genMWV;
  
  int N_had_W, N_lep_W;
  int N_had_Wgen, N_lep_Wgen;
  
  int nLooseEle, nLooseMu, nLep;

  //supercluster variables
  double sc_et, sc_eta;
  bool isEB;

  double tau1, tau2, tau3, tau21;
  
  double deltaR_LeptonWJet, deltaPhi_LeptonMet, deltaPhi_WJetMet, deltaPhi_WJetWlep;
  //systematics from MET
  double deltaPhi_LeptonMet_UnclEnUp, deltaPhi_LeptonMet_UnclEnDown, deltaPhi_LeptonMet_JECUp,deltaPhi_LeptonMet_JECDown, deltaPhi_LeptonMet_JERUp, deltaPhi_LeptonMet_JERDown, deltaPhi_LeptonMet_LeptonEnUp, deltaPhi_LeptonMet_LeptonEnDown;
  double deltaPhi_WJetMet_UnclEnUp, deltaPhi_WJetMet_UnclEnDown, deltaPhi_WJetMet_JECUp,deltaPhi_WJetMet_JECDown, deltaPhi_WJetMet_JERUp, deltaPhi_WJetMet_JERDown, deltaPhi_WJetMet_LeptonEnUp, deltaPhi_WJetMet_LeptonEnDown;
  double deltaPhi_WJetWlep_UnclEnUp, deltaPhi_WJetWlep_UnclEnDown, deltaPhi_WJetWlep_JECUp, deltaPhi_WJetWlep_JECDown, deltaPhi_WJetWlep_LeptonEnUp, deltaPhi_WJetWlep_LeptonEnDown;
  
  // Jet quantities
  int NAK8jet, NAK8jet_smearedUp, NAK8jet_smearedDown, NAK8jet_JECUp, NAK8jet_JECDown;
  int njets, njets_JERUp, njets_JERDown, njets_JECUp, njets_JECDown;
  int nbtag, nbtagMedium, nbtagLoose, nbtag_JERUp, nbtag_JERDown, nbtag_JECUp, nbtag_JECDown;

  double jet_pt, jet_eta, jet_phi, jet_mass, jet_mass_pruned, jet_mass_softdrop, jet_tau2tau1, jet_tau3tau2, jet_tau1, jet_tau2, jet_tau3;
  //PUPPI variables 
  double jet_pt_PUPPI, jet_eta_PUPPI, jet_phi_PUPPI, jet_mass_PUPPI, jet_tau1_PUPPI, jet_tau2_PUPPI, jet_tau3_PUPPI, jet_tau21_PUPPI, jet_tau32_PUPPI, jet_mass_softdrop_PUPPI, jet_tau21_DT;

  //gen info
  bool isMatched_;
  //JEC uncertainties
  double JECunc;
  double jet_pt_JECUp, jet_pt_JECDown, jet_mass_JECUp, jet_mass_JECDown, jet_mass_pruned_JECUp, jet_mass_pruned_JECDown, jet_mass_softdrop_JECUp, jet_mass_softdrop_JECDown, jet_mass_softdrop_PUPPI_JECUp, jet_mass_softdrop_PUPPI_JECDown;
  //JER uncerainties
  double jet_pt_JERUp, jet_pt_JERDown, jet_mass_JERUp, jet_mass_JERDown, jet_mass_softdrop_JERUp, jet_mass_softdrop_JERDown, jet_mass_pruned_JERUp, jet_mass_pruned_JERDown, jet_mass_softdrop_PUPPI_JERUp, jet_mass_softdrop_PUPPI_JERDown;
  //AK4 jets
  double jet2_pt, jet2_btag, jet3_pt, jet3_btag;
  double jet2_eta,jet2_phi, jet3_eta, jet3_phi;

  //additional info for AK4 jets
  std::vector<double> jetFlavours;

  std::vector<double> BgenjetStatus43_pt;
  std::vector<double> BgenjetStatus43_eta;
  std::vector<double> BgenjetStatus43_phi;
  std::vector<double> BgenjetStatus43_mass;
  std::vector<double> BgenjetStatus43_motherPDGID;

  std::vector<double> BgenjetStatus21_pt;
  std::vector<double> BgenjetStatus21_eta;
  std::vector<double> BgenjetStatus21_phi;
  std::vector<double> BgenjetStatus21_mass;
  std::vector<double> BgenjetStatus21_motherPDGID;

  //MET uncertainties
  double MET_UnclEnUp, MET_UnclEnDown, MET_JECUp, MET_JECDown, MET_JERUp, MET_JERDown,  MET_LeptonEnUp, MET_LeptonEnDown;
  //MET phi uncertainties
  double MET_Phi_UnclEnUp, MET_Phi_UnclEnDown, MET_Phi_JECUp, MET_Phi_JECDown, MET_Phi_JERUp, MET_Phi_JERDown, MET_Phi_LeptonEnUp, MET_Phi_LeptonEnDown;
  
  //m_lvj
  double m_lvj, m_lvj_SD;
  //m_lvj systematics
  double m_lvj_UnclEnUp, m_lvj_UnclEnDown, m_lvj_JECUp, m_lvj_JECDown, m_lvj_LeptonEnUp, m_lvj_LeptonEnDown, m_lvj_LeptonResUp, m_lvj_LeptonResDown, m_lvj_JERUp, m_lvj_JERDown;
  double m_lvj_SD_UnclEnUp, m_lvj_SD_UnclEnDown, m_lvj_SD_JECUp, m_lvj_SD_JECDown, m_lvj_SD_LeptonEnUp, m_lvj_SD_LeptonEnDown, m_lvj_SD_LeptonResUp, m_lvj_SD_LeptonResDown, m_lvj_SD_JERUp, m_lvj_SD_JERDown;

  double refXsec;
  //aTGC weights
  std::vector<double> aTGCWeights;
  double aTGCWeightUnitConv;

  int NominalPDF;

  std::vector<double> PDFWeights;
  std::vector<double> ScaleWeights;
  bool bit_HLT_Ele_27_tight, bit_HLT_Ele_45, bit_HLT_Ele_115, bit_HLT_Photon_175, bit_HLT_Ele_27_OR_45_OR_115;

  //Defining Tokens
  edm::EDGetTokenT<std::vector< PileupSummaryInfo > > PUInfoToken_;
  edm::EDGetTokenT<edm::View<pat::MET> > metToken_;
  edm::EDGetTokenT<edm::View<reco::Candidate>> leptonicVToken_;
  edm::EDGetTokenT<edm::View<reco::Candidate>> genParticlesToken_;
  edm::EDGetTokenT<edm::View<pat::Jet>> fatJetsToken_, fatJetsSmearedUpToken_, fatJetsSmearedDownToken_;
  edm::EDGetTokenT<edm::View<reco::GenJet>> genJetsAK8Token_;
  edm::EDGetTokenT<edm::View<pat::Jet>> AK4JetsToken_, AK4JetsSmearedUpToken_, AK4JetsSmearedDownToken_, AK4JetsShiftedUpToken_, AK4JetsShiftedDownToken_;
  edm::EDGetTokenT<edm::View<reco::Vertex> > vertexToken_;
  edm::EDGetTokenT<edm::View<reco::Candidate>> looseMuToken_;
  edm::EDGetTokenT<edm::View<reco::Candidate>> looseEleToken_;
  edm::EDGetTokenT<edm::View<reco::Candidate>> leptonsToken_;
  edm::EDGetTokenT<edm::View<pat::Muon>> muonsToken_;
  edm::EDGetTokenT<edm::TriggerResults> TriggerResultsToken;
  bool isMC;
  edm::EDGetTokenT<double> rhoToken_;
  bool isSignal;
  std::string channel;
  edm::LumiReWeighting  LumiWeights_;
  edm::EDGetTokenT<GenEventInfoProduct> genInfoToken;
  edm::EDGetTokenT<LHEEventProduct> LHEEventProductTokenExternal;
  edm::EDGetTokenT<LHERunInfoProduct> lheProducerToken;
  SystematicsHelper SystematicsHelper_;
  MuonScaleFactor MuonScaleFactor_;
  ElectronScaleFactor ElectronScaleFactor_;
  JetResolutionSmearer<pat::Jet>JetResolutionSmearer_;
  BTagHelper BTagHelper_;
  // For PUPPI Softdrop Mass Correction
  TF1 *puppisd_corrGEN, *puppisd_corrRECO_cen, *puppisd_corrRECO_for;
  JetCorrectionUncertainty * jecUnc;
  double bTagDiscrCut, bTagDiscrCutMedium, bTagDiscrCutLoose;
  // L1 Prefiring weights
  //double prefiringWeight;
  //edm::EDGetTokenT< double > prefweight_token;
};

//
// constructors and destructor
//
TreeMaker::TreeMaker(const edm::ParameterSet& iConfig):
  metToken_(consumes<edm::View<pat::MET>>(iConfig.getParameter<edm::InputTag>("metSrc"))),
  leptonicVToken_(consumes<edm::View<reco::Candidate>>(iConfig.getParameter<edm::InputTag>("leptonicVSrc"))),
  genParticlesToken_(mayConsume<edm::View<reco::Candidate>>(iConfig.getParameter<edm::InputTag>("genSrc"))),
  fatJetsToken_(consumes<edm::View<pat::Jet>>(iConfig.getParameter<edm::InputTag>("fatJetSrc"))),
  genJetsAK8Token_(mayConsume<edm::View<reco::GenJet>>(iConfig.getParameter<edm::InputTag>("genJetsAK8Src"))),
  AK4JetsToken_(consumes<edm::View<pat::Jet>>(iConfig.getParameter<edm::InputTag>("AK4JetSrc"))),
  vertexToken_(consumes<edm::View<reco::Vertex>>(iConfig.getParameter<edm::InputTag>("vertexSrc"))),
  looseMuToken_(consumes<edm::View<reco::Candidate>>(iConfig.getParameter<edm::InputTag>("looseMuSrc"))),
  looseEleToken_(consumes<edm::View<reco::Candidate>>(iConfig.getParameter<edm::InputTag>("looseEleSrc"))),
  leptonsToken_(consumes<edm::View<reco::Candidate>>(iConfig.getParameter<edm::InputTag>("leptonSrc"))),
  muonsToken_(mayConsume<edm::View<pat::Muon>>(iConfig.getParameter<edm::InputTag>("leptonSrc"))),
  isMC(iConfig.getParameter<bool>("isMC")),
  rhoToken_(consumes<double> (iConfig.getParameter<edm::InputTag>("rho"))),
  isSignal(iConfig.getParameter<bool>("isSignal")),
  channel(iConfig.getParameter<std::string>("channel")),
  SystematicsHelper_(SystematicsHelper()),
  MuonScaleFactor_(
    "aTGCsAnalysis/TreeMaker/data/Muon_TrackingSF_RunBtoH.root",
    "aTGCsAnalysis/TreeMaker/data/Muon_IDSF_average_RunBtoH.root",
    "aTGCsAnalysis/TreeMaker/data/Muon_IsoSF_average_RunBtoH.root",
    "aTGCsAnalysis/TreeMaker/data/Muon_SingleLeptonTriggerSF_average_RunBtoH.root",
    "HighPtID",
    "tkLooseISO"
  ),
  ElectronScaleFactor_(
    "aTGCsAnalysis/TreeMaker/data/HEEP_SF.root",
    "aTGCsAnalysis/TreeMaker/data/egammaEffi.txt_EGM2D.root"
  ),
  JetResolutionSmearer_(iConfig.getParameter<bool>("isMC")),
  BTagHelper_(iConfig.getParameter<std::string>("BtagEffFile"), iConfig.getParameter<double>("BtagDiscrCut")),
  bTagDiscrCut(iConfig.getParameter<double>("BtagDiscrCut")),
  bTagDiscrCutMedium(iConfig.getParameter<double>("BtagDiscrCutMedium")),
  bTagDiscrCutLoose(iConfig.getParameter<double>("BtagDiscrCutLoose"))
{

  if ((channel != "mu") && (channel != "el")) {
    throw cms::Exception("InvalidValue") << "Invalid value for channel parameter, should be mu or el." << std::endl;
  }

  // For PUPPI Correction
  edm::FileInPath puppiCorr("aTGCsAnalysis/TreeMaker/data/puppiCorrSummer16.root");
  TFile* file = TFile::Open( puppiCorr.fullPath().c_str(),"READ");
  puppisd_corrGEN      = (TF1*)file->Get("puppiJECcorr_gen");
  puppisd_corrRECO_cen = (TF1*)file->Get("puppiJECcorr_reco_0eta1v3");
  puppisd_corrRECO_for = (TF1*)file->Get("puppiJECcorr_reco_1v3eta2v5");

  //loading PU and generator information for MC
   if (isMC) {
     SystematicsHelper_  = SystematicsHelper(channel, consumesCollector());

    // Use the AK8 jet label as basis
    fatJetsSmearedUpToken_ = consumes<edm::View<pat::Jet>>(edm::InputTag(iConfig.getParameter<edm::InputTag>("fatJetSrc").label() + "SmearedUp"));
    fatJetsSmearedDownToken_ = consumes<edm::View<pat::Jet>>(edm::InputTag(iConfig.getParameter<edm::InputTag>("fatJetSrc").label() + "SmearedDown"));

    // Use the AK4 jet label as basis
    AK4JetsSmearedUpToken_ = consumes<edm::View<pat::Jet>>(edm::InputTag(iConfig.getParameter<edm::InputTag>("AK4JetSrc").label() + "SmearedUp"));
    AK4JetsSmearedDownToken_ = consumes<edm::View<pat::Jet>>(edm::InputTag(iConfig.getParameter<edm::InputTag>("AK4JetSrc").label() + "SmearedDown"));

    AK4JetsShiftedUpToken_ = consumes<edm::View<pat::Jet>>(edm::InputTag(iConfig.getParameter<edm::InputTag>("AK4JetSrc").label() + "ShiftedUp"));
    AK4JetsShiftedDownToken_ = consumes<edm::View<pat::Jet>>(edm::InputTag(iConfig.getParameter<edm::InputTag>("AK4JetSrc").label() + "ShiftedDown"));

     PUInfoToken_ = consumes<std::vector< PileupSummaryInfo > >(iConfig.getParameter<edm::InputTag>("PUInfo"));

     //PU-reweighting
     LumiWeights_ = edm::LumiReWeighting(MC_dist(), data_dist());

    genInfoToken = mayConsume<GenEventInfoProduct> (iConfig.getParameter<edm::InputTag>( "genInfo" ) );
    LHEEventProductTokenExternal = mayConsume<LHEEventProduct> (iConfig.getParameter<edm::InputTag>( "LHEEventProductSrcExternal" ) );
    if(!isSignal)lheProducerToken = consumes< LHERunInfoProduct, edm::InRun >(edm::InputTag("externalLHEProducer"));
    else lheProducerToken = consumes< LHERunInfoProduct, edm::InRun >(edm::InputTag("externalLHEProducer"));
    VTagSF = iConfig.getParameter<double>("VTagSF");

    // L1 Prefiring weights
    //prefweight_token = consumes< double >(edm::InputTag("prefiringweight:NonPrefiringProb"));

   }

   jecUnc = nullptr;

   if (channel == "el")TriggerResultsToken = consumes<edm::TriggerResults>(iConfig.getParameter<edm::InputTag>("triggers"));

  //now do what ever initialization is needed
  edm::Service<TFileService> fs;
  outTree_ = fs->make<TTree>("BasicTree","BasicTree");

  //event info
  outTree_->Branch("event",	      &nevent,    	  "event/I"           );
  outTree_->Branch("lumi", 	      &lumi,   		  "lumi/I"  		);
  outTree_->Branch("run",	      &run,		  "run/I"  	       );
  
  //number of primary vertices
  outTree_->Branch("nPV",	      &nPV,		  "nPV/I"  	       );
  outTree_->Branch("rho",       &rho_,     "rho/D"          );
  //PUweight
  if (isMC) {
     outTree_->Branch("gnPV", &gnPV, "gnPV/D");

     outTree_->Branch("puweight",       &PUweight,     "puweight/D"          );
     outTree_->Branch("LeptonSF",       &LeptonSF,     "LeptonSF/D"          );
     outTree_->Branch("LeptonSF_Up",       &LeptonSF_Up,     "LeptonSF_Up/D"          );
     outTree_->Branch("LeptonSF_Down",       &LeptonSF_Down,     "LeptonSF_Down/D"          );
     outTree_->Branch("genweight",       &genWeight,     "genweight/D"          );
     outTree_->Branch("btagWeight",       &btagWeight,     "btagWeight/D"          );
     outTree_->Branch("btagWeight_BTagUp",       &btagWeight_BTagUp,     "btagWeight_BTagUp/D"          );
     outTree_->Branch("btagWeight_BTagDown",       &btagWeight_BTagDown,     "btagWeight_BTagDown/D"          );
     outTree_->Branch("btagWeight_MistagUp",       &btagWeight_MistagUp,     "btagWeight_MistagUp/D"          );
     outTree_->Branch("btagWeight_MistagDown",       &btagWeight_MistagDown,     "btagWeight_MistagDown/D"          );
     outTree_->Branch("topPtSF",       &topPtSF,     "topPtSF/D"          );
     //total weights: central and systematics
     outTree_->Branch("totWeight",       &totWeight,     "totWeight/D"          );
     
     outTree_->Branch("totWeight_BTagUp",       &totWeight_BTagUp,     "totWeight_BTagUp/D"          );
     outTree_->Branch("totWeight_BTagDown",       &totWeight_BTagDown,     "totWeight_BTagDown/D"          );
     
     outTree_->Branch("totWeight_MistagUp",       &totWeight_MistagUp,     "totWeight_MistagUp/D"          );
     outTree_->Branch("totWeight_MistagDown",       &totWeight_MistagDown,     "totWeight_MistagDown/D"          );

     outTree_->Branch("totWeight_LeptonIDUp",       &totWeight_LeptonIDUp,     "totWeight_LeptonIDUp/D"          );
     outTree_->Branch("totWeight_LeptonIDDown",       &totWeight_LeptonIDDown,     "totWeight_LeptonIDDown/D"          );


     //PDF and scale weights: systematics
     outTree_->Branch("PDFWeights","std::vector<double>",&PDFWeights);
     outTree_->Branch("ScaleWeights","std::vector<double>",&ScaleWeights);
     //generator info about the decay of WW
     outTree_->Branch("WDecayClass",     &WDecayClass,    "WDecayClass/I"      );
     //generator W info
     outTree_->Branch("Wplus_gen_pt",     &Wplus_gen_pt,    "Wplus_gen_pt/D"      );
     outTree_->Branch("Wplus_gen_eta",     &Wplus_gen_eta,    "Wplus_gen_eta/D"      );
     outTree_->Branch("Wplus_gen_phi",     &Wplus_gen_phi,    "Wplus_gen_phi/D"      );
     outTree_->Branch("Wplus_gen_mass",     &Wplus_gen_mass,    "Wplus_gen_mass/D"      );

     outTree_->Branch("Wminus_gen_pt",     &Wminus_gen_pt,    "Wminus_gen_pt/D"      );
     outTree_->Branch("Wminus_gen_eta",     &Wminus_gen_eta,    "Wminus_gen_eta/D"      );
     outTree_->Branch("Wminus_gen_phi",     &Wminus_gen_phi,    "Wminus_gen_phi/D"      );
     outTree_->Branch("Wminus_gen_mass",     &Wminus_gen_mass,    "Wminus_gen_mass/D"      );

     outTree_->Branch("genPtV", &genPtV, "genPtV/D");
     outTree_->Branch("genMWV", &genMWV, "genMWV/D");

     // Prefiring weight
     //outTree_->Branch("prefiringWeight", &prefiringWeight, "prefiringWeight/D");
   };
  if (channel == "el") {
    outTree_->Branch("bit_HLT_Ele_27_tight",       &bit_HLT_Ele_27_tight,     "bit_HLT_Ele_27_tight/B"          );
    outTree_->Branch("bit_HLT_Ele_45",       &bit_HLT_Ele_45,     "bit_HLT_Ele_45/B"          );
    outTree_->Branch("bit_HLT_Ele_115",       &bit_HLT_Ele_115,     "bit_HLT_Ele_115/B"          );
    outTree_->Branch("bit_HLT_Photon_175",       &bit_HLT_Photon_175,     "bit_HLT_Photon_175/B"          );
    outTree_->Branch("bit_HLT_Ele_27_OR_45_OR_115",       &bit_HLT_Ele_27_OR_45_OR_115,     "bit_HLT_Ele_27_OR_45_OR_115/B");
  }
  
  //number of loose leptons
  outTree_->Branch("nLooseEle",      &nLooseEle, 	  "nLooseEle/I"       );
  outTree_->Branch("nLooseMu",      &nLooseMu, 	  "nLooseMu/I"       );
  
  // number of leptons for particular mu/ele channel (should be tight lepton)
  outTree_->Branch("nLep",            &nLep, 	      "nLep/I"       );
  
  //leptons (tight, ele or mu depends on the channel)
  outTree_->Branch("l_pt",	      &Lepton.pt,      "l_pt/D"         	);
  outTree_->Branch("l_eta",	      &Lepton.eta,     "l_eta/D"        	);
  outTree_->Branch("l_phi",	      &Lepton.phi,     "l_phi/D"        	);
  //lepton uncertainties
  if (isMC) {
    if(channel == "el"){
        outTree_->Branch("sc_eta",       &sc_eta,     "sc_eta/D"          );  
        outTree_->Branch("sc_et",       &sc_et,     "sc_et/D"          );  
        outTree_->Branch("isEB",       &isEB,     "isEB/B"          );  
    }
    outTree_->Branch("l_pt_LeptonEnUp",       &Lepton.pt_LeptonEnUp,     "l_pt_LeptonEnUp/D"          );
    outTree_->Branch("l_pt_LeptonEnDown",     &Lepton.pt_LeptonEnDown,   "l_pt_LeptonEnDown/D"          );
    outTree_->Branch("l_pt_LeptonResUp",      &Lepton.pt_LeptonResUp,    "l_pt_LeptonResUp/D"          );
    outTree_->Branch("l_pt_LeptonResDown",    &Lepton.pt_LeptonResDown,  "l_pt_LeptonResDown/D"          );
  }
  //W leptonic observables
  outTree_->Branch("W_pt",	      &Wboson_lep.pt,     "W_pt/D"         );
  outTree_->Branch("W_eta",	      &Wboson_lep.eta,    "W_eta/D"        );
  outTree_->Branch("W_phi",	      &Wboson_lep.phi,    "W_phi/D"        );
  outTree_->Branch("W_mass",      &Wboson_lep.mass,   "W_mass/D"       );
  outTree_->Branch("W_mt",	      &Wboson_lep.mt,     "W_mt/D"         );

  if (isMC){
    //pt
    outTree_->Branch("W_pt_LeptonEnUp",        &Wboson_lep.pt_LeptonEnUp,     "W_pt_LeptonEnUp/D"         );
    outTree_->Branch("W_pt_LeptonEnDown",      &Wboson_lep.pt_LeptonEnDown,   "W_pt_LeptonEnDown/D"       );
    
    outTree_->Branch("W_pt_LeptonResUp",        &Wboson_lep.pt_LeptonResUp,     "W_pt_LeptonResUp/D"         );
    outTree_->Branch("W_pt_LeptonResDown",      &Wboson_lep.pt_LeptonResDown,   "W_pt_LeptonResDown/D"       );

    outTree_->Branch("W_pt_JECUp",        &Wboson_lep.pt_JECUp,     "W_pt_JECUp/D"         );
    outTree_->Branch("W_pt_JECDown",      &Wboson_lep.pt_JECDown,   "W_pt_JECDown/D"       );

    outTree_->Branch("W_pt_UnclEnUp",        &Wboson_lep.pt_UnclEnUp,     "W_pt_UnclEnUp/D"         );
    outTree_->Branch("W_pt_UnclEnDown",      &Wboson_lep.pt_UnclEnDown,   "W_pt_UnclEnDown/D"       );

    //mass 
    outTree_->Branch("W_mass_LeptonEnUp",        &Wboson_lep.mass_LeptonEnUp,     "W_mass_LeptonEnUp/D"         );
    outTree_->Branch("W_mass_LeptonEnDown",      &Wboson_lep.mass_LeptonEnDown,   "W_mass_LeptonEnDown/D"       );
    
    outTree_->Branch("W_mass_LeptonResUp",        &Wboson_lep.mass_LeptonResUp,     "W_mass_LeptonResUp/D"         );
    outTree_->Branch("W_mass_LeptonResDown",      &Wboson_lep.mass_LeptonResDown,   "W_mass_LeptonResDown/D"       );

    outTree_->Branch("W_mass_JECUp",        &Wboson_lep.mass_JECUp,     "W_mass_JECUp/D"         );
    outTree_->Branch("W_mass_JECDown",      &Wboson_lep.mass_JECDown,   "W_mass_JECDown/D"       );

    outTree_->Branch("W_mass_UnclEnUp",        &Wboson_lep.mass_UnclEnUp,     "W_mass_UnclEnUp/D"         );
    outTree_->Branch("W_mass_UnclEnDown",      &Wboson_lep.mass_UnclEnDown,   "W_mass_UnclEnDown/D"       );

    //mt 
    outTree_->Branch("W_mt_LeptonEnUp",        &Wboson_lep.mt_LeptonEnUp,     "W_mt_LeptonEnUp/D"         );
    outTree_->Branch("W_mt_LeptonEnDown",      &Wboson_lep.mt_LeptonEnDown,   "W_mt_LeptonEnDown/D"       );
    
    outTree_->Branch("W_mt_LeptonResUp",        &Wboson_lep.mt_LeptonResUp,     "W_mt_LeptonResUp/D"         );
    outTree_->Branch("W_mt_LeptonResDown",      &Wboson_lep.mt_LeptonResDown,   "W_mt_LeptonResDown/D"       );

    outTree_->Branch("W_mt_JECUp",        &Wboson_lep.mt_JECUp,     "W_mt_JECUp/D"         );
    outTree_->Branch("W_mt_JECDown",      &Wboson_lep.mt_JECDown,   "W_mt_JECDown/D"       );

    outTree_->Branch("W_mt_UnclEnUp",        &Wboson_lep.mt_UnclEnUp,     "W_mt_UnclEnUp/D"         );
    outTree_->Branch("W_mt_UnclEnDown",      &Wboson_lep.mt_UnclEnDown,   "W_mt_UnclEnDown/D"       );
    
  } 

  
  outTree_->Branch("charge_W_lep",    &Wboson_lep.charge, "charge_W_lep/D"     );
    
  outTree_->Branch("N_lep_W",	      &N_lep_W,		  "N_lep_W/I"          );
  
  outTree_->Branch("N_had_W_gen",     &N_had_Wgen,	  "N_had_W_gen/I"      );
  outTree_->Branch("N_lep_W_gen",     &N_lep_Wgen, 	  "N_lep_W_gen/I"      );

  //MET observables  
  outTree_->Branch("pfMET", 	      &METCand.pt, 	  "pfMET/D"              );
  outTree_->Branch("pfMETPhi",	      &METCand.phi, 	  "pfMETPhi/D"          );
  

  if (isMC) {
    //MET uncertainties
    //UnclEn
    outTree_->Branch("pfMET_UnclEnUp",         &MET_UnclEnUp,    "pfMET_UnclEnUp/D"              );
    outTree_->Branch("pfMET_UnclEnDown",         &MET_UnclEnDown,    "pfMET_UnclEnDown/D"              );
    //JER
    outTree_->Branch("pfMET_JERUp",         &MET_JERUp,    "pfMET_JERUp/D"              );
    outTree_->Branch("pfMET_JERDown",         &MET_JERDown,    "pfMET_JERDown/D"              );
    //JEC
    outTree_->Branch("pfMET_JECUp",         &MET_JECUp,    "pfMET_JECUp/D"              );
    outTree_->Branch("pfMET_JECDown",         &MET_JECDown,    "pfMET_JECDown/D"              );
    //Lepton energy energy scale
    outTree_->Branch("pfMET_LeptonEnUp",         &MET_LeptonEnUp,    "pfMET_LeptonEnUp/D"              );
    outTree_->Branch("pfMET_LeptonEnDown",         &MET_LeptonEnDown,    "pfMET_LeptonEnDown/D"              );
   
    //MET phi uncertainties
    //UnclEn
    outTree_->Branch("pfMETPhi_UnclEnUp",         &MET_Phi_UnclEnUp,    "pfMETPhi_UnclEnUp/D"              );
    outTree_->Branch("pfMETPhi_UnclEnDown",         &MET_Phi_UnclEnDown,    "pfMETPhi_UnclEnDown/D"              );
    //JER
    outTree_->Branch("pfMETPhi_JERUp",         &MET_Phi_JERUp,    "pfMETPhi_JERUp/D"              );
    outTree_->Branch("pfMETPhi_JERDown",         &MET_Phi_JERDown,    "pfMETPhi_JERDown/D"              );
    //JEC
    outTree_->Branch("pfMETPhi_JECUp",         &MET_Phi_JECUp,    "pfMETPhi_JECUp/D"              );
    outTree_->Branch("pfMETPhi_JECDown",         &MET_Phi_JECDown,    "pfMETPhi_JECDown/D"              );
    //Lepton energy scale
    outTree_->Branch("pfMETPhi_LeptonEnUp",         &MET_Phi_LeptonEnUp,    "pfMETPhi_LeptonEnUp/D"              );
    outTree_->Branch("pfMETPhi_LeptonEnDown",         &MET_Phi_LeptonEnDown,    "pfMETPhi_LeptonEnDown/D"              );
   
  }
  /// Other observables
  outTree_->Branch("deltaR_LeptonWJet",  &deltaR_LeptonWJet,	  "deltaR_LeptonWJet/D"   );
  outTree_->Branch("deltaPhi_LeptonMet", &deltaPhi_LeptonMet,	  "deltaPhi_LeptonMet/D"  );
  outTree_->Branch("deltaPhi_WJetMet",&deltaPhi_WJetMet,  "deltaPhi_WJetMet/D" );
  outTree_->Branch("deltaPhi_WJetWlep",&deltaPhi_WJetWlep,  "deltaPhi_WJetWlep/D" );
  if (isMC){
    //UnclEn
    outTree_->Branch("deltaPhi_LeptonMet_UnclEnUp", &deltaPhi_LeptonMet_UnclEnUp,   "deltaPhi_LeptonMet_UnclEnUp/D"  );
    outTree_->Branch("deltaPhi_LeptonMet_UnclEnDown", &deltaPhi_LeptonMet_UnclEnDown,   "deltaPhi_LeptonMet_UnclEnDown/D"  );
    //JEC
    outTree_->Branch("deltaPhi_LeptonMet_JECUp", &deltaPhi_LeptonMet_JECUp,   "deltaPhi_LeptonMet_JECUp/D"  );
    outTree_->Branch("deltaPhi_LeptonMet_JECDown", &deltaPhi_LeptonMet_JECDown,   "deltaPhi_LeptonMet_JECDown/D"  );
    //Lepton energy scale
    outTree_->Branch("deltaPhi_LeptonMet_LeptonEnUp", &deltaPhi_LeptonMet_LeptonEnUp,   "deltaPhi_LeptonMet_LeptonEnUp/D"  );
    outTree_->Branch("deltaPhi_LeptonMet_LeptonEnDown", &deltaPhi_LeptonMet_LeptonEnDown,   "deltaPhi_LeptonMet_LeptonEnDown/D"  );
    //JER
    outTree_->Branch("deltaPhi_LeptonMet_JERUp", &deltaPhi_LeptonMet_JERUp,   "deltaPhi_LeptonMet_JERUp/D"  );
    outTree_->Branch("deltaPhi_LeptonMet_JERDown", &deltaPhi_LeptonMet_JERDown,   "deltaPhi_LeptonMet_LeptonEnDown/D"  );
    /////////////////////
    //UnclEn
    outTree_->Branch("deltaPhi_WJetMet_UnclEnUp", &deltaPhi_WJetMet_UnclEnUp,   "deltaPhi_WJetMet_UnclEnUp/D"  );
    outTree_->Branch("deltaPhi_WJetMet_UnclEnDown", &deltaPhi_WJetMet_UnclEnDown,   "deltaPhi_WJetMet_UnclEnDown/D"  );
    //JEC
    outTree_->Branch("deltaPhi_WJetMet_JECUp", &deltaPhi_WJetMet_JECUp,   "deltaPhi_WJetMet_JECUp/D"  );
    outTree_->Branch("deltaPhi_WJetMet_JECDown", &deltaPhi_WJetMet_JECDown,   "deltaPhi_WJetMet_JECDown/D"  );
    //Lepton energy scale
    outTree_->Branch("deltaPhi_WJetMet_LeptonEnUp", &deltaPhi_WJetMet_LeptonEnUp,   "deltaPhi_WJetMet_LeptonEnUp/D"  );
    outTree_->Branch("deltaPhi_WJetMet_LeptonEnDown", &deltaPhi_WJetMet_LeptonEnDown,   "deltaPhi_WJetMet_LeptonEnDown/D"  );
    //JER
    outTree_->Branch("deltaPhi_WJetMet_JERUp", &deltaPhi_WJetMet_JERUp,   "deltaPhi_WJetMet_JERUp/D"  );
    outTree_->Branch("deltaPhi_WJetMet_JERDown", &deltaPhi_WJetMet_JERDown,   "deltaPhi_WJetMet_LeptonEnDown/D"  );
    /////////////////////
    //UnclEn
    outTree_->Branch("deltaPhi_WJetWlep_UnclEnUp", &deltaPhi_WJetWlep_UnclEnUp,   "deltaPhi_WJetWlep_UnclEnUp/D"  );
    outTree_->Branch("deltaPhi_WJetWlep_UnclEnDown", &deltaPhi_WJetWlep_UnclEnDown,   "deltaPhi_WJetWlep_UnclEnDown/D"  );
    //JEC
    outTree_->Branch("deltaPhi_WJetWlep_JECUp", &deltaPhi_WJetWlep_JECUp,   "deltaPhi_WJetWlep_JECUp/D"  );
    outTree_->Branch("deltaPhi_WJetWlep_JECDown", &deltaPhi_WJetWlep_JECDown,   "deltaPhi_WJetWlep_JECDown/D"  );
    //Lepton energy scale
    outTree_->Branch("deltaPhi_WJetWlep_LeptonEnUp", &deltaPhi_WJetWlep_LeptonEnUp,   "deltaPhi_WJetWlep_LeptonEnUp/D"  );
    outTree_->Branch("deltaPhi_WJetWlep_LeptonEnDown", &deltaPhi_WJetWlep_LeptonEnDown,   "deltaPhi_WJetWlep_LeptonEnDown/D"  );
  }
  //Jet observables
  outTree_->Branch("NAK8jet",            &NAK8jet,              "NAK8jet/I"   );
  outTree_->Branch("jet_pt",  	      &jet_pt,	  	  "jet_pt/D"   );
  outTree_->Branch("jet_eta",  	      &jet_eta,	  	  "jet_eta/D"   );
  outTree_->Branch("jet_phi",  	      &jet_phi,	  	  "jet_phi/D"   );
  outTree_->Branch("jet_mass",         &jet_mass,       "jet_mass/D"   );
  outTree_->Branch("Mjpruned", &jet_mass_pruned,	  "Mjpruned/D"   );
  outTree_->Branch("jet_mass_softdrop",&jet_mass_softdrop,"jet_mass_softdrop/D"   );
  outTree_->Branch("jet_tau2tau1",    &jet_tau2tau1,	  "jet_tau2tau1/D"   );
  outTree_->Branch("jet_tau3tau2",    &jet_tau3tau2,    "jet_tau3tau2/D"   );
  outTree_->Branch("jet_tau1",    &jet_tau1,    "jet_tau1/D"   );
  outTree_->Branch("jet_tau2",    &jet_tau2,    "jet_tau2/D"   );
  outTree_->Branch("jet_tau3",    &jet_tau3,    "jet_tau3/D"   );
  //PUPPI variables
  outTree_->Branch("jet_pt_PUPPI",    &jet_pt_PUPPI,    "jet_pt_PUPPI/D"   );
  outTree_->Branch("jet_eta_PUPPI",    &jet_eta_PUPPI,    "jet_eta_PUPPI/D"   );
  outTree_->Branch("jet_phi_PUPPI",    &jet_phi_PUPPI,    "jet_phi_PUPPI/D"   );
  outTree_->Branch("jet_mass_PUPPI",    &jet_mass_PUPPI,    "jet_mass_PUPPI/D"   );
  outTree_->Branch("jet_tau1_PUPPI",    &jet_tau1_PUPPI,    "jet_tau1_PUPPI/D"   );
  outTree_->Branch("jet_tau2_PUPPI",    &jet_tau2_PUPPI,    "jet_tau2_PUPPI/D"   );
  outTree_->Branch("jet_tau3_PUPPI",    &jet_tau3_PUPPI,    "jet_tau3_PUPPI/D"   );
  outTree_->Branch("jet_tau21_PUPPI",    &jet_tau21_PUPPI,    "jet_tau21_PUPPI/D"   );
  outTree_->Branch("jet_tau32_PUPPI",    &jet_tau32_PUPPI,    "jet_tau32_PUPPI/D"   );
  outTree_->Branch("jet_mass_softdrop_PUPPI",    &jet_mass_softdrop_PUPPI,    "jet_mass_softdrop_PUPPI/D"   );
  outTree_->Branch("jet_tau21_DT",    &jet_tau21_DT,    "jet_tau21_DT/D"   );
  
  if (isMC) {
     //JEC uncertainties
    outTree_->Branch("JECunc",    &JECunc,    "JECunc/D"   ); 
    outTree_->Branch("jet_pt_JECUp",    &jet_pt_JECUp,    "jet_pt_JECUp/D"   ); 
    outTree_->Branch("jet_pt_JECDown",    &jet_pt_JECDown,    "jet_pt_JECDown/D"   );  
    outTree_->Branch("jet_mass_JECUp",    &jet_mass_JECUp,    "jet_mass_JECUp/D"   ); 
    outTree_->Branch("jet_mass_JECDown",    &jet_mass_JECDown,    "jet_mass_JECDown/D"   );  
    //JER uncertainties
    outTree_->Branch("jet_pt_JERUp",    &jet_pt_JERUp,    "jet_pt_JERUp/D"   ); 
    outTree_->Branch("jet_pt_JERDown",    &jet_pt_JERDown,    "jet_pt_JERDown/D"   );  
    outTree_->Branch("jet_mass_JERUp",    &jet_mass_JERUp,    "jet_mass_JERUp/D"   ); 
    outTree_->Branch("jet_mass_JERDown",    &jet_mass_JERDown,    "jet_mass_JERDown/D"   );  
    //JEC uncertainties
    outTree_->Branch("Mjpruned_JECUp",    &jet_mass_pruned_JECUp,    "Mjpruned_JECUp/D"   ); 
    outTree_->Branch("Mjpruned_JECDown",    &jet_mass_pruned_JECDown,    "Mjpruned_JECDown/D"   );  
    outTree_->Branch("jet_mass_softdrop_JECUp",    &jet_mass_softdrop_JECUp,    "jet_mass_softdrop_JECUp/D"   ); 
    outTree_->Branch("jet_mass_softdrop_JECDown",    &jet_mass_softdrop_JECDown,    "jet_mass_softdrop_JECDown/D"   );  
    outTree_->Branch("jet_mass_softdrop_PUPPI_JECUp",    &jet_mass_softdrop_PUPPI_JECUp,    "jet_mass_softdrop_PUPPI_JECUp/D"   );
    outTree_->Branch("jet_mass_softdrop_PUPPI_JECDown",    &jet_mass_softdrop_PUPPI_JECDown,    "jet_mass_softdrop_PUPPI_JECDown/D"   );
    //JER uncertainties
    outTree_->Branch("Mjpruned_JERUp",    &jet_mass_pruned_JERUp,    "Mjpruned_JERUp/D"   ); 
    outTree_->Branch("Mjpruned_JERDown",    &jet_mass_pruned_JERDown,    "Mjpruned_JERDown/D"   );  
    outTree_->Branch("jet_mass_softdrop_JERUp",    &jet_mass_softdrop_JERUp,    "jet_mass_softdrop_JERUp/D"   ); 
    outTree_->Branch("jet_mass_softdrop_JERDown",    &jet_mass_softdrop_JERDown,    "jet_mass_softdrop_JERDown/D"   );  
    outTree_->Branch("jet_mass_softdrop_PUPPI_JERUp",    &jet_mass_softdrop_PUPPI_JERUp,    "jet_mass_softdrop_PUPPI_JERUp/D"   );
    outTree_->Branch("jet_mass_softdrop_PUPPI_JERDown",    &jet_mass_softdrop_PUPPI_JERDown,    "jet_mass_softdrop_PUPPI_JERDown/D"   );
    outTree_->Branch("isMatched",    &isMatched_,    "isMatched/B"   ); 
    //add info for AK4 jets
    outTree_ -> Branch("jetFlavours",  &jetFlavours); 
    
    outTree_ -> Branch("BgenjetStatus21_pt",  &BgenjetStatus21_pt); 
    outTree_ -> Branch("BgenjetStatus21_eta",  &BgenjetStatus21_eta); 
    outTree_ -> Branch("BgenjetStatus21_phi",  &BgenjetStatus21_phi); 
    outTree_ -> Branch("BgenjetStatus21_mass",  &BgenjetStatus21_mass); 
    outTree_ -> Branch("BgenjetStatus21_motherPDGID",  &BgenjetStatus21_motherPDGID); 

    outTree_ -> Branch("BgenjetStatus43_pt",  &BgenjetStatus43_pt); 
    outTree_ -> Branch("BgenjetStatus43_eta",  &BgenjetStatus43_eta); 
    outTree_ -> Branch("BgenjetStatus43_phi",  &BgenjetStatus43_phi); 
    outTree_ -> Branch("BgenjetStatus43_mass",  &BgenjetStatus43_mass); 
    outTree_ -> Branch("BgenjetStatus43_motherPDGID",  &BgenjetStatus43_motherPDGID); 
  }
  outTree_->Branch("njets",         &njets,           "njets/I"   );
  outTree_->Branch("nbtag",         &nbtag,           "nbtag/I"   );
  outTree_->Branch("nbtagMedium",         &nbtagMedium,           "nbtagMedium/I"   );
  outTree_->Branch("nbtagLoose",         &nbtagLoose,           "nbtagLoose/I"   );

  if (isMC) {
    outTree_->Branch("njets_JERUp",         &njets_JERUp,           "njets_JERUp/I"   );
    outTree_->Branch("njets_JERDown",       &njets_JERDown,         "njets_JERDown/I"   );

    outTree_->Branch("nbtag_JERUp",         &nbtag_JERUp,           "nbtag_JERUp/I"   );
    outTree_->Branch("nbtag_JERDown",       &nbtag_JERDown,         "nbtag_JERDown/I"   );

    outTree_->Branch("njets_JECUp",         &njets_JECUp,           "njets_JECRUp/I"   );
    outTree_->Branch("njets_JECDown",       &njets_JECDown,         "njets_JECDown/I"   );

    outTree_->Branch("nbtag_JECUp",         &nbtag_JECUp,           "nbtag_JECUp/I"   );
    outTree_->Branch("nbtag_JECDown",       &nbtag_JECDown,         "nbtag_JECDown/I"   );

  }
  
  outTree_->Branch("jet2_pt",  	      &jet2_pt,	          "jet2_pt/D"   );
  outTree_->Branch("jet2_btag",       &jet2_btag,         "jet2_btag/D"   );
  outTree_->Branch("jet3_pt",  	      &jet3_pt,	          "jet3_pt/D"   );
  outTree_->Branch("jet3_btag",	      &jet3_btag,         "jet3_btag/D"   );
  
  outTree_->Branch("MWW",       &m_lvj,         "MWW/D"   );
  outTree_->Branch("MWW_SD",	      &m_lvj_SD,         "MWW_SD/D"   );
  if (isMC) {
    outTree_->Branch("MWW_UnclEnUp",       &m_lvj_UnclEnUp,         "MWW_UnclEnUp/D"   );
    outTree_->Branch("MWW_UnclEnDown",       &m_lvj_UnclEnDown,         "MWW_UnclEnDown/D"   );      
    outTree_->Branch("MWW_JECUp",       &m_lvj_JECUp,         "MWW_JECUp/D"   );
    outTree_->Branch("MWW_JECDown",       &m_lvj_JECDown,         "MWW_JECDown/D"   );  
    outTree_->Branch("MWW_LeptonEnUp",       &m_lvj_LeptonEnUp,         "MWW_LeptonEnUp/D"   );
    outTree_->Branch("MWW_LeptonEnDown",       &m_lvj_LeptonEnDown,         "MWW_LeptonEnDown/D"   );      
    outTree_->Branch("MWW_LeptonResUp",       &m_lvj_LeptonResUp,         "MWW_LeptonResUp/D"   );
    outTree_->Branch("MWW_LeptonResDown",       &m_lvj_LeptonResDown,         "MWW_LeptonResDown/D"   );   
    outTree_->Branch("MWW_JERUp",       &m_lvj_JERUp,         "MWW_JERUp/D"   );
    outTree_->Branch("MWW_JERDown",       &m_lvj_JERDown,         "MWW_JERDown/D"   );       

    outTree_->Branch("MWW_SD_UnclEnUp",       &m_lvj_SD_UnclEnUp,         "MWW_SD_UnclEnUp/D"   );
    outTree_->Branch("MWW_SD_UnclEnDown",       &m_lvj_SD_UnclEnDown,         "MWW_SD_UnclEnDown/D"   );
    outTree_->Branch("MWW_SD_JECUp",       &m_lvj_SD_JECUp,         "MWW_SD_JECUp/D"   );
    outTree_->Branch("MWW_SD_JECDown",       &m_lvj_SD_JECDown,         "MWW_SD_JECDown/D"   );
    outTree_->Branch("MWW_SD_LeptonEnUp",       &m_lvj_SD_LeptonEnUp,         "MWW_SD_LeptonEnUp/D"   );
    outTree_->Branch("MWW_SD_LeptonEnDown",       &m_lvj_SD_LeptonEnDown,         "MWW_SD_LeptonEnDown/D"   );
    outTree_->Branch("MWW_SD_LeptonResUp",       &m_lvj_SD_LeptonResUp,         "MWW_SD_LeptonResUp/D"   );
    outTree_->Branch("MWW_SD_LeptonResDown",       &m_lvj_SD_LeptonResDown,         "MWW_SD_LeptonResDown/D"   );
    outTree_->Branch("MWW_SD_JERUp",       &m_lvj_SD_JERUp,         "MWW_SD_JERUp/D"   );
    outTree_->Branch("MWW_SD_JERDown",       &m_lvj_SD_JERDown,         "MWW_SD_JERDown/D"   );
  }

 if (isSignal) {
  outTree_ -> Branch("aTGCWeights",  &aTGCWeights);
  outTree_ -> Branch("refXsec", &refXsec, "refXsec/D");
  }
  aTGCWeightUnitConv=1;

}


TreeMaker::~TreeMaker()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void
TreeMaker::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;

   nevent = iEvent.eventAuxiliary().event();
   run    = iEvent.eventAuxiliary().run();
   lumi   = iEvent.eventAuxiliary().luminosityBlock();

   edm::Handle< double > rhoH;
   iEvent.getByToken(rhoToken_,rhoH);
   rho_ = *rhoH;
      
   //Leptonic Ws
   edm::Handle<edm::View<reco::Candidate> > leptonicVs;
   iEvent.getByToken(leptonicVToken_, leptonicVs);
   
   //GenParticles
   edm::Handle<edm::View<reco::Candidate> > genParticles;
   if(isMC)iEvent.getByToken(genParticlesToken_, genParticles); 
   
   //Vertices 
   edm::Handle<edm::View<reco::Vertex> > vertices;
   iEvent.getByToken(vertexToken_, vertices);
   
   //Jets
   edm::Handle<edm::View<pat::Jet> > jets; 
   iEvent.getByToken(fatJetsToken_, jets);

   edm::Handle<edm::View<pat::Jet> > jetsSmearedUp;
   if(isMC) iEvent.getByToken(fatJetsSmearedUpToken_, jetsSmearedUp);

   edm::Handle<edm::View<pat::Jet> > jetsSmearedDown;
   if(isMC) iEvent.getByToken(fatJetsSmearedDownToken_, jetsSmearedDown);

   edm::Handle<edm::View<reco::GenJet>> genJetsAK8;
   if(isMC) iEvent.getByToken(genJetsAK8Token_, genJetsAK8);

   //AK4 Jets (for Btag veto )
   edm::Handle<edm::View<pat::Jet> > AK4Jets;
   iEvent.getByToken(AK4JetsToken_, AK4Jets);
   njets = AK4Jets->size();

   njets_JERUp = 0;
   njets_JERDown = 0;
   njets_JECUp = 0;
   njets_JECDown = 0;
   edm::Handle<edm::View<pat::Jet> > AK4JetsSmearedUp, AK4JetsSmearedDown, AK4JetsShiftedUp, AK4JetsShiftedDown;
   if(isMC) {
     iEvent.getByToken(AK4JetsSmearedUpToken_, AK4JetsSmearedUp);
     njets_JERUp = AK4JetsSmearedUp->size();
     iEvent.getByToken(AK4JetsSmearedDownToken_, AK4JetsSmearedDown);
     njets_JERDown = AK4JetsSmearedDown->size();

     iEvent.getByToken(AK4JetsShiftedUpToken_, AK4JetsShiftedUp);
     njets_JECUp = AK4JetsShiftedUp->size();
     iEvent.getByToken(AK4JetsShiftedDownToken_, AK4JetsShiftedDown);
     njets_JECDown = AK4JetsShiftedDown->size();
   }
   
   //MET
   edm::Handle<edm::View<pat::MET> > metHandle;
   iEvent.getByToken(metToken_, metHandle);
   
   //loose electrons
   edm::Handle<edm::View<reco::Candidate> > looseElectrons;
   iEvent.getByToken(looseEleToken_, looseElectrons);
   
   //loose muons
   edm::Handle<edm::View<reco::Candidate> > looseMuons;
   iEvent.getByToken(looseMuToken_, looseMuons); 
   
   //leptons (tight)
   edm::Handle<edm::View<reco::Candidate> > leptons;
   iEvent.getByToken(leptonsToken_, leptons); 

   std::map<std::string, math::XYZTLorentzVector>  SystMap; 
   std::map<std::string, math::XYZTLorentzVector>  LeptonSystMap;
   std::map<std::string, math::XYZTLorentzVector>  MetSystMap;
   if (isMC) {
      SystMap = SystematicsHelper_.getWLepSystematicsLoretzVectors(iEvent);
      LeptonSystMap = SystematicsHelper_.getLeptonSystematicsLoretzVectors(iEvent);
      MetSystMap = SystematicsHelper_.getMetSystematicsLoretzVectors(iEvent);
   }

   nPV = vertices->size();
   
   edm::Handle<std::vector< PileupSummaryInfo > >  PUInfo;
   edm::Handle <GenEventInfoProduct> genInfo; 
   
  if (isMC) {
    iEvent.getByToken(PUInfoToken_, PUInfo);
    std::vector<PileupSummaryInfo>::const_iterator PVI;
    float Tnpv = -1;
    for(PVI = PUInfo->begin(); PVI != PUInfo->end(); ++PVI) {
     int BX = PVI->getBunchCrossing();
     if(BX == 0) { 
       Tnpv = PVI->getTrueNumInteractions();
       continue;
     }

    }
    gnPV=Tnpv;
    PUweight = LumiWeights_.weight( Tnpv );

   iEvent.getByToken( genInfoToken , genInfo);
   genWeight = (genInfo -> weight());
   //btag weights
   if (njets > 0) {
     btagWeight =  BTagHelper_.getEventWeight(AK4Jets);
     if (btagWeight != btagWeight) {
      std::cout << "Event: " << nevent << " LS: " << lumi << std::endl;
      throw std::runtime_error("Inf/NaN btagWeight for event ");
     }
     btagWeight_BTagUp =  BTagHelper_.getEventWeight(AK4Jets, UP, BTAG);
     btagWeight_BTagDown =  BTagHelper_.getEventWeight(AK4Jets, DOWN, BTAG);
     btagWeight_MistagUp =  BTagHelper_.getEventWeight(AK4Jets, UP, MISTAG);
     btagWeight_MistagDown =  BTagHelper_.getEventWeight(AK4Jets, DOWN, MISTAG);
   }
   else {
    btagWeight = 1.;
    btagWeight_BTagUp = 1.;
    btagWeight_BTagDown = 1.;
    btagWeight_MistagUp = 1.;
    btagWeight_MistagDown = 1.;
   }

   // L1 Prefiring weights
   //edm::Handle< double > theprefweight;
   //iEvent.getByToken(prefweight_token, theprefweight ) ;
   //prefiringWeight =(*theprefweight);

   //PDF uncertainties
   edm::Handle<LHEEventProduct> LHEevtProductExternal;
   iEvent.getByToken(LHEEventProductTokenExternal, LHEevtProductExternal);
   range PDFRange = PDFVariationMap.at(NominalPDF);
   if(isSignal) PDFRange = PDFVariationMap.at(NominalPDF/100);
   
   //define number of PDF variations 
   unsigned int NPDFs = PDFRange.high - PDFRange.low + 1;
   PDFWeights.clear();
   ScaleWeights.clear();
   PDFWeights.resize(NPDFs);
   ScaleWeights.resize(9);

   unsigned int iPDF_ID = PDFRange.low;
   //if there are no weights for PDF uncertainties just fill with ones, that's the case for tW single top  sample
   if (LHEevtProductExternal->weights().size() == 0 ) std::fill(PDFWeights.begin(), PDFWeights.end(), 1.);
   for (unsigned int i=0; i<LHEevtProductExternal->weights().size(); i++) {
    if (iPDF_ID > PDFRange.high) break;
    if (LHEevtProductExternal->weights()[i].id == std::to_string(iPDF_ID)){
      unsigned int iPDF = iPDF_ID - PDFRange.low;
      PDFWeights.at(iPDF) = (LHEevtProductExternal->weights()[i].wgt)/LHEevtProductExternal->originalXWGTUP();
      iPDF_ID++;
     }
   }	

   //scale variation uncertainties
   range RangeOfScaleVariation;
   if (NominalPDF == 263000 ) RangeOfScaleVariation = range(1,9);
   else RangeOfScaleVariation = range(1001, 1009);

   //if there are no weights for scale uncertainties just fill with ones, that's the case for tW single top  sample
   if (LHEevtProductExternal->weights().size() == 0 ) std::fill(ScaleWeights.begin(), ScaleWeights.end(), 1.);

   unsigned int iScale_ID = RangeOfScaleVariation.low;
   for (unsigned int i=0; i<LHEevtProductExternal->weights().size(); i++) {
    if (iScale_ID > RangeOfScaleVariation.high) break;
    if (LHEevtProductExternal->weights()[i].id == std::to_string(iScale_ID)){
      unsigned int iScale = iScale_ID - RangeOfScaleVariation.low;
      ScaleWeights.at(iScale) = (LHEevtProductExternal->weights()[i].wgt)/LHEevtProductExternal->originalXWGTUP();
      iScale_ID++;
     }
   }


   if(isSignal){
    aTGCWeights.clear();
    refXsec = LHEevtProductExternal -> originalXWGTUP();
    int weightNumber = 1;
    if( LHEevtProductExternal->weights().size() ) {
      aTGCWeightUnitConv=genWeight/(LHEevtProductExternal->weights().at(0).wgt);
      for ( unsigned int iwgt = 0; iwgt < LHEevtProductExternal->weights().size(); ++iwgt ) {
        const LHEEventProduct::WGT& wgt = LHEevtProductExternal->weights().at(iwgt);
        if( boost::algorithm::contains(wgt.id, "rwgt_" + std::to_string(weightNumber))){
         aTGCWeights.push_back(wgt.wgt);
         weightNumber ++;
       }
      }
    }
   }

   ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<double> > Wplus_p4 = genWLorentzVector(genParticles, 1);
   ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<double> > Wminus_p4 = genWLorentzVector(genParticles, -1);
   Wplus_gen_pt = Wplus_p4.Pt();
   Wplus_gen_eta = Wplus_p4.Eta();
   Wplus_gen_phi = Wplus_p4.Phi();
   Wplus_gen_mass = Wplus_p4.M();

   Wminus_gen_pt = Wminus_p4.Pt();
   Wminus_gen_eta = Wminus_p4.Eta();
   Wminus_gen_phi = Wminus_p4.Phi();
   Wminus_gen_mass = Wminus_p4.M();
  }

  // Store generator level vector boson pT and diboson mass
  genPtV=0;
  genMWV=0;
  if(isMC)
  {
  // Scan the list of gen particles, isolate the starting W or Z, identify the hadronically decaying one and store its pt.
  for(unsigned int iGen=0; iGen<genParticles->size(); ++iGen)
  {
	const reco::Candidate & p= genParticles->at(iGen);
	/*std::cout<<p.pdgId()<<" "<<p.status();
	genHadronicVp4.SetPt(p.pt());
        genHadronicVp4.SetEta(p.eta());
        genHadronicVp4.SetPhi(p.phi());
        genHadronicVp4.SetM(p.mass());
	std::cout<<" "<<genHadronicVp4.px()<<" "<<genHadronicVp4.py()<<" "<<genHadronicVp4.pz()<<" "<<genHadronicVp4.e()<<" "<<genHadronicVp4.M()<<std::endl;*/
	if((fabs(p.pdgId())==23 || fabs(p.pdgId())==24) && p.status()==22)
	{
		if(decaysHadronic(&p))
		{
			genPtV=p.pt();

			genHadronicVp4.SetPt(p.pt());
			genHadronicVp4.SetEta(p.eta());
			genHadronicVp4.SetPhi(p.phi());
			genHadronicVp4.SetM(p.mass());
			//std::cout<<"*Hadronically decaying W* "<<genHadronicVp4.px()<<" "<<genHadronicVp4.py()<<" "<<genHadronicVp4.pz()<<" "<<genHadronicVp4.e()<<" "<<genHadronicVp4.M()<<std::endl;
		}
		else
		{
			genLeptonicVp4.SetPt(p.pt());
			genLeptonicVp4.SetEta(p.eta());
			genLeptonicVp4.SetPhi(p.phi());
			genLeptonicVp4.SetM(p.mass());
			//std::cout<<"*Leptonically decaying W* "<<genLeptonicVp4.px()<<" "<<genLeptonicVp4.py()<<" "<<genLeptonicVp4.pz()<<" "<<genLeptonicVp4.e()<<" "<<genLeptonicVp4.M()<<std::endl;
		}
	}
  }
  genDibosonp4=genHadronicVp4+genLeptonicVp4;
  genMWV=genDibosonp4.M();
  //std::cout<<"Diboson mass: "<<genMWV<<std::endl<<std::endl;
  }

  if (isMC && jecUnc == nullptr) {
    // Only have to initialise once
    edm::ESHandle<JetCorrectorParametersCollection> JetCorParCollAK8;
    iSetup.get<JetCorrectionsRecord>().get("AK8PFchs",JetCorParCollAK8);
    JetCorrectorParameters const & JetCorPar = (*JetCorParCollAK8)["Uncertainty"];
    jecUnc = new JetCorrectionUncertainty(JetCorPar);
  }
  JECunc = 0.;

   //  Defining decay channel on the gen level
   N_had_Wgen  = 0, N_lep_Wgen = 0 ;

   if (isMC) {
    DefineDecayChannel(genParticles, N_lep_Wgen , N_had_Wgen );
    if (N_lep_Wgen == 0 && N_had_Wgen == 2  ) WDecayClass = Hadronic;
    else if (N_lep_Wgen == 1 && N_had_Wgen == 1 ) WDecayClass = Semileptonic;
    else if (N_lep_Wgen == 2 && N_had_Wgen == 0 ) WDecayClass = Leptonic;
    else WDecayClass = UnDefined;
  }

   N_lep_W = leptonicVs -> size();
   
   //loose leptons
   nLooseEle = looseElectrons -> size();
   nLooseMu = looseMuons -> size();
   
   nLep = leptons -> size();
   
  
   //electron channel
   if ( ( leptons -> size() ) > 0)
   {
    auto leptonPtr = leptons -> ptrAt(0);
    reco::MuonPtr asmuonPtr(leptonPtr);
    reco::ElectronPtr aselectronPtr(leptonPtr);
     if (channel == "mu"){
      Lepton.pt =  asmuonPtr -> tunePMuonBestTrack() -> pt();
      Lepton.eta = asmuonPtr -> tunePMuonBestTrack() -> eta();
      Lepton.phi = asmuonPtr -> tunePMuonBestTrack() -> phi();
    }
    else if (channel == "el"){
      Lepton.pt = (leptons -> at(0)).pt();
      Lepton.eta = (leptons -> at(0)).eta();
      Lepton.phi = (leptons -> at(0)).phi();
      reco::SuperClusterRef  superCluster = aselectronPtr->superCluster();
      sc_eta = superCluster->eta();
      sc_et = (superCluster -> energy())* sin((aselectronPtr->superClusterPosition()).theta());
      const reco::CaloClusterPtr& seed = aselectronPtr -> superCluster()->seed();
      isEB = ( seed->seed().subdetId() == EcalBarrel );
    }
    if (isMC){
       Lepton.pt_LeptonEnUp = LeptonSystMap.at("LeptonEnUp").Pt();
       Lepton.pt_LeptonEnDown = LeptonSystMap.at("LeptonEnDown").Pt();
       Lepton.pt_LeptonResUp = LeptonSystMap.at("LeptonResUp").Pt();
       Lepton.pt_LeptonResDown = LeptonSystMap.at("LeptonResDown").Pt();
    }
   }
   else
   {
     Lepton.pt = -99.;
     Lepton.eta = -99.;
     Lepton.phi = -99.;
     Lepton.pt_LeptonEnUp = -99.;
     Lepton.pt_LeptonEnDown = -99.;
     Lepton.pt_LeptonResUp = -99.;
     Lepton.pt_LeptonResDown = -99.;
   }

   if (channel == "mu"){
      // Muon scale factors apply cuts on their nVtx
      int goodNPV = 0;
      for (const auto & itr : *vertices) {
        if (fabs(itr.z()) <= 25 && itr.ndof() > 4 && fabs(itr.position().rho()) <= 2 && !itr.isFake()) {
          goodNPV++;
        }
      }

      LeptonSF = MuonScaleFactor_.getScaleFactor(Lepton.pt, std::abs(Lepton.eta), Lepton.phi, goodNPV);
      LeptonSF_Up = MuonScaleFactor_.getScaleFactor(Lepton.pt, std::abs(Lepton.eta), Lepton.phi, goodNPV, "up");
      LeptonSF_Down = MuonScaleFactor_.getScaleFactor(Lepton.pt, std::abs(Lepton.eta), Lepton.phi, goodNPV, "down");
    }
   else if (channel == "el") {
      LeptonSF = ElectronScaleFactor_.getScaleFactor(Lepton.pt, Lepton.eta, sc_eta, Lepton.phi, nPV);
      LeptonSF_Up = ElectronScaleFactor_.getScaleFactor(Lepton.pt, Lepton.eta, sc_eta, Lepton.phi, nPV, "up");
      LeptonSF_Down = ElectronScaleFactor_.getScaleFactor(Lepton.pt, Lepton.eta, sc_eta, Lepton.phi, nPV, "down");
    }
   //leptonically decaying W
   if (leptonicVs -> size() > 0)
   {
      const reco::Candidate  & leptonicV = leptonicVs->at(0);   
      Wboson_lep.pt = leptonicV.pt();
      Wboson_lep.eta = leptonicV.eta();
      Wboson_lep.phi = leptonicV.phi();
      Wboson_lep.mass = leptonicV.mass();
      Wboson_lep.mt = leptonicV.mt();
      Wboson_lep.charge = leptonicV.charge();
      if (isMC){
        //pt
        Wboson_lep.pt_LeptonEnUp = SystMap.at("LeptonEnUp").Pt();
        Wboson_lep.pt_LeptonEnDown = SystMap.at("LeptonEnDown").Pt();
        Wboson_lep.pt_LeptonResUp = SystMap.at("LeptonResUp").Pt();
        Wboson_lep.pt_LeptonResDown = SystMap.at("LeptonResDown").Pt();
        Wboson_lep.pt_JECUp = SystMap.at("JetEnUp").Pt();
        Wboson_lep.pt_JECDown = SystMap.at("JetEnDown").Pt();
        Wboson_lep.pt_UnclEnUp = SystMap.at("UnclusteredEnUp").Pt();
        Wboson_lep.pt_UnclEnDown = SystMap.at("UnclusteredEnDown").Pt();

        //mass
        Wboson_lep.mass_LeptonEnUp = SystMap.at("LeptonEnUp").M();
        Wboson_lep.mass_LeptonEnDown = SystMap.at("LeptonEnDown").M();
        Wboson_lep.mass_LeptonResUp = SystMap.at("LeptonResUp").M();
        Wboson_lep.mass_LeptonResDown = SystMap.at("LeptonResDown").M();
        Wboson_lep.mass_JECUp = SystMap.at("JetEnUp").M();
        Wboson_lep.mass_JECDown = SystMap.at("JetEnDown").M();
        Wboson_lep.mass_UnclEnUp = SystMap.at("UnclusteredEnUp").M();
        Wboson_lep.mass_UnclEnDown = SystMap.at("UnclusteredEnDown").M();


        //mt
        Wboson_lep.mt_LeptonEnUp = SystMap.at("LeptonEnUp").Mt();
        Wboson_lep.mt_LeptonEnDown = SystMap.at("LeptonEnDown").Mt();
        Wboson_lep.mt_LeptonResUp = SystMap.at("LeptonResUp").Mt();
        Wboson_lep.mt_LeptonResDown = SystMap.at("LeptonResDown").Mt();
        Wboson_lep.mt_JECUp = SystMap.at("JetEnUp").Mt();
        Wboson_lep.mt_JECDown = SystMap.at("JetEnDown").Mt();
        Wboson_lep.mt_UnclEnUp = SystMap.at("UnclusteredEnUp").Mt();
        Wboson_lep.mt_UnclEnDown = SystMap.at("UnclusteredEnDown").Mt();

      }
  }
  
  else 
   {
      Wboson_lep.pt = -99.;
      Wboson_lep.eta = -99.;
      Wboson_lep.phi = -99.;
      Wboson_lep.mass = -99.;
      Wboson_lep.mt = -99.;
      Wboson_lep.charge = -99.;
      if (isMC){
        //pt
        Wboson_lep.pt_LeptonEnUp = -99.;
        Wboson_lep.pt_LeptonEnDown = -99.;
        Wboson_lep.pt_LeptonResUp = -99.;
        Wboson_lep.pt_LeptonResDown = -99.;
        Wboson_lep.pt_JECUp = -99.;
        Wboson_lep.pt_JECDown = -99.;
        Wboson_lep.pt_UnclEnUp = -99.;
        Wboson_lep.pt_UnclEnDown = -99.;

        //mass
        Wboson_lep.mass_LeptonEnUp = -99.;
        Wboson_lep.mass_LeptonEnDown = -99.;
        Wboson_lep.mass_LeptonResUp = -99.;
        Wboson_lep.mass_LeptonResDown = -99.;
        Wboson_lep.mass_JECUp = -99.;
        Wboson_lep.mass_JECDown = -99.;
        Wboson_lep.mass_UnclEnUp = -99.;
        Wboson_lep.mass_UnclEnDown = -99.;

        //mt 

        Wboson_lep.mt_LeptonEnUp = -99.;
        Wboson_lep.mt_LeptonEnDown = -99.;
        Wboson_lep.mt_LeptonResUp = -99.;
        Wboson_lep.mt_LeptonResDown = -99.;
        Wboson_lep.mt_JECUp = -99.;
        Wboson_lep.mt_JECDown = -99.;
        Wboson_lep.mt_UnclEnUp = -99.;
        Wboson_lep.mt_UnclEnDown = -99.;


      }
  }
  
    //MET quantities
   if (metHandle->size() > 0)
   {
      const pat::MET& metCand = metHandle->at(0);

      METCand.pt = metCand.pt();
      METCand.phi = metCand.phi();

      //MET uncertainties
      if (isMC){
        // MET uncertainties
        MET_LeptonEnUp = MetSystMap.at("LeptonEnUp").Pt();
        MET_LeptonEnDown = MetSystMap.at("LeptonEnDown").Pt();
        MET_JECUp = MetSystMap.at("JetEnUp").Pt();
        MET_JECDown = MetSystMap.at("JetEnDown").Pt();
        MET_JERUp = MetSystMap.at("JetResUp").Pt();
        MET_JERDown = MetSystMap.at("JetResDown").Pt();
        MET_UnclEnUp = MetSystMap.at("UnclusteredEnUp").Pt();
        MET_UnclEnDown = MetSystMap.at("UnclusteredEnDown").Pt();

        //MET phi uncertainties
        MET_Phi_LeptonEnUp = MetSystMap.at("LeptonEnUp").Phi();
        MET_Phi_LeptonEnDown = MetSystMap.at("LeptonEnDown").Phi();
        MET_Phi_JECUp = MetSystMap.at("JetEnUp").Phi();
        MET_Phi_JECDown = MetSystMap.at("JetEnDown").Phi();
        MET_Phi_JERUp = MetSystMap.at("JetResUp").Phi();
        MET_Phi_JERDown = MetSystMap.at("JetResDown").Phi();
        MET_Phi_UnclEnUp = MetSystMap.at("UnclusteredEnUp").Phi();
        MET_Phi_UnclEnDown = MetSystMap.at("UnclusteredEnDown").Phi();
      }
   }
   else
   {
      METCand.pt = -99.;
      METCand.phi = -99.;

      //MET uncertainties
       //METUncl
      MET_UnclEnUp = -99. ;
      MET_UnclEnUp = -99.;
      //JER
      MET_JERUp = -99.;
      MET_JERDown = -99. ;
      //JEC
      MET_JECUp = -99. ;
      MET_JECDown = -99. ;
      //Lepton energy 
      MET_LeptonEnUp = -99.;
      MET_LeptonEnDown = -99. ;

      //MET phi uncertainties
       //METUncl
      MET_Phi_UnclEnUp = -99. ;
      MET_Phi_UnclEnUp = -99.;
      //JER
      MET_Phi_JERUp = -99.;
      MET_Phi_JERDown = -99. ;
      //JEC
      MET_Phi_JECUp = -99. ;
      MET_Phi_JECDown = -99. ;
      //Lepton energy 
      MET_Phi_LeptonEnUp = -99.;
      MET_Phi_LeptonEnDown = -99. ;
      
   }
   
    if (jets -> size() > 0 && leptonicVs -> size() > 0)
    {
      deltaR_LeptonWJet = deltaR(Lepton.eta,Lepton.phi,(jets -> at(0)).eta(), (jets -> at(0)).phi()); 
      deltaPhi_LeptonMet = deltaPhi(Lepton.phi, METCand.phi);
      deltaPhi_WJetMet = deltaPhi(jets->at(0).phi(), METCand.phi);
      deltaPhi_WJetWlep = deltaPhi(jets->at(0).phi(), Wboson_lep.phi);
      if (isMC){
        //Unclustered energy
        deltaPhi_LeptonMet_UnclEnUp = deltaPhi(Lepton.phi, MET_Phi_UnclEnUp);
        deltaPhi_LeptonMet_UnclEnDown = deltaPhi(Lepton.phi, MET_Phi_UnclEnDown);
        //JEC
        deltaPhi_LeptonMet_JECUp = deltaPhi(Lepton.phi, MET_Phi_JECUp);
        deltaPhi_LeptonMet_JECDown = deltaPhi(Lepton.phi, MET_Phi_JECDown);
        //lepton energy scale
        deltaPhi_LeptonMet_LeptonEnUp = deltaPhi(Lepton.phi, MET_Phi_LeptonEnUp);
        deltaPhi_LeptonMet_LeptonEnDown = deltaPhi(Lepton.phi, MET_Phi_LeptonEnDown);
        //JER
        deltaPhi_LeptonMet_JERUp = deltaPhi(Lepton.phi, MET_Phi_JERUp);
        deltaPhi_LeptonMet_JERDown = deltaPhi(Lepton.phi, MET_Phi_JERDown);

        //////////////////////
        //Unclustered energy
        deltaPhi_WJetMet_UnclEnUp = deltaPhi(jets->at(0).phi(), MET_Phi_UnclEnUp);
        deltaPhi_WJetMet_UnclEnDown = deltaPhi(jets->at(0).phi(), MET_Phi_UnclEnDown);
        //JEC
        deltaPhi_WJetMet_JECUp = deltaPhi(jets->at(0).phi(), MET_Phi_JECUp);
        deltaPhi_WJetMet_JECDown = deltaPhi(jets->at(0).phi(), MET_Phi_JECDown);
        //lepton energy scale
        deltaPhi_WJetMet_LeptonEnUp = deltaPhi(jets->at(0).phi(), MET_Phi_LeptonEnUp);
        deltaPhi_WJetMet_LeptonEnDown = deltaPhi(jets->at(0).phi(), MET_Phi_LeptonEnDown);
        //JER
        deltaPhi_WJetMet_JERUp = deltaPhi(jets->at(0).phi(), MET_Phi_JERUp);
        deltaPhi_WJetMet_JERDown = deltaPhi(jets->at(0).phi(), MET_Phi_JERDown);

        //////////////////////
        //Unclustered energy
        deltaPhi_WJetWlep_UnclEnUp = deltaPhi(jets->at(0).phi(), SystMap.at("UnclusteredEnUp").Phi());
        deltaPhi_WJetWlep_UnclEnDown = deltaPhi(jets->at(0).phi(), SystMap.at("UnclusteredEnDown").Phi());
        //JEC
        deltaPhi_WJetWlep_JECUp = deltaPhi(jets->at(0).phi(), SystMap.at("JetEnUp").Phi());
        deltaPhi_WJetWlep_JECDown = deltaPhi(jets->at(0).phi(), SystMap.at("JetEnDown").Phi());
        //lepton energy scale
        deltaPhi_WJetWlep_LeptonEnUp = deltaPhi(jets->at(0).phi(), SystMap.at("LeptonEnUp").Phi());
        deltaPhi_WJetWlep_LeptonEnDown = deltaPhi(jets->at(0).phi(), SystMap.at("LeptonEnDown").Phi());


      }
    }
    else 
    {
      deltaR_LeptonWJet = -99.; 
      deltaPhi_LeptonMet = -99.;
      deltaPhi_WJetMet = -99.;
      deltaPhi_WJetWlep = -99.;

      if (isMC){
      //Unclustered energy
        deltaPhi_LeptonMet_UnclEnUp = -99.;
        deltaPhi_LeptonMet_UnclEnDown = -99.;
        //JEC
        deltaPhi_LeptonMet_JECUp = -99.;
        deltaPhi_LeptonMet_JECDown = -99.;
        //lepton energy scale
        deltaPhi_LeptonMet_LeptonEnUp = -99.;
        deltaPhi_LeptonMet_LeptonEnDown = -99.;
        //JER
        deltaPhi_LeptonMet_JERUp = -99.;
        deltaPhi_LeptonMet_JERDown = -99.;
        /////////////
         //Unclustered energy
        deltaPhi_WJetMet_UnclEnUp = -99.;
        deltaPhi_WJetMet_UnclEnDown = -99.;
        //JEC
        deltaPhi_WJetMet_JECUp = -99.;
        deltaPhi_WJetMet_JECDown = -99.;
        //lepton energy scale
        deltaPhi_WJetMet_LeptonEnUp = -99.;
        deltaPhi_WJetMet_LeptonEnDown = -99.;
        //JER
        deltaPhi_WJetMet_JERUp = -99.;
        deltaPhi_WJetMet_JERDown = -99.;

         //////////////////////
        //Unclustered energy
        deltaPhi_WJetWlep_UnclEnUp = -99.;
        deltaPhi_WJetWlep_UnclEnDown = -99.;
        //JEC
        deltaPhi_WJetWlep_JECUp = -99.;
        deltaPhi_WJetWlep_JECDown = -99.;
        //lepton energy scale
        deltaPhi_WJetWlep_LeptonEnUp = -99.;
        deltaPhi_WJetWlep_LeptonEnDown = -99.;
      }

    }


  NAK8jet = jets -> size();
  if(isMC) NAK8jet_smearedUp = jetsSmearedUp -> size();
  if(isMC) NAK8jet_smearedDown = jetsSmearedDown -> size();
  JetResolutionSmearer_.setRhoAndSeed(rho_, iEvent);

  // Different types because the pat::Jet returns math::XYZTLorentzVector (uses E not M)
  // but for the SD ones we need to modify the mass, so need the M4D not E4D
  math::XYZTLorentzVector smearedJetUp, smearedJetDown;
  ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<double> > smearedJetUp_SD, smearedJetDown_SD;

   if (jets -> size() > 0)
  {
    const pat::Jet & fatJet = jets->at(0);
    jet_tau2tau1 = fatJet.userFloat("NjettinessAK8:tau2")/fatJet.userFloat("NjettinessAK8:tau1");
    jet_tau3tau2 = fatJet.userFloat("NjettinessAK8:tau3")/fatJet.userFloat("NjettinessAK8:tau2");
    jet_tau1 = fatJet.userFloat("NjettinessAK8:tau1");
    jet_tau2 = fatJet.userFloat("NjettinessAK8:tau2");
    jet_tau3 = fatJet.userFloat("NjettinessAK8:tau3");
    // Need to manually apply correction factor to userFloat values - calculate by using
    // ratio of corrected PT to uncorrected pt
    double corr = fatJet.correctedP4(fatJet.currentJECLevel()).pt() / fatJet.correctedP4("Uncorrected").pt();
    jet_mass_pruned = corr * fatJet.userFloat("ak8PFJetsCHSPrunedMass");
    jet_mass_softdrop = corr * fatJet.userFloat("ak8PFJetsCHSSoftDropMass");

    //https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookMiniAOD2016#Jets
    // Do these require correction factor?
    jet_pt_PUPPI = fatJet.userFloat("ak8PFJetsPuppiValueMap:pt");
    jet_eta_PUPPI = fatJet.userFloat("ak8PFJetsPuppiValueMap:eta");
    jet_phi_PUPPI = fatJet.userFloat("ak8PFJetsPuppiValueMap:phi");
    jet_mass_PUPPI = fatJet.userFloat("ak8PFJetsPuppiValueMap:mass");
    jet_tau1_PUPPI = fatJet.userFloat("ak8PFJetsPuppiValueMap:NjettinessAK8PuppiTau1");
    jet_tau2_PUPPI = fatJet.userFloat("ak8PFJetsPuppiValueMap:NjettinessAK8PuppiTau2");
    jet_tau3_PUPPI = fatJet.userFloat("ak8PFJetsPuppiValueMap:NjettinessAK8PuppiTau3");
    jet_tau21_PUPPI = jet_tau2_PUPPI/jet_tau1_PUPPI;
    jet_tau32_PUPPI = jet_tau3_PUPPI/jet_tau2_PUPPI;


    TLorentzVector puppi_softdrop,puppi_softdrop_subjet;
    auto const & sdSubjetsPuppi = fatJet.subjets("SoftDropPuppi");
    for ( auto const & it : sdSubjetsPuppi )
    {
      puppi_softdrop_subjet.SetPtEtaPhiM(it->correctedP4(0).pt(),it->correctedP4(0).eta(),it->correctedP4(0).phi(),it->correctedP4(0).mass());
      puppi_softdrop+=puppi_softdrop_subjet;
    }

    float puppiCorr= getPUPPIweight( jet_pt_PUPPI, jet_eta_PUPPI );
    jet_mass_softdrop_PUPPI = puppi_softdrop.M() * puppiCorr;
    jet_tau21_DT = jet_tau21_PUPPI + 0.063*std::log(jet_pt_PUPPI*jet_pt_PUPPI/jet_mass_PUPPI);

    jet_pt = fatJet.pt();
    jet_eta = fatJet.eta();
    jet_phi = fatJet.phi();
    jet_mass = fatJet.mass();

    if(isMC)
    {
      isMatched_ = isMatchedToGenW(genParticles, fatJet);

      //JEC uncertainty
      jecUnc->setJetEta(jet_eta);
      jecUnc->setJetPt(jet_pt);  // must be corrected PT
      JECunc = jecUnc->getUncertainty(true);

      jet_pt_JECDown = (1 - JECunc)*jet_pt;
      jet_pt_JECUp   = (1 + JECunc)*jet_pt;
      jet_mass_JECDown = (1 - JECunc)*jet_mass;
      jet_mass_JECUp   = (1 + JECunc)*jet_mass;
      jet_mass_pruned_JECDown = (1 - JECunc)*jet_mass_pruned;
      jet_mass_pruned_JECUp = (1 + JECunc)*jet_mass_pruned;
      jet_mass_softdrop_JECDown = (1 - JECunc)*jet_mass_softdrop;
      jet_mass_softdrop_JECUp = (1 + JECunc)*jet_mass_softdrop;
      jet_mass_softdrop_PUPPI_JECDown = (1 - JECunc)*jet_mass_softdrop_PUPPI;
      jet_mass_softdrop_PUPPI_JECUp = (1 + JECunc)*jet_mass_softdrop_PUPPI;

      // Numbers taken from JetWTagging twiki
      float mSDSF = 1.0;
      float mSDSFUnc = 0.2;
      float mSDResolutionAbs = 10.1;
      float mSDResolutionRel = mSDResolutionAbs / 80.; // FIXME! Need better number than 80

      //JER uncertainty
      if (jetsSmearedUp->size() > 0) {
        const pat::Jet & fatJetUp = jetsSmearedUp->at(0);
        jet_pt_JERUp = fatJetUp.pt();
        jet_mass_JERUp = fatJetUp.mass();
        double JERUpCorrection = fatJetUp.pt()/jet_pt;
        jet_mass_pruned_JERUp = JERUpCorrection*jet_mass_pruned;
        jet_mass_softdrop_JERUp = JERUpCorrection*jet_mass_softdrop;
        // For PUPPI SD mass, we treat it separately using official JMR SF & unc, and resolution.
        // We don't have a gen level mass, so we'll use pT to calculate the factor for mass
        // see https://twiki.cern.ch/twiki/bin/viewauth/CMS/JetWtagging
        float c = getSmearingFactor(mSDSF, mSDSFUnc, mSDResolutionRel, fatJet, *genJetsAK8, 1, 0.4, 99999., true);
        jet_mass_softdrop_PUPPI_JERUp = c*jet_mass_softdrop_PUPPI;
        smearedJetUp = fatJetUp.p4();
        smearedJetUp_SD.SetPt(fatJetUp.pt());
        smearedJetUp_SD.SetEta(fatJetUp.eta());
        smearedJetUp_SD.SetPhi(fatJetUp.phi());
        smearedJetUp_SD.SetM(jet_mass_softdrop_JERUp);
      } else {
        jet_pt_JERUp = -99;
        jet_mass_JERUp = -99;
        jet_mass_pruned_JERUp = -99;
        jet_mass_softdrop_JERUp = -99;
        jet_mass_softdrop_PUPPI_JERUp = -99;
      }

      if (jetsSmearedDown->size() > 0) {
        const pat::Jet & fatJetDown = jetsSmearedDown->at(0);
        jet_pt_JERDown = fatJetDown.pt();
        jet_mass_JERDown = fatJetDown.mass();
        double JERDownCorrection = fatJetDown.pt()/jet_pt;
        jet_mass_pruned_JERDown = JERDownCorrection*jet_mass_pruned;
        jet_mass_softdrop_JERDown = JERDownCorrection*jet_mass_softdrop;
        float c = getSmearingFactor(mSDSF, mSDSFUnc, mSDResolutionRel, fatJet, *genJetsAK8, -1, 0.4, 99999., true);
        jet_mass_softdrop_PUPPI_JERDown = c*jet_mass_softdrop_PUPPI;
        smearedJetDown = fatJetDown.p4();
        smearedJetDown_SD.SetPt(fatJetDown.pt());
        smearedJetDown_SD.SetEta(fatJetDown.eta());
        smearedJetDown_SD.SetPhi(fatJetDown.phi());
        smearedJetDown_SD.SetM(jet_mass_softdrop_JERDown);
      } else {
        jet_pt_JERDown = -99;
        jet_mass_JERDown = -99;
        jet_mass_pruned_JERDown = -99;
        jet_mass_softdrop_JERDown = -99;
        jet_mass_softdrop_PUPPI_JERDown = -99;
      }

    }
  }
  
  else throw cms::Exception("InvalidValue") << "This shouldn't happen, we require at least 1 jet, but the size of the jet collection for this event is zero!" << std::endl; 
  
  //Loop over the collection of the AK4 jets which contain b-tagging information (to veto b-jets)
  if(isMC){
    jetFlavours.clear();

    BgenjetStatus43_pt.clear();
    BgenjetStatus43_eta.clear();
    BgenjetStatus43_phi.clear();
    BgenjetStatus43_mass.clear();
    BgenjetStatus43_motherPDGID.clear();


    BgenjetStatus21_pt.clear();
    BgenjetStatus21_eta.clear();
    BgenjetStatus21_phi.clear();
    BgenjetStatus21_mass.clear();
    BgenjetStatus21_motherPDGID.clear();
  
    for (unsigned int iGen = 0; iGen < genParticles-> size() && isMC; ++iGen)
    {
      if (std::abs((genParticles -> at(iGen)).pdgId()) == 5 ){
      if ((genParticles -> at(iGen)).status() == 21){
        BgenjetStatus21_pt.push_back((genParticles -> at(iGen)).pt());
        BgenjetStatus21_eta.push_back((genParticles -> at(iGen)).eta());
        BgenjetStatus21_phi.push_back((genParticles -> at(iGen)).phi());
        BgenjetStatus21_mass.push_back((genParticles -> at(iGen)).mass());
        BgenjetStatus21_motherPDGID.push_back((genParticles -> at(iGen)).mother()->pdgId());
      }
      if ((genParticles -> at(iGen)).status() == 43){
        BgenjetStatus43_pt.push_back((genParticles -> at(iGen)).pt());
        BgenjetStatus43_eta.push_back((genParticles -> at(iGen)).eta());
        BgenjetStatus43_phi.push_back((genParticles -> at(iGen)).phi());
        BgenjetStatus43_mass.push_back((genParticles -> at(iGen)).mass());
        BgenjetStatus43_motherPDGID.push_back((genParticles -> at(iGen)).mother()->pdgId());
      }
     }
    }
  }
  // Count number of btagged AK4 jets
  nbtag = 0;
  nbtagMedium = 0;
  nbtagLoose = 0;
  nbtag_JECUp = 0;
  nbtag_JECDown = 0;
  nbtag_JERUp = 0;
  nbtag_JERDown = 0;
  for (const auto & itr : *AK4Jets)
  {
    //taken from: https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation80X#Supported_Algorithms_and_Operati
    if((itr.bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags")) > bTagDiscrCut){
      nbtag ++;
    }
    if((itr.bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags")) > bTagDiscrCutMedium){
      nbtagMedium ++;
    }
    if((itr.bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags")) > bTagDiscrCutLoose){
      nbtagLoose ++;
    }
    // std::cout << "Nominal " << itr.pt() << " : " << itr.eta() << " : " << itr.bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags") << std::endl;
    if(isMC) jetFlavours.push_back(itr.partonFlavour());
  }

  if (isMC) {
    for (const auto & itr : *AK4JetsSmearedUp) {
      if (itr.bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags") > bTagDiscrCut) {
        nbtag_JERUp++;
      }
      // std::cout << "Smeared Up " << itr.pt() << " : " << itr.eta() << " : " << itr.bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags") << std::endl;
    }

    for (const auto & itr : *AK4JetsSmearedDown) {
      if (itr.bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags") > bTagDiscrCut) {
        nbtag_JERDown++;
      }
      // std::cout << "Smeared Down " << itr.pt() << " : " << itr.eta() << " : " << itr.bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags") << std::endl;
    }

    for (const auto & itr : *AK4JetsShiftedUp) {
      if (itr.bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags") > bTagDiscrCut) {
        nbtag_JECUp++;
      }
      // std::cout << "Shifted Up " << itr.pt() << " : " << itr.eta() << " : " << itr.bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags") << std::endl;
    }

    for (const auto & itr : *AK4JetsShiftedDown) {
      if (itr.bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags") > bTagDiscrCut) {
        nbtag_JECDown++;
      }
      // std::cout << "Shifted Down " << itr.pt() << " : " << itr.eta() << " : " << itr.bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags") << std::endl;
    }
  }
 
  if (AK4Jets -> size() > 0)
  {
    jet2_pt = (AK4Jets -> at(0)).pt();
    jet2_eta = (AK4Jets -> at(0)).phi();
    jet2_phi = (AK4Jets -> at(0)).eta();
    jet2_btag = (AK4Jets -> at(0)).bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags");
  }
  
  else 
  {
    jet2_pt = -99.;
    jet2_eta = -99.;
    jet2_phi = -99.;
    jet2_btag = -99.;
  }
  
  if (AK4Jets -> size() > 1)
  {
    jet3_pt = (AK4Jets -> at(1)).pt();
    jet3_eta = (AK4Jets -> at(1)).eta();
    jet3_phi = (AK4Jets -> at(1)).phi();
    jet3_btag = (AK4Jets -> at(1)).bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags");
  } 
  else 
  {
    jet3_pt = -99.;
    jet3_eta = -99.;
    jet3_phi = -99.;
    jet3_btag = -99.;
  }
  
  //diboson mass
   ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<double> > hadronicVp4, hadronicVp4_SD, leptonicVp4, lvj_p4, lvj_p4_SD;
   //hadronic V
   hadronicVp4.SetPt(jet_pt);
   hadronicVp4.SetEta(jet_eta);
   hadronicVp4.SetPhi(jet_phi);
   hadronicVp4.SetM(jet_mass);

   // hadronic V but with puppi SD mass
   hadronicVp4_SD.SetPt(jet_pt);
   hadronicVp4_SD.SetEta(jet_eta);
   hadronicVp4_SD.SetPhi(jet_phi);
   hadronicVp4_SD.SetM(jet_mass_softdrop_PUPPI);

   //std::cout<<"*Hadronically decaying W* "<<hadronicVp4.px()<<" "<<hadronicVp4.py()<<" "<<hadronicVp4.pz()<<" "<<hadronicVp4.e()<<" "<<hadronicVp4.M()<<std::endl;

   //leptonic W
   leptonicVp4.SetPt(Wboson_lep.pt);
   leptonicVp4.SetEta(Wboson_lep.eta);
   leptonicVp4.SetPhi(Wboson_lep.phi);
   leptonicVp4.SetM(Wboson_lep.mass);
   //std::cout<<"*Leptonically decaying W* "<<leptonicVp4.px()<<" "<<leptonicVp4.py()<<" "<<leptonicVp4.pz()<<" "<<leptonicVp4.e()<<" "<<leptonicVp4.M()<<std::endl;
   
   if (leptonicVs -> size() > 0) {
     saveDibosonMass(leptonicVp4, hadronicVp4, m_lvj);
     saveDibosonMass(leptonicVp4, hadronicVp4_SD, m_lvj_SD);
   } else {
     m_lvj = -99.;
     m_lvj_SD = -99.;
   }
   //std::cout<<"Diboson mass: "<<m_lvj<<std::endl<<std::endl;
   //std::cout<<"---------------------------------------"<<std::endl;
   //systematics
   if (isMC) {
     m_lvj_UnclEnUp = -99.;
     m_lvj_UnclEnDown = -99.;

     m_lvj_JECUp = -99.;
     m_lvj_JECDown = -99.;

     m_lvj_LeptonEnUp = -99.;
     m_lvj_LeptonEnDown = -99.;

     m_lvj_LeptonResUp = -99.;
     m_lvj_LeptonResDown = -99.;

     m_lvj_JERUp = -99.;
     m_lvj_JERDown = -99.;

     // SD versions
     m_lvj_SD_UnclEnUp = -99.;
     m_lvj_SD_UnclEnDown = -99.;

     m_lvj_SD_JECUp = -99.;
     m_lvj_SD_JECDown = -99.;

     m_lvj_SD_LeptonEnUp = -99.;
     m_lvj_SD_LeptonEnDown = -99.;

     m_lvj_SD_LeptonResUp = -99.;
     m_lvj_SD_LeptonResDown = -99.;

     m_lvj_SD_JERUp = -99.;
     m_lvj_SD_JERDown = -99.;

     if (leptonicVs -> size() > 0) {
       //METUnclEn
       saveDibosonMass(SystMap.at("UnclusteredEnUp"), hadronicVp4, m_lvj_UnclEnUp);
       saveDibosonMass(SystMap.at("UnclusteredEnDown"), hadronicVp4, m_lvj_UnclEnDown);

       saveDibosonMass(SystMap.at("UnclusteredEnUp"), hadronicVp4_SD, m_lvj_SD_UnclEnUp);
       saveDibosonMass(SystMap.at("UnclusteredEnDown"), hadronicVp4_SD, m_lvj_SD_UnclEnDown);

       //JEC
       saveDibosonMass(SystMap.at("JetEnUp"), hadronicVp4*(1+JECunc), m_lvj_JECUp);
       saveDibosonMass(SystMap.at("JetEnDown"), hadronicVp4*(1+JECunc), m_lvj_JECDown);

       saveDibosonMass(SystMap.at("JetEnUp"), hadronicVp4_SD*(1+JECunc), m_lvj_SD_JECUp);
       saveDibosonMass(SystMap.at("JetEnDown"), hadronicVp4_SD*(1+JECunc), m_lvj_SD_JECDown);

       //lepton energy scale uncertainty
       saveDibosonMass(SystMap.at("LeptonEnUp"), hadronicVp4, m_lvj_LeptonEnUp);
       saveDibosonMass(SystMap.at("LeptonEnDown"), hadronicVp4, m_lvj_LeptonEnDown);

       saveDibosonMass(SystMap.at("LeptonEnUp"), hadronicVp4_SD, m_lvj_SD_LeptonEnUp);
       saveDibosonMass(SystMap.at("LeptonEnDown"), hadronicVp4_SD, m_lvj_SD_LeptonEnDown);

       //lepton energy resolution uncertainty
       saveDibosonMass(SystMap.at("LeptonResUp"), hadronicVp4, m_lvj_LeptonResUp);
       saveDibosonMass(SystMap.at("LeptonResDown"), hadronicVp4, m_lvj_LeptonResDown);

       saveDibosonMass(SystMap.at("LeptonResUp"), hadronicVp4_SD, m_lvj_SD_LeptonResUp);
       saveDibosonMass(SystMap.at("LeptonResDown"), hadronicVp4_SD, m_lvj_SD_LeptonResDown);
     }

     //jet energy resolution uncertainty
     if (leptonicVs -> size() > 0 && jetsSmearedUp -> size() > 0) {
       saveDibosonMass(SystMap.at("JetResUp"), smearedJetUp, m_lvj_JERUp);
       saveDibosonMass(SystMap.at("JetResUp"), smearedJetUp_SD, m_lvj_SD_JERUp);
     }
     if (leptonicVs -> size() > 0 && jetsSmearedUp -> size() > 0) {
       saveDibosonMass(SystMap.at("JetResDown"), smearedJetDown, m_lvj_JERDown);
       saveDibosonMass(SystMap.at("JetResDown"), smearedJetDown_SD, m_lvj_SD_JERDown);
     }
   }

  edm::Handle<edm::TriggerResults> Triggers;
  if (channel == "el") {
    iEvent.getByToken(TriggerResultsToken, Triggers);
    edm::TriggerNames names = iEvent.triggerNames(*Triggers);
    for (unsigned int iTrig = 0; iTrig < Triggers -> size(); iTrig ++)
    {
      if( boost::algorithm::contains(names.triggerName(iTrig), "HLT_Ele27_WPTight_Gsf_v") )  bit_HLT_Ele_27_tight =  Triggers -> accept(iTrig);
      if( boost::algorithm::contains(names.triggerName(iTrig), "HLT_Ele45_WPLoose_Gsf_v") )  bit_HLT_Ele_45 =  Triggers -> accept(iTrig);
      if( boost::algorithm::contains(names.triggerName(iTrig), "HLT_Ele115_CaloIdVT_GsfTrkIdT_v") )  bit_HLT_Ele_115 =  Triggers -> accept(iTrig);
      if( boost::algorithm::contains(names.triggerName(iTrig), "HLT_Photon175_v") )  bit_HLT_Photon_175 =  Triggers -> accept(iTrig);
      bit_HLT_Ele_27_OR_45_OR_115 = bit_HLT_Ele_27_tight || bit_HLT_Ele_45 || bit_HLT_Ele_115;
    }
  }

  topPtSF=1.;
  bool topInGen=0, antitopInGen=0;
  double topSF=1., antitopSF=1.;
  if(isMC)
  {
        for(unsigned int iGen=0; iGen<genParticles->size(); ++iGen)
                if((genParticles->at(iGen)).pdgId()==6 && (genParticles->at(iGen)).status()==22)
		{
			topInGen=1;
			topSF=std::exp(0.0615-(0.0005*(genParticles->at(iGen)).pt()));
		}
                else if((genParticles->at(iGen)).pdgId()==-6 && (genParticles->at(iGen)).status()==22)
		{
			antitopInGen=1;
                        antitopSF=std::exp(0.0615-(0.0005*(genParticles->at(iGen)).pt()));
		}
        if(topInGen && antitopInGen)
		topPtSF=std::sqrt(topSF*antitopSF); 
//	std::cout<<topSF<<" "<<antitopSF<<" "<<topPtSF<<std::endl;
  }

  double genWeightPosForaTGC=std::abs(genWeight);
  if(isMC && !isSignal) genWeightPosForaTGC=genWeight;

  if (isMC) {
    totWeight = PUweight*genWeightPosForaTGC*aTGCWeightUnitConv*LeptonSF*btagWeight*VTagSF*topPtSF;
    totWeight_BTagUp = PUweight*genWeightPosForaTGC*aTGCWeightUnitConv*LeptonSF*btagWeight_BTagUp*VTagSF*topPtSF;
    totWeight_BTagDown = PUweight*genWeightPosForaTGC*aTGCWeightUnitConv*LeptonSF*btagWeight_BTagDown*VTagSF*topPtSF;
    totWeight_MistagUp = PUweight*genWeightPosForaTGC*aTGCWeightUnitConv*LeptonSF*btagWeight_MistagUp*VTagSF*topPtSF;
    totWeight_MistagDown = PUweight*genWeightPosForaTGC*aTGCWeightUnitConv*LeptonSF*btagWeight_MistagDown*VTagSF*topPtSF;
    totWeight_LeptonIDUp = PUweight*genWeightPosForaTGC*aTGCWeightUnitConv*LeptonSF_Up*btagWeight*VTagSF*topPtSF;
    totWeight_LeptonIDDown = PUweight*genWeightPosForaTGC*aTGCWeightUnitConv*LeptonSF_Down*btagWeight*VTagSF*topPtSF;
  }

  if(isSignal)
  {
	if(aTGCWeights.size()>=124) outTree_->Fill();
  }
  else
  {
	outTree_->Fill();
  }

}


float TreeMaker::getPUPPIweight(float puppipt, float puppieta){

  float genCorr  = 1.;
  float recoCorr = 1.;
  float totalWeight = 1.;
        
  genCorr =  puppisd_corrGEN->Eval( puppipt );
  if( fabs(puppieta)  <= 1.3 ){
    recoCorr = puppisd_corrRECO_cen->Eval( puppipt );
  }
  else{
    recoCorr = puppisd_corrRECO_for->Eval( puppipt );
  }

  if(isMC) totalWeight= genCorr * recoCorr;
  else totalWeight= recoCorr;

  return totalWeight;
}


float TreeMaker::getSmearingFactor(float sf, float unc, float resolution, const pat::Jet & jet, const edm::View<reco::GenJet> & genJets, int variation, float drMax, float relResMax, bool usePuppiPt) {
  // Calculate smearing factor using hybrid method
  // https://twiki.cern.ch/twiki/bin/view/CMS/JetResolution
  // i.e. rescale if match with gen-level object,
  // otherwise use stochastic method with scaling drawn from Gaussian
  // does matching with GenJets, using closest in dR, given some maximum dR
  // and relative resolution criteria
  //
  // sf is the scale factor, unc is its uncertainty
  // resolution is the relative pt resolution in simulation
  // jet is the reco jet
  // genJets are the collection of genJets to determine if there's a matching genJet
  // variation is whether to do nominal(0), up (1), or down (-1)
  // drMax is the maximum dR to count as a match with a genJet
  // relResMax is the maximum relative reoslution to count as a match
  // usePuppiPt is a bool to control whether to use Puppi Pt or normal pt for the reco jet
  if (!(variation==0 || abs(variation)==1)) {
    throw std::runtime_error("variation must be 0 (nominal) or +/-1");
  }

  float jet_pt = usePuppiPt ? jet.userFloat("ak8PFJetsPuppiValueMap:pt") : jet.pt();

  // First find if there's a match
  float ptGen = -1.;
  float dRBest = 9999;
  for (const auto & itr: genJets) {
    float dR = deltaR(jet, itr);
    float relRes = fabs(jet_pt - itr.pt())/jet_pt;
    if (dR < drMax && relRes < relResMax && dR < dRBest) {
      dRBest = dR;
      ptGen = itr.pt();
    }
  }
  // Now calc factor
  float this_sf = sf + (variation * unc);
  if (ptGen >= 0) {
    // scaling method
    // std::cout << "match" << std::endl;
    // std::cout << jet.pt() << " : " << jet.eta() << " : " << jet.phi() << std::endl;
    // std::cout << ptGen << " : " << etaGen << " : " << phiGen << std::endl;
    return 1 + ((this_sf-1)*(1-(ptGen/jet_pt)));
  } else {
    // std::cout << "no match" << std::endl;
    // stochastic method
    // initialise seed with reproducible number
    TRandom rand((int)(1000*jet.eta()));
    float random_gauss = rand.Gaus(0, resolution);
    return 1 + random_gauss * (sqrt(std::max((this_sf*this_sf) - 1, 0.0f)));
  }
}

void TreeMaker::saveDibosonMass(const ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<double> > & leptonicV_p4, const ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<double> > & hadronicV_p4, double & massVar)
{
  auto lvj_p4 = leptonicV_p4 + hadronicV_p4;
  massVar = lvj_p4.M();
}

void TreeMaker::saveDibosonMass(math::XYZTLorentzVector & leptonicV_p4, const ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<double> > & hadronicV_p4, double & massVar)
{
  auto lvj_p4 = leptonicV_p4 + hadronicV_p4;
  massVar = lvj_p4.M();
}

void TreeMaker::saveDibosonMass(math::XYZTLorentzVector & leptonicV_p4, math::XYZTLorentzVector & hadronicV_p4, double & massVar)
{
  auto lvj_p4 = leptonicV_p4 + hadronicV_p4;
  massVar = lvj_p4.M();
}


bool TreeMaker::decaysHadronic(const reco::Candidate* p)
{
   if(p!=NULL)
   {	
	//cout<<p->pdgId()<<" ";
	if(abs(p->pdgId())<7 || abs(p->pdgId())==21) return true;
	else
	{
		for(size_t i=0; i<p->numberOfDaughters(); ++i)
		{
		      	const reco::Candidate* d= (reco::Candidate*)p->daughter(i);
	               	if(decaysHadronic(d)) return true;
		}
	}
   }
   return false;	
}

void TreeMaker::beginRun(const edm::Run& iRun, const edm::EventSetup& iSetup){


  edm::Handle<LHERunInfoProduct> run; 


  if(!isSignal && isMC) iRun.getByLabel( "externalLHEProducer", run );
  else if (isSignal )iRun.getByLabel( "externalLHEProducer", run );
  else return;
  std::cout << "Nominal : " << run->heprup().PDFSUP.first << std::endl;
  NominalPDF = run->heprup().PDFSUP.first;

}

void TreeMaker::endRun(const edm::Run& iRun, const edm::EventSetup& iSetup){}
// ------------ method called once each job just before starting event loop  ------------
void 
TreeMaker::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void
TreeMaker::endJob() {
  std::cout << "TreeMaker endJob()..." << std::endl;
}

//define this as a plug-in
DEFINE_FWK_MODULE(TreeMaker);
