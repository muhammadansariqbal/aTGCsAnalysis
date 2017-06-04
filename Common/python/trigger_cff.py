import FWCore.ParameterSet.Config as cms

TriggerMuon = cms.EDFilter("HLTHighLevel",
				  HLTPaths = cms.vstring("HLT_Mu50_v*", "HLT_TkMu50_v*"),
				  throw = cms.bool(False),
				  TriggerResultsTag = cms.InputTag("TriggerResults", "", "HLT"),
				  andOr = cms.bool(True), eventSetupPathsKey = cms.string("")#false = and-mode (all requested triggers), true = or-mode (at least one) 
				 )

# electron trigger is disables
TriggerElectron = cms.EDFilter("HLTHighLevel",
				  HLTPaths = cms.vstring("HLT_Ele45_WPLoose_Gsf_v*", "HLT_Ele105_CaloIdVT_GsfTrkIdT_v*", "HLT_Ele115_CaloIdVT_GsfTrkIdT_v*", "HLT_Ele30_WPTight_Gsf_v*", "HLT_Ele50_CaloIdVT_GsfTrkIdT_PFJet165_v*"),
				  throw = cms.bool(False),
				  TriggerResultsTag = cms.InputTag("TriggerResults", "", "HLT"),
				  andOr = cms.bool(True), eventSetupPathsKey = cms.string("")#false = and-mode (all requested triggers), true = or-mode (at least one) 
				 )

