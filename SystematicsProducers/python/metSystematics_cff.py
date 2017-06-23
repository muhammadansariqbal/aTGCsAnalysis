import FWCore.ParameterSet.Config as cms

#Note that systematics on leptons is not added, 16.10.2015.

def CreateWLepWithSystematicsSequence(process, channel):
	process.metSequenceSystematics = cms.Sequence()
	ListOfSystematics = ["JetRes", "JetEn",  "MuonEn",  "ElectronEn", "UnclusteredEn", "LeptonRes"]
	metSrcPrefix = "patPFMetT1"
	_Wtomunu = cms.EDProducer("WLeptonicProducer",
						 leptons = cms.InputTag("bestMuon"),
						 MET = cms.InputTag("slimmedMETs"),
						 cut = cms.string("")
						 )
	_Wtoenu = cms.EDProducer("WLeptonicProducer",
						leptons = cms.InputTag("bestElectron"),
						MET = cms.InputTag("slimmedMETs"),
						cut = cms.string("")
						)
	for iSyst in ListOfSystematics:
		if channel == "mu" :
			if iSyst != "MuonEn" and iSyst!= "ElectronEn" and iSyst!= "LeptonRes" :
				setattr(process, 'Wtomunu' + iSyst + 'Up', _Wtomunu.clone(MET = cms.InputTag(metSrcPrefix + iSyst + 'Up')))
				setattr(process, 'Wtomunu' + iSyst + 'Down', _Wtomunu.clone(MET = cms.InputTag(metSrcPrefix + iSyst + 'Down')))
				process.metSequenceSystematics *= getattr(process, 'Wtomunu' + iSyst + 'Up') * getattr(process, 'Wtomunu' + iSyst + 'Down')
			elif iSyst is 'MuonEn' :
				setattr(process, 'Wtomunu' + iSyst + 'Up', _Wtomunu.clone(MET = cms.InputTag(metSrcPrefix + iSyst + 'Up'), leptons = cms.InputTag('bestMuonEnUp') ))
				setattr(process, 'Wtomunu' + iSyst + 'Down', _Wtomunu.clone(MET = cms.InputTag(metSrcPrefix + iSyst + 'Down'), leptons = cms.InputTag('bestMuonEnDown') ) )
				process.metSequenceSystematics *= getattr(process, 'Wtomunu' + iSyst + 'Up') * getattr(process, 'Wtomunu' + iSyst + 'Down')
			elif iSyst is 'LeptonRes' :
				setattr(process, 'Wtomunu' + iSyst + 'Up', _Wtomunu.clone( leptons = cms.InputTag('bestMuonResUp') ))
				setattr(process, 'Wtomunu' + iSyst + 'Down', _Wtomunu.clone( leptons = cms.InputTag('bestMuonResDown') ))
				process.metSequenceSystematics *= getattr(process, 'Wtomunu' + iSyst + 'Up') * getattr(process, 'Wtomunu' + iSyst + 'Down') 
			else :
				continue
		elif  channel == "el":
			if iSyst != "MuonEn" and iSyst!= "ElectronEn"  and iSyst != "LeptonRes" :
				setattr(process, 'Wtoelnu' + iSyst + 'Up', _Wtoenu.clone(MET = cms.InputTag(metSrcPrefix + iSyst + 'Up')))
				setattr(process, 'Wtoelnu' + iSyst + 'Down', _Wtoenu.clone(MET = cms.InputTag(metSrcPrefix + iSyst + 'Down')))
				process.metSequenceSystematics *= getattr(process, 'Wtoelnu' + iSyst + 'Up') * getattr(process, 'Wtoelnu' + iSyst + 'Down')
			elif iSyst is 'ElectronEn' :
				setattr(process, 'Wtoelnu' + iSyst + 'Up', _Wtoenu.clone(MET = cms.InputTag(metSrcPrefix + iSyst + 'Up'), leptons = cms.InputTag('bestElectronEnUp') ))
				setattr(process, 'Wtoelnu' + iSyst + 'Down', _Wtoenu.clone(MET = cms.InputTag(metSrcPrefix + iSyst + 'Down'), leptons = cms.InputTag('bestElectronEnDown')))
				process.metSequenceSystematics *= getattr(process, 'Wtoelnu' + iSyst + 'Up') * getattr(process, 'Wtoelnu' + iSyst + 'Down')
			elif iSyst is "LeptonRes" :
				setattr(process, 'Wtoelnu' + iSyst + 'Up', _Wtomunu.clone( leptons = cms.InputTag('bestElectronResUp') ))
				setattr(process, 'Wtoelnu' + iSyst + 'Down', _Wtomunu.clone( leptons = cms.InputTag('bestElectronResDown') ))
				process.metSequenceSystematics *= getattr(process, 'Wtoelnu' + iSyst + 'Up') * getattr(process, 'Wtoelnu' + iSyst + 'Down') 
			else :
				continue

		else :
			raise ValueError('Only muon and electron channel are supported, mu or el.')

	return process.metSequenceSystematics
