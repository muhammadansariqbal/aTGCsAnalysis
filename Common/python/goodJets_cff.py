import FWCore.ParameterSet.Config as cms
from PhysicsTools.SelectorUtils.pfJetIDSelector_cfi import pfJetIDSelector
from RecoJets.JetProducers.ak4PFJets_cfi import *
import PhysicsTools.PatAlgos.cleaningLayer1.jetCleaner_cfi as jetCleaner_cfi


# Apply JEC
from PhysicsTools.PatAlgos.producersLayer1.jetUpdater_cff import updatedPatJetCorrFactors
from PhysicsTools.PatAlgos.producersLayer1.jetUpdater_cff import updatedPatJets

patAK8JetCorrFactorsReapplyJEC = updatedPatJetCorrFactors.clone(
        src = cms.InputTag("slimmedJetsAK8"),
        levels = ['L2Relative', 'L3Absolute'],  # no L1FastJet ?
        payload = 'AK8PFchs'
        )

slimmedJetsAK8NewJEC = updatedPatJets.clone(
        jetSource = cms.InputTag("slimmedJetsAK8"),
        jetCorrFactorsSource = cms.VInputTag(cms.InputTag("patAK8JetCorrFactorsReapplyJEC"))
        )

# fat jets
selectedPatJetsAK8ByPt = cms.EDFilter("PATJetSelector",
    src = cms.InputTag("slimmedJetsAK8NewJEC"),
    cut = cms.string("pt > 170"),
    filter = cms.bool(True)
)               

selectedPatJetsAK8 = cms.EDFilter("PATJetSelector",
    src = cms.InputTag("selectedPatJetsAK8ByPt"),
    cut = cms.string("abs(eta) < 2.4")
)     

cleanJets = jetCleaner_cfi.cleanPatJets.clone()
cleanJets.src = "selectedPatJetsAK8"
cleanJets.checkOverlaps.muons.src = "tightMuons"
cleanJets.checkOverlaps.muons.deltaR = 1.0
cleanJets.checkOverlaps.muons.requireNoOverlaps = True


cleanJets.checkOverlaps.electrons.src = "tightElectrons"
cleanJets.checkOverlaps.electrons.deltaR = 1.0
cleanJets.checkOverlaps.electrons.requireNoOverlaps = True
cleanJets.checkOverlaps.photons = cms.PSet()
cleanJets.checkOverlaps.taus = cms.PSet()
cleanJets.checkOverlaps.tkIsoElectrons = cms.PSet()
cleanJets.finalCut = ""


goodJetsNotUsed = cms.EDFilter("PFJetIDSelectionFunctorFilter",
                        filterParams = pfJetIDSelector.clone(),
                        src = cms.InputTag("cleanJets")
                        )
goodJets = cms.EDFilter("jetID",
                        filter_flag = cms.bool(False),
                        jets_src = cms.InputTag("cleanJets"),
                        ID = cms.string("loose"))


bestJet =cms.EDFilter("LargestPtCandViewSelector",
    src = cms.InputTag("goodJets"),
    maxNumber = cms.uint32(1)
  )



fatJetsSequence = cms.Sequence(patAK8JetCorrFactorsReapplyJEC + slimmedJetsAK8NewJEC + selectedPatJetsAK8ByPt + selectedPatJetsAK8 + cleanJets + goodJets + bestJet)


# Create a different collection of jets which  contains b-tagging information. This is necessary because slimmedJetsAK8 jets don't contain BTagInfo

selectedPatJetsAK4 = cms.EDFilter("PATJetSelector",
    src = cms.InputTag("slimmedJets"),
    cut = cms.string("pt > 30 & abs(eta) < 2.4")
)      

cleanAK4Jets = jetCleaner_cfi.cleanPatJets.clone()
cleanAK4Jets.src = "selectedPatJetsAK4"
cleanAK4Jets.checkOverlaps.muons.src = "tightMuons"
cleanAK4Jets.checkOverlaps.muons.deltaR = 0.3
cleanAK4Jets.checkOverlaps.muons.requireNoOverlaps = True


#cleanJets.checkOverlaps.muons = cms.PSet()

cleanAK4Jets.checkOverlaps.electrons.src = "tightElectrons"
cleanAK4Jets.checkOverlaps.electrons.deltaR = 0.3
cleanAK4Jets.checkOverlaps.electrons.requireNoOverlaps = True
cleanAK4Jets.checkOverlaps.photons = cms.PSet()
cleanAK4Jets.checkOverlaps.taus = cms.PSet()
cleanAK4Jets.checkOverlaps.tkIsoElectrons = cms.PSet()
cleanAK4Jets.checkOverlaps.jets = cms.PSet(
					src = cms.InputTag("bestJet"),
					algorithm = cms.string("byDeltaR"),
					preselection = cms.string(""),
					deltaR = cms.double(0.8),
					checkRecoComponents = cms.bool(False), # don't check if they share some AOD object ref
					pairCut = cms.string(""),
					requireNoOverlaps = cms.bool(True), # overlaps don't cause the jet to be discared
				      )

goodAK4JetsNotUsed = cms.EDFilter("PFJetIDSelectionFunctorFilter",
                        filterParams = pfJetIDSelector.clone(),
                        src = cms.InputTag("cleanAK4Jets")
                        )
goodAK4Jets = cms.EDFilter("jetID",
                        filter_flag = cms.bool(False),
                        jets_src = cms.InputTag("cleanAK4Jets"),
                        ID = cms.string("loose"))




AK4JetsSequence = cms.Sequence(selectedPatJetsAK4 + cleanAK4Jets + goodAK4Jets)

