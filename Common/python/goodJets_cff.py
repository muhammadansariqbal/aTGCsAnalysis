import FWCore.ParameterSet.Config as cms
from PhysicsTools.SelectorUtils.pfJetIDSelector_cfi import pfJetIDSelector
from RecoJets.JetProducers.ak4PFJets_cfi import *
import PhysicsTools.PatAlgos.cleaningLayer1.jetCleaner_cfi as jetCleaner_cfi

# Clean jets first, common to all smearing etc variations
cleanJets = jetCleaner_cfi.cleanPatJets.clone()
cleanJets.src = "slimmedJetsAK8"
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

# Apply JEC
from PhysicsTools.PatAlgos.producersLayer1.jetUpdater_cff import updatedPatJetCorrFactors
from PhysicsTools.PatAlgos.producersLayer1.jetUpdater_cff import updatedPatJets

patAK8JetCorrFactorsReapplyJEC = updatedPatJetCorrFactors.clone(
        src = cms.InputTag("cleanJets"),
        levels = ['L1FastJet', 'L2Relative', 'L3Absolute'],
        payload = 'AK8PFchs'
        )

slimmedJetsAK8NewJEC = updatedPatJets.clone(
        jetSource = cms.InputTag("cleanJets"),
        jetCorrFactorsSource = cms.VInputTag(cms.InputTag("patAK8JetCorrFactorsReapplyJEC"))
        )

# smear newly corrected jets

slimmedJetsAK8Smeared = cms.EDProducer('SmearedPATJetProducer',
        src = cms.InputTag('slimmedJetsAK8NewJEC'),
        enabled = cms.bool(True),
        rho = cms.InputTag("fixedGridRhoFastjetAll"),
        algo = cms.string('AK8PFchs'),
        algopt = cms.string('AK8PFchs_pt'),

        genJets = cms.InputTag('slimmedGenJetsAK8'),
        dRMax = cms.double(0.4),  # since AK8
        dPtMaxFactor = cms.double(3),

        debug = cms.untracked.bool(False)
        )


# fat jets
selectedPatJetsAK8ByPt = cms.EDFilter("PATJetSelector",
    src = cms.InputTag("slimmedJetsAK8Smeared"),
    cut = cms.string("pt > 170"),
    filter = cms.bool(True)
)               

selectedPatJetsAK8 = cms.EDFilter("PATJetSelector",
    src = cms.InputTag("selectedPatJetsAK8ByPt"),
    cut = cms.string("abs(eta) < 2.4")
)     

goodJetsNotUsed = cms.EDFilter("PFJetIDSelectionFunctorFilter",
                        filterParams = pfJetIDSelector.clone(),
                        src = cms.InputTag("selectedPatJetsAK8")
                        )
goodJets = cms.EDFilter("jetID",
                        filter_flag = cms.bool(False),
                        jets_src = cms.InputTag("selectedPatJetsAK8"),
                        ID = cms.string("loose"))


bestJet =cms.EDFilter("LargestPtCandViewSelector",
    src = cms.InputTag("goodJets"),
    maxNumber = cms.uint32(1)
  )


fatJetsSequence = cms.Sequence(cleanJets + patAK8JetCorrFactorsReapplyJEC + slimmedJetsAK8NewJEC + slimmedJetsAK8Smeared + selectedPatJetsAK8ByPt + selectedPatJetsAK8 + goodJets + bestJet)

# Add in smeared AK8 jets
slimmedJetsAK8SmearedUp = slimmedJetsAK8Smeared.clone(variation=cms.int32(1))
selectedPatJetsAK8ByPtSmearedUp = selectedPatJetsAK8ByPt.clone(src=cms.InputTag("slimmedJetsAK8SmearedUp"))
selectedPatJetsAK8SmearedUp = selectedPatJetsAK8.clone(src=cms.InputTag("selectedPatJetsAK8ByPtSmearedUp"))
goodJetsSmearedUp = goodJets.clone(jets_src=cms.InputTag("selectedPatJetsAK8SmearedUp"))
bestJetSmearedUp = bestJet.clone(src=cms.InputTag("goodJetsSmearedUp"))
fatJetsSequence *= (slimmedJetsAK8SmearedUp + selectedPatJetsAK8ByPtSmearedUp + selectedPatJetsAK8SmearedUp + goodJetsSmearedUp + bestJetSmearedUp)

