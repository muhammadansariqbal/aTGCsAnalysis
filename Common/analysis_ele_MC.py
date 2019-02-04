import FWCore.ParameterSet.Config as cms
from aTGCsAnalysis.SystematicsProducers.metSystematics_cff import *
process = cms.Process( "aTGCanalysis" )
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(10000)
)

process.options = cms.untracked.PSet(wantSummary = cms.untracked.bool(True))
process.options.allowUnscheduled = cms.untracked.bool(False) 

#process.load("aTGCsAnalysis.Common.L1PrefiringWeights_cff")
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

process.GlobalTag.globaltag = '80X_mcRun2_asymptotic_2016_TrancheIV_v8'
process.BadChargedCandidateFilter.muons = cms.InputTag("slimmedMuons")
process.BadChargedCandidateFilter.PFCandidates = cms.InputTag("packedPFCandidates")
process.BadPFMuonFilter.muons = cms.InputTag("slimmedMuons")
process.BadPFMuonFilter.PFCandidates = cms.InputTag("packedPFCandidates")

process.GenWeights = cms.EDAnalyzer("GenWeights",
            genInfo = cms.InputTag("generator"),
            )

process.NoiseFilters = cms.EDFilter("NoiseFilter",
            noiseFilter = cms.InputTag("TriggerResults", "", "PAT"),
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
                                    minNLoose = cms.int32(1),
                                    maxNLoose = cms.int32(1),
                                    minNTight = cms.int32(1),
                                    maxNTight = cms.int32(1),
				   )

process.MuonVeto = cms.EDFilter("LeptonVeto",
            looseLeptonSrc = cms.InputTag("looseMuons"),
            tightLeptonSrc = cms.InputTag("tightMuons"),
                                    minNLoose = cms.int32(0),
                                    maxNLoose = cms.int32(0),
                                    minNTight = cms.int32(0),
                                    maxNTight = cms.int32(0),
           )

process.leptonSequence = cms.Sequence(process.muSequence + process.eleSequence + process.ElectronVeto + process.MuonVeto +  process.leptonicWtoenuSequenceMC )

process.jetFilter = cms.EDFilter("CandViewCountFilter",
                                 src = cms.InputTag("goodJets"),
                                 minNumber = cms.uint32(1),
                                )

process.jetSequence = cms.Sequence(process.fatJetsSequence +
				                            process.jetFilter+
                          				   process.AK4JetsSequence)

# Update the MET for latest JEC etc
from aTGCsAnalysis.Common.MET_cff import doMetCorrections
doMetCorrections(process, isData=False, runBtoF=False)

process.treeDumper = cms.EDAnalyzer("TreeMaker",
                                    PUInfo = cms.InputTag("slimmedAddPileupInfo"),
                                    genInfo = cms.InputTag("generator"),
                                    rho = cms.InputTag("fixedGridRhoFastjetAll"),
                                    leptonicVSrc = cms.InputTag("Wtoenu"),
                                    metSrc = cms.InputTag("METele"),
                                    genSrc = cms.InputTag("prunedGenParticles"),
                                    fatJetSrc = cms.InputTag("goodJets"),
                                    genJetsAK8Src = cms.InputTag("slimmedGenJetsAK8"),
                                    AK4JetSrc  = cms.InputTag("goodAK4Jets"),
                                    vertexSrc = cms.InputTag("offlineSlimmedPrimaryVertices"),
                                    looseEleSrc = cms.InputTag("looseElectrons"),
                                    looseMuSrc = cms.InputTag("looseMuons"),
                                    leptonSrc = cms.InputTag("tightElectrons"),
                                    triggers = cms.InputTag("TriggerResults","","HLT"),
                                    LHEEventProductSrcExternal = cms.InputTag("externalLHEProducer"),
                                    isMC = cms.bool(True),
                                    BtagEffFile = cms.string(""),
                                    BtagDiscrCut = cms.double(0.9535),
                                    BtagDiscrCutMedium = cms.double(0.8484),
                                    BtagDiscrCutLoose = cms.double(0.5426),
		                    isSignal = cms.bool(False),
                                    channel = cms.string("el"),
                                    VTagSF = cms.double(1.03),
                                    )


# Identify the channel 
process.DecayChannel = cms.EDAnalyzer("DecayChannelAnalyzer")

process.metSequenceSystematics = CreateWLepWithSystematicsSequence(process, "el")

# PATH
process.analysis = cms.Path(
    process.GenWeights +
    process.NoiseFilters +
    process.BadChargedCandidateFilter +
    process.BadPFMuonFilter +
#   process.prefiringweight +
    process.egmGsfElectronIDSequence +
    process.leptonSequence +
    process.fullPatMetSequence +
    process.METele +
    process.Wtoenu +
    process.jetSequence +
    process.metSequenceSystematics +
    process.treeDumper
)


process.source = cms.Source("PoolSource",
    secondaryFileNames = cms.untracked.vstring(),
    # ttbar MC
    fileNames = cms.untracked.vstring('file:///eos/cms/store/mc/RunIISummer16MiniAODv2/TT_TuneCUETP8M2T4_13TeV-powheg-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/80000/FCB8AF84-A6BE-E611-8C65-0CC47A5FBE25.root'),
    # WJets MC
    #fileNames = cms.untracked.vstring('file:///eos/cms/store/mc/RunIISummer16MiniAODv2/WJetsToLNu_Pt-400To600_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/80000/A40770A2-1AD0-E611-AF94-A4BADB1C4493.root'),
)

process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

process.TFileService = cms.Service("TFileService",
                                 fileName = cms.string("tree_ele.root")
                                  )
