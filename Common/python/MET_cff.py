import FWCore.ParameterSet.Config as cms

METele = cms.EDFilter("PATMETSelector",
                         src = cms.InputTag("slimmedMETs"),
                         cut = cms.string("pt > 80"),
                         filter = cms.bool(True)
                         )

METmu = cms.EDFilter("PATMETSelector",
                         src = cms.InputTag("slimmedMETs"),
                         cut = cms.string("pt > 40"),
                         filter = cms.bool(True)
                         )

from PhysicsTools.PatUtils.tools.runMETCorrectionsAndUncertainties import runMetCorAndUncFromMiniAOD

def doMetCorrections(process, isData):
    # If you only want to re-correct and get the proper uncertainties
    # eg do Type-1 corrections due to new JEC
    runMetCorAndUncFromMiniAOD(process, isData=isData)
    # Update the main MET selectors
    process.METmu.src = cms.InputTag("patPFMetT1")
    process.METele.src = cms.InputTag("patPFMetT1")