slimmedJetsAK8SmearedDown = slimmedJetsAK8Smeared.clone(variation=cms.int32(-1))
selectedPatJetsAK8ByPtSmearedDown = selectedPatJetsAK8ByPt.clone(src=cms.InputTag("slimmedJetsAK8SmearedDown"))
selectedPatJetsAK8SmearedDown = selectedPatJetsAK8.clone(src=cms.InputTag("selectedPatJetsAK8ByPtSmearedDown"))
goodJetsSmearedDown = goodJets.clone(jets_src=cms.InputTag("selectedPatJetsAK8SmearedDown"))
bestJetSmearedDown = bestJet.clone(src=cms.InputTag("goodJetsSmearedDown"))
fatJetsSequence *= (slimmedJetsAK8SmearedDown + selectedPatJetsAK8ByPtSmearedDown + selectedPatJetsAK8SmearedDown + goodJetsSmearedDown + bestJetSmearedDown)


# Create a different collection of jets which  contains b-tagging information. This is necessary because slimmedJetsAK8 jets don't contain BTagInfo

# Clean AK4 jets first
cleanAK4Jets = jetCleaner_cfi.cleanPatJets.clone()
cleanAK4Jets.src = "slimmedJets"
cleanAK4Jets.checkOverlaps.muons.src = "tightMuons"
cleanAK4Jets.checkOverlaps.muons.deltaR = 0.3
cleanAK4Jets.checkOverlaps.muons.requireNoOverlaps = True

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

# Apply JEC
patAK4JetCorrFactorsReapplyJEC = updatedPatJetCorrFactors.clone(
        src = cms.InputTag("cleanAK4Jets"),
        levels = ['L1FastJet', 'L2Relative', 'L3Absolute'],
        payload = 'AK4PFchs'
        )

slimmedJetsAK4NewJEC = updatedPatJets.clone(
        jetSource = cms.InputTag("cleanAK4Jets"),
        jetCorrFactorsSource = cms.VInputTag(cms.InputTag("patAK4JetCorrFactorsReapplyJEC"))
        )

# Smear newly corrected jets
slimmedJetsAK4Smeared = cms.EDProducer('SmearedPATJetProducer',
        src = cms.InputTag('slimmedJetsAK4NewJEC'),
        enabled = cms.bool(True),
        rho = cms.InputTag("fixedGridRhoFastjetAll"),
        algo = cms.string('AK4PFchs'),
        algopt = cms.string('AK4PFchs_pt'),

        genJets = cms.InputTag('slimmedGenJets'),
        dRMax = cms.double(0.2),  # since AK4
        dPtMaxFactor = cms.double(3),

        debug = cms.untracked.bool(False)
        )

selectedPatJetsAK4 = cms.EDFilter("PATJetSelector",
    src = cms.InputTag("slimmedJetsAK4Smeared"),
    cut = cms.string("pt > 30 & abs(eta) < 2.4"),  # only use central jets as only for b-tagging - HF useless
    #addBTagInfo = cms.bool(True),  # definitely need these!
    #addDiscriminators = cms.bool(True)
)

goodAK4JetsNotUsed = cms.EDFilter("PFJetIDSelectionFunctorFilter",
                        filterParams = pfJetIDSelector.clone(),
                        src = cms.InputTag("selectedPatJetsAK4")
                        )
goodAK4Jets = cms.EDFilter("jetID",
                        filter_flag = cms.bool(False),
                        jets_src = cms.InputTag("selectedPatJetsAK4"),
                        ID = cms.string("loose"))

