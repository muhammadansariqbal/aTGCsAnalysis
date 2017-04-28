import FWCore.ParameterSet.Config as cms

from aTGCsAnalysis.SystematicsProducers.metSystematics_cff import *

process = cms.Process( "aTGCanalysis" )
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(10000)
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
process.GlobalTag.globaltag = '76X_mcRun2_asymptotic_RunIIFall15DR76_v1'

process.GenWeights = cms.EDAnalyzer("GenWeights",
            genInfo = cms.InputTag("generator"),
            )


process.NoiseFilters = cms.EDFilter("NoiseFilter",
            noiseFilter = cms.InputTag("TriggerResults", "", "PAT"),
            filterNames = cms.vstring("Flag_HBHENoiseFilter", "Flag_HBHENoiseIsoFilter",  "Flag_CSCTightHaloFilter", "Flag_EcalDeadCellTriggerPrimitiveFilter", "Flag_goodVertices", "Flag_eeBadScFilter") 
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
my_id_modules = ['RecoEgamma.ElectronIdentification.Identification.heepElectronID_HEEPV61_cff']

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


process.leptonSequence = cms.Sequence(process.muSequence + process.eleSequence + process.ElectronVeto + process.MuonVeto +  process.leptonicWtomunuSequenceMC )

process.jetFilter = cms.EDFilter("CandViewCountFilter",
                                 src = cms.InputTag("goodJets"),
                                 minNumber = cms.uint32(1),
                                )

process.jetSequence = cms.Sequence(process.fatJetsSequence +
                                    process.jetFilter+
                                     process.AK4JetsSequence )

process.treeDumper = cms.EDAnalyzer("TreeMaker",
                                    PUInfo = cms.InputTag("slimmedAddPileupInfo"),
                                    genInfo = cms.InputTag("generator"),
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
                                    LHEEventProductSrcExternal = cms.InputTag("source"),
                                    isMC = cms.bool(True),
                                    BtagEffFile = cms.string(""),
                                    isSignal = cms.bool(True),
                                    channel = cms.string("mu"),
                                    VTagSF = cms.double(0.915)
                                    )


# Identify the channel 
process.DecayChannel = cms.EDAnalyzer("DecayChannelAnalyzer")

process.metSequenceSystematics = CreateWLepWithSystematicsSequence(process, "mu")

# PATH
process.analysis = cms.Path(process.GenWeights + process.NoiseFilters + process.TriggerMuon + process.METmu +  process.egmGsfElectronIDSequence +  process.leptonSequence +   process.jetSequence + process.metSequenceSystematics +  process.treeDumper)

process.source = cms.Source("PoolSource",
    secondaryFileNames = cms.untracked.vstring(),
    fileNames = cms.untracked.vstring('/store/mc/RunIIFall15MiniAODv2/WWToLNuQQ_aTGC_13TeV-madgraph-pythia8/MINIAODSIM/PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/50000/B64AA5AE-B5B8-E511-80FD-001EC9B21623.root'),    
)


process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 1000
process.TFileService = cms.Service("TFileService",
                                 fileName = cms.string("tree_mu.root")
                                  )