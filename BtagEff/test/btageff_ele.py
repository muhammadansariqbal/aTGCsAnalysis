import FWCore.ParameterSet.Config as cms

from aTGCsAnalysis.SystematicsProducers.metSystematics_cff import *

process = cms.Process( "BtagEff" )
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
process.load('RecoMET.METFilters.BadChargedCandidateFilter_cfi')
process.load('RecoMET.METFilters.BadPFMuonFilter_cfi')

process.GlobalTag.globaltag = '80X_mcRun2_asymptotic_2016_TrancheIV_v8'
process.BadChargedCandidateFilter.muons = cms.InputTag("slimmedMuons")
process.BadChargedCandidateFilter.PFCandidates = cms.InputTag("packedPFCandidates")
process.BadPFMuonFilter.muons = cms.InputTag("slimmedMuons")
process.BadPFMuonFilter.PFCandidates = cms.InputTag("packedPFCandidates")

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

# Note, using leptonicWtoenuSequenceData as we only want the reco'd W, not all the up/down variations
process.leptonSequence = cms.Sequence(process.muSequence + process.eleSequence + process.ElectronVeto + process.MuonVeto +  process.leptonicWtoenuSequenceData )

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


process.BtagAnalyzer = cms.EDAnalyzer("BTaggingEffAnalyzer",
                                      channel = cms.string("el"),
                                      vertexSrc = cms.InputTag("offlineSlimmedPrimaryVertices"),
                                      leptonSrc = cms.InputTag("tightElectrons"),
                                      leptonicVSrc = cms.InputTag("Wtoenu"),
                                      metSrc = cms.InputTag("METele"),
                                      JetsTag = cms.InputTag("goodAK4Jets"),
                                      fatJetSrc = cms.InputTag("goodJets"),
                                      DiscriminatorTag = cms.string("pfCombinedInclusiveSecondaryVertexV2BJetTags"),
                                      DiscriminatorValue = cms.double(0.9535),
                                      ptBinning = cms.vdouble(20., 30., 50., 70., 100., 140., 200., 300., 600., 1000.),
                                      #ptBinning = cms.vdouble(20., 40., 100., 300., 1000.),
                                      #etaBinning = cms.vdouble(-2.4, -0.8, 0.8, 2.4),
                                      etaBinning = cms.vdouble(-2.4, 2.4),
                                      METCut = cms.double(80.),
                                      leptonPtCut = cms.double(50.),
                                      WPtCut = cms.double(200.),
                                      MWWCut = cms.double(900.)
                                    )

# PATH
process.analysis = cms.Path(process.NoiseFilters  + process.BadChargedCandidateFilter  + process.BadPFMuonFilter + process.fullPatMetSequence + process.METele +  process.egmGsfElectronIDSequence +  process.leptonSequence +   process.jetSequence + process.BtagAnalyzer)


process.source = cms.Source("PoolSource",
    secondaryFileNames = cms.untracked.vstring(),
    fileNames = cms.untracked.vstring('file:/pnfs/desy.de/cms/tier2/store/mc/RunIISummer16MiniAODv2/TTJets_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/110000/08BA365D-40E5-E611-955F-00266CF89498.root'),

)

process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

process.TFileService = cms.Service("TFileService",
                                 fileName = cms.string("btag_tree_ele.root")
                                  )
