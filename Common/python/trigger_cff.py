import FWCore.ParameterSet.Config as cms

TriggerMuon = cms.EDFilter("HLTHighLevel",
				  HLTPaths = cms.vstring("HLT_Mu50_v*", "HLT_TkMu50_v*"),
				  throw = cms.bool(False),
				  TriggerResultsTag = cms.InputTag("TriggerResults", "", "HLT"),
				  andOr = cms.bool(True), eventSetupPathsKey = cms.string("")#false = and-mode (all requested triggers), true = or-mode (at least one) 
				 )

TriggerElectron = cms.EDFilter("HLTHighLevel",
				  HLTPaths = cms.vstring("HLT_Ele115_CaloIdVT_GsfTrkIdT_v*", "HLT_Ele40_WPTight_Gsf_v*", "HLT_Photon175_v*"),
				  throw = cms.bool(False),
				  TriggerResultsTag = cms.InputTag("TriggerResults", "", "HLT"),
				  andOr = cms.bool(True), eventSetupPathsKey = cms.string("")#false = and-mode (all requested triggers), true = or-mode (at least one) 
				 )

