import FWCore.ParameterSet.Config as cms

process = cms.Process( "aTGCanalysis" )
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(10000000)
)

process.options = cms.untracked.PSet(wantSummary = cms.untracked.bool(True))
process.options.allowUnscheduled = cms.untracked.bool(False) 

process.load("aTGCsAnalysis.Common.goodMuons_cff")
process.load("aTGCsAnalysis.Common.goodElectrons_cff")
process.load("aTGCsAnalysis.Common.MET_cff")
process.load("aTGCsAnalysis.Common.goodJets_cff")
process.load("aTGCsAnalysis.Common.trigger_cff")
process.load("aTGCsAnalysis.Common.leptonicW_cff")
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
process.load("Configuration.StandardSequences.GeometryRecoDB_cff")
process.load('RecoMET.METFilters.BadChargedCandidateFilter_cfi')
process.load('RecoMET.METFilters.BadPFMuonFilter_cfi')

process.GlobalTag.globaltag = '80X_dataRun2_2016SeptRepro_v7'
process.BadChargedCandidateFilter.muons = cms.InputTag("slimmedMuons")
process.BadChargedCandidateFilter.PFCandidates = cms.InputTag("packedPFCandidates")
process.BadPFMuonFilter.muons = cms.InputTag("slimmedMuons")
process.BadPFMuonFilter.PFCandidates = cms.InputTag("packedPFCandidates")

process.NoiseFilters = cms.EDFilter("NoiseFilter",
            noiseFilter = cms.InputTag("TriggerResults", "", "RECO"),
            filterNames = cms.vstring("Flag_HBHENoiseFilter", "Flag_HBHENoiseIsoFilter",  "Flag_globalTightHalo2016Filter", "Flag_EcalDeadCellTriggerPrimitiveFilter", "Flag_goodVertices", "Flag_eeBadScFilter") 
            )

#
# Set up electron ID (VID framework)
#

from PhysicsTools.SelectorUtils.tools.vid_id_tools import *
# turn on VID producer, indicate data format  to be
# DataFormat.AOD or DataFormat.MiniAOD, as appropriate 
dataFormat = DataFormat.MiniAOD

switchOnVIDElectronIdProducer(process, dataFormat)

# define which IDs we want to produce
my_id_modules = ['RecoEgamma.ElectronIdentification.Identification.heepElectronID_HEEPV70_cff']

#add them to the VID producer
for idmod in my_id_modules:
    setupAllVIDIdsInModule(process,idmod,setupVIDElectronSelection)

#
# Configure an example module for user analysis with electrons
#

process.ElectronVeto = cms.EDFilter("LeptonVeto",
            looseLeptonSrc = cms.InputTag("looseElectrons"),
            tightLeptonSrc = cms.InputTag("tightElectrons"),
                                    minNLoose = cms.int32(0),
                                    maxNLoose = cms.int32(0),
                                    minNTight = cms.int32(0),
                                    maxNTight = cms.int32(0),
           )

process.MuonVeto = cms.EDFilter("LeptonVeto",
            looseLeptonSrc = cms.InputTag("looseMuons"),
            tightLeptonSrc = cms.InputTag("tightMuons"),
                                    minNLoose = cms.int32(1),
                                    maxNLoose = cms.int32(1),
                                    minNTight = cms.int32(1),
                                    maxNTight = cms.int32(1),
           )


process.leptonSequence = cms.Sequence(process.muSequence + process.eleSequence + process.ElectronVeto + process.MuonVeto +  process.leptonicWtomunuSequenceData )

# Correct JEC levels for data
process.patAK8JetCorrFactorsReapplyJEC.levels.append("L2L3Residual")
process.patAK4JetCorrFactorsReapplyJEC.levels.append("L2L3Residual")

process.jetFilter = cms.EDFilter("CandViewCountFilter",
                                 src = cms.InputTag("goodJets"),
                                 minNumber = cms.uint32(1),
                                )

process.jetSequence = cms.Sequence(process.fatJetsSequence +
                                    process.jetFilter+
                                     process.AK4JetsSequence )

# Update the MET for latest JEC etc
from aTGCsAnalysis.Common.MET_cff import doMetCorrections
doMetCorrections(process, isData=True)

process.treeDumper = cms.EDAnalyzer("TreeMaker",
                                    rho = cms.InputTag("fixedGridRhoFastjetAll"),
                                    leptonicVSrc = cms.InputTag("Wtomunu"),
                                    metSrc = cms.InputTag("METmu"),
                                    genSrc = cms.InputTag("prunedGenParticles"),
                                    fatJetSrc = cms.InputTag("goodJets"),
                                    AK4JetSrc  = cms.InputTag("goodAK4Jets"),
                                    vertexSrc = cms.InputTag("offlineSlimmedPrimaryVertices"),
                                    looseEleSrc = cms.InputTag("looseElectrons"),
                                    looseMuSrc = cms.InputTag("looseMuons"),
                                    leptonSrc = cms.InputTag("tightMuons"),
                                    isMC = cms.bool(False),
                                    BtagEffFile = cms.string(""),
		                            isSignal = cms.bool(False),	
                                    channel = cms.string("mu")
                                    )


# Identify the channel 
process.DecayChannel = cms.EDAnalyzer("DecayChannelAnalyzer")

# PATH
process.analysis = cms.Path(process.NoiseFilters + process.BadChargedCandidateFilter  + process.BadPFMuonFilter  + process.TriggerMuon + process.fullPatMetSequence + process.METmu +  process.egmGsfElectronIDSequence +  process.leptonSequence +  process.jetSequence +  process.treeDumper)

process.source = cms.Source("PoolSource",
    secondaryFileNames = cms.untracked.vstring(),
    fileNames = cms.untracked.vstring('file:///afs/cern.ch/work/m/maiqbal/private/aTGC/TestSamples/data_mu.root')
    
)

process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 1000
process.TFileService = cms.Service("TFileService",
                                 fileName = cms.string("tree_mu.root")
                                  )
