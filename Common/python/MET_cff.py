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

def doMetCorrections(process, isData, runBtoF):
    # If you only want to re-correct and get the proper uncertainties
    # eg do Type-1 corrections due to new JEC
    runMetCorAndUncFromMiniAOD(process, isData=isData)

    # Update MET phi corrections to latest ones
    newPhiCorr = None
    if isData:
        if runBtoF:
            from multPhiCorr_ReMiniAOD_Data_BCDEF_80X_sumPt_cfi import multPhiCorr_Data_BCDEF_80X as newPhiCorr
        else:
            from multPhiCorr_ReMiniAOD_Data_GH_80X_sumPt_cfi import multPhiCorr_Data_GH_80X as newPhiCorr
    else:
        from multPhiCorr_Summer16_MC_DY_80X_sumPt_cfi import multPhiCorr_MC_DY_sumPT_80X as newPhiCorr
    if not newPhiCorr:
        raise RuntimeError("Cannot find new phi corrections for this input")
    process.patPFMetTxyCorr.parameters = newPhiCorr

    # Update the main MET selectors
    process.METmu.src = cms.InputTag("patPFMetT1Txy")
    process.METele.src = cms.InputTag("patPFMetT1Txy")