AK4JetsSequence = cms.Sequence(cleanAK4Jets + patAK4JetCorrFactorsReapplyJEC + slimmedJetsAK4NewJEC + slimmedJetsAK4Smeared + selectedPatJetsAK4 + goodAK4Jets)

# Add in smeared AK4 jets
slimmedJetsAK4SmearedUp = slimmedJetsAK4Smeared.clone(variation=cms.int32(1))
selectedPatJetsAK4SmearedUp = selectedPatJetsAK4.clone(src=cms.InputTag("slimmedJetsAK4SmearedUp"))
goodAK4JetsSmearedUp = goodAK4Jets.clone(jets_src=cms.InputTag("selectedPatJetsAK4SmearedUp"))
AK4JetsSequence *= (slimmedJetsAK4SmearedUp + selectedPatJetsAK4SmearedUp + goodAK4JetsSmearedUp)

slimmedJetsAK4SmearedDown = slimmedJetsAK4Smeared.clone(variation=cms.int32(-1))
selectedPatJetsAK4SmearedDown = selectedPatJetsAK4.clone(src=cms.InputTag("slimmedJetsAK4SmearedDown"))
goodAK4JetsSmearedDown = goodAK4Jets.clone(jets_src=cms.InputTag("selectedPatJetsAK4SmearedDown"))
AK4JetsSequence *= (slimmedJetsAK4SmearedDown + selectedPatJetsAK4SmearedDown + goodAK4JetsSmearedDown)

# Add in shifted AK4 jets
slimmedJetsAK4ShiftedUp = cms.EDProducer("ShiftedPATJetProducer",
                                  addResidualJES = cms.bool(True),
                                  jetCorrLabelUpToL3 = cms.InputTag("ak4PFCHSL1FastL2L3Corrector"),
                                  jetCorrLabelUpToL3Res = cms.InputTag("ak4PFCHSL1FastL2L3ResidualCorrector"),
                                  jetCorrPayloadName = cms.string('AK4PFchs'),
                                  jetCorrUncertaintyTag = cms.string('Uncertainty'),
                                  shiftBy = cms.double(1.0),
                                  src = cms.InputTag("cleanAK4Jets")
                                 )
# Do we then need to smear the shifted jets?
selectedPatJetsAK4ShiftedUp = selectedPatJetsAK4.clone(src=cms.InputTag("slimmedJetsAK4ShiftedUp"))
goodAK4JetsShiftedUp = goodAK4Jets.clone(jets_src=cms.InputTag("selectedPatJetsAK4ShiftedUp"))
AK4JetsSequence *= (slimmedJetsAK4ShiftedUp + selectedPatJetsAK4ShiftedUp + goodAK4JetsShiftedUp)

slimmedJetsAK4ShiftedDown = cms.EDProducer("ShiftedPATJetProducer",
                                  addResidualJES = cms.bool(True),
                                  jetCorrLabelUpToL3 = cms.InputTag("ak4PFCHSL1FastL2L3Corrector"),
                                  jetCorrLabelUpToL3Res = cms.InputTag("ak4PFCHSL1FastL2L3ResidualCorrector"),
                                  jetCorrPayloadName = cms.string('AK4PFchs'),
                                  jetCorrUncertaintyTag = cms.string('Uncertainty'),
                                  shiftBy = cms.double(-1.0),
                                  src = cms.InputTag("cleanAK4Jets")
                                 )
selectedPatJetsAK4ShiftedDown = selectedPatJetsAK4.clone(src=cms.InputTag("slimmedJetsAK4ShiftedDown"))
goodAK4JetsShiftedDown = goodAK4Jets.clone(jets_src=cms.InputTag("selectedPatJetsAK4ShiftedDown"))
AK4JetsSequence *= (slimmedJetsAK4ShiftedDown + selectedPatJetsAK4ShiftedDown + goodAK4JetsShiftedDown)

