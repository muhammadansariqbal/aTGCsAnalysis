import os
import sys
import fileinput
import optparse
import sys
import shutil
import subprocess
import collections

def DefineNJobs(sample): 
	ps = subprocess.Popen(['./das_client.py', '--query', "file dataset=" + sample + " | count(file.name)"], stdout=subprocess.PIPE)
	output = ps.communicate()[0]
	for line in output.splitlines():
		if "count(file.name)=" in line :
			replacedline = line.replace("count(file.name)=","")
	N = int(replacedline)
	
	if N < 100 :
		NFilesPerJob = 1
	else :
		# let's roughly assume that number of files is ~1000 in the worst case
		NFilesPerJob = int(N/100)
	print "N of files per job : " , sample , " " , NFilesPerJob	
	return NFilesPerJob


def createConfigFile(processName, channel, isMC, isSignal, runBtoF=True):
	if not os.path.exists("analysisConfigs"):
		os.makedirs("analysisConfigs")
	BTagEfficiencyPattern = "BtagEffFile = cms.string(\"\"),\n"
	VTagSFPattern = "VTagSF = cms.double(1.03)"
	ConfigFileName = ""
   	if isSignal and isMC :
   		shutil.copy("../analysis_" + channel + "_signal.py", "analysisConfigs")
   		ConfigFileName = "analysisConfigs/analysis_" + channel + "_signal.py"
   	elif  processName == "WZ":
   		shutil.copy("../analysis_" + channel + "_MC.py", "analysisConfigs/analysis_" + channel + "_" + processName + ".py")
   		ConfigFileName = "analysisConfigs/analysis_" + channel + "_" + processName + ".py"
   		configFile = open("../analysis_" + channel + "_MC.py")
   		outFile = open("analysisConfigs/analysis_" + channel + "_" + processName + ".py", "w+")
   		for line in configFile:
   			if BTagEfficiencyPattern in line :
   				replaceWith = "BtagEffFile = cms.string(\"aTGCsAnalysis/TreeMaker/data/eff_" + processName + "_" + channel + ".root\"),\n"
   				outFile.write(line.replace(BTagEfficiencyPattern, replaceWith ))
   			else :
   				outFile.write(line)
   	elif "ttbar" in processName:
   		shutil.copy("../analysis_" + channel + "_MC.py", "analysisConfigs/analysis_" + channel + "_" + processName + ".py")
   		ConfigFileName = "analysisConfigs/analysis_" + channel + "_" + processName + ".py"
   		configFile = open("../analysis_" + channel + "_MC.py")
   		outFile = open("analysisConfigs/analysis_" + channel + "_" + processName + ".py", "w+")
   		for line in configFile:
   			if BTagEfficiencyPattern in line :
   				replaceWith = "BtagEffFile = cms.string(\"aTGCsAnalysis/TreeMaker/data/eff_ttbar_" + channel + ".root\"),\n"
   				outFile.write(line.replace(BTagEfficiencyPattern, replaceWith ))
   			else :
   				outFile.write(line)
   	# don't apply V-tagging scale factor for W+jets or single top t-channel and s-channel
   	elif "WJets" in processName or "SingleTop-t-channel" in  processName or "SingleTop-s-channel" in processName:
   		print processName
   		shutil.copy("../analysis_" + channel + "_MC.py", "analysisConfigs/analysis_" + channel + "_" + processName + ".py")
   		ConfigFileName = "analysisConfigs/analysis_" + channel + "_" + processName + ".py"
   		configFile = open("../analysis_" + channel + "_MC.py")
   		outFile = open("analysisConfigs/analysis_" + channel + "_" + processName + ".py", "w+")
   		for line in configFile:
   			if VTagSFPattern in line :
   				replaceWith = "VTagSF = cms.double(1.0)"
   				outFile.write(line.replace(VTagSFPattern, replaceWith ))
   			else :
   				outFile.write(line)

   	elif isMC :
   		shutil.copy("../analysis_" + channel + "_MC.py", "analysisConfigs")
   		ConfigFileName = "analysisConfigs/analysis_" + channel + "_MC.py"
   	elif not isMC :
   		# DATA
   		original_config = "../analysis_" + channel + ".py"
   		ConfigFileName = "analysisConfigs/analysis_"+channel+"_" + processName + ".py"
   		shutil.copy(original_config, ConfigFileName)
   		# specific edits for Runs B-F vs G-H
   		if not runBtoF:
			with open(original_config) as inFile, open(ConfigFileName, "w+") as outFile:
				for line in inFile:
					if "runBtoF=True" in line:
						outFile.write(line.replace("runBtoF=True", "runBtoF=False"))
					else:
						outFile.write(line)
   	else :
   		raise ValueError('This should not happen!')
   	return os.path.abspath(ConfigFileName)


def createFileForJob(processName, channel, sampleName, feature, configFileName, outDir, YourJSONFile, RunRange, isMC, isSignal, wantToSubmit=False, fileName='template.txt'):
	if not os.path.exists(outDir):
    		os.makedirs(outDir)

	patternFeature = "$FEATURE$"
	patternProcessName = "$PROCESS$"
	patternChannel = "$CHANNEL$"
	patternSample = "$SAMPLE$"
	patternConfigFile= "$CONFIGFILENAME$"
	patternJobSplitting = "$JOBSplitting$"
	patternForDataOnly = "$IsData$"
	patterUnitPerJob = "$UNITSPERJOB$"


	fileTemplate = 'templates/' + fileName
	tempFile = open( outDir + "/" + processName + "-" + channel + ".py", 'w+' )
	infile = open(fileTemplate)
	for line in infile:
		if patternFeature in line and patternChannel in line and patternProcessName in line:
			tempFile.write( line.replace( patternFeature, feature).replace(patternProcessName, processName).replace(patternChannel, channel))
		elif patternConfigFile in line :
			tempFile.write( line.replace( patternConfigFile, configFileName))		
		elif patternSample in line:
			tempFile.write( line.replace( patternSample, sampleName))
		elif patternJobSplitting in line:
			if isMC :
				tempFile.write( line.replace( patternJobSplitting, "FileBased"))
			else :
				tempFile.write( line.replace( patternJobSplitting, "LumiBased"))
		elif patternForDataOnly in line:
			if not isMC:
				tempFile.write( "config.Data.lumiMask = '" + YourJSONFile + "'\n")
				tempFile.write( "config.Data.runRange = '" + RunRange + "'\n")
			else :
				continue
		elif patterUnitPerJob in line :
			if isMC :
				NUnitPerJobWise = DefineNJobs(sampleName)
				tempFile.write( line.replace( patterUnitPerJob, str(NUnitPerJobWise)))
			else :
				tempFile.write( line.replace( patterUnitPerJob, "150"))
		else:
			tempFile.write( line)
	tempFile.close()
	if wantToSubmit :
		os.system("crab submit -c " + os.path.dirname(os.path.abspath(__file__)) + "/" + outDir + "/"  + processName + "-" + channel + ".py")
		print "\033[0;40;32mtask:", processName + "-" + channel, "was submitted!\033[0m"
	return;


parser =  optparse.OptionParser()
parser.add_option('-p', '--Feature', dest="Feature", default='my_feature')
(options, arg) = parser.parse_args()


def submitJobs(MCBackgroundsSampleDictionary, SignalMCSampleDictionary, DataDictionaryElectronChannel, DataDictionaryMuonChannel, JSONFile, YourRunRange,wantToSubmit=False):
#	for key in MCBackgroundsSampleDictionary:
#		ConfigFileName = createConfigFile(key, "mu", True, False)
#		print key, " ", ConfigFileName
#		createFileForJob(key, "mu", MCBackgroundsSampleDictionary[key], options.Feature, ConfigFileName,  "crabConfigs",  JSONFile, YourRunRange, True, False, wantToSubmit)
#		ConfigFileName = createConfigFile(key, "ele", True, False)
#		createFileForJob(key, "ele", MCBackgroundsSampleDictionary[key], options.Feature, ConfigFileName,  "crabConfigs",  JSONFile, YourRunRange, True, False, wantToSubmit)
#	for key in SignalMCSampleDictionary:
#		ConfigFileName = createConfigFile(key, "mu", True, True)
#		createFileForJob(key, "mu", SignalMCSampleDictionary[key], options.Feature, ConfigFileName,  "crabConfigs",  JSONFile, YourRunRange, True, True, wantToSubmit)
#		ConfigFileName = createConfigFile(key, "ele", True, True)
#		createFileForJob(key, "ele", SignalMCSampleDictionary[key], options.Feature, ConfigFileName, "crabConfigs", JSONFile, YourRunRange, True, True, wantToSubmit)
#	for key in DataDictionaryElectronChannel:
#		runBtoF = not ("RunG" in key or "RunH" in key)
#		ConfigFileName = createConfigFile(key, "ele", False, False, runBtoF)
#		createFileForJob(key, "ele", DataDictionaryElectronChannel[key], options.Feature, ConfigFileName,  "crabConfigs",  JSONFile, YourRunRange, False, True, wantToSubmit)
	for key in DataDictionaryMuonChannel:
		runBtoF = not ("RunG" in key or "RunH" in key)
		ConfigFileName = createConfigFile(key, "mu", False, False, runBtoF)
		createFileForJob(key, "mu", DataDictionaryMuonChannel[key], options.Feature, ConfigFileName,  "crabConfigs",  JSONFile, YourRunRange, False, True, wantToSubmit)

MCBackgroundsSampleDictionaryUnordered =[
	('ttbar-powheg','/TT_TuneCUETP8M2T4_13TeV-powheg-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/MINIAODSIM'),

	('WJets_HT-100To200','/WJetsToLNu_HT-100To200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/MINIAODSIM'),
	('WJets_HT-100To200-ext1','/WJetsToLNu_HT-100To200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1/MINIAODSIM'),
	('WJets_HT-100To200-ext2','/WJetsToLNu_HT-100To200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext2-v1/MINIAODSIM'),

	('WJets_HT-200To400','/WJetsToLNu_HT-200To400_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/MINIAODSIM'),
	('WJets_HT-200To400-ext1','/WJetsToLNu_HT-200To400_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1/MINIAODSIM'),
	('WJets_HT-200To400-ext2','/WJetsToLNu_HT-200To400_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext2-v1/MINIAODSIM'),

	('WJets_HT-400To600','/WJetsToLNu_HT-400To600_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/MINIAODSIM'),
	('WJets_HT-400To600-ext','/WJetsToLNu_HT-400To600_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1/MINIAODSIM'),

	('WJets_HT-600To800','/WJetsToLNu_HT-600To800_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/MINIAODSIM'),
	('WJets_HT-600To800-ext','/WJetsToLNu_HT-600To800_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1/MINIAODSIM'),

	('WJets_HT-800To1200','/WJetsToLNu_HT-800To1200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/MINIAODSIM'),
	('WJets_HT-800To1200-ext','/WJetsToLNu_HT-800To1200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1/MINIAODSIM'),

	('WJets_HT-1200To2500','/WJetsToLNu_HT-1200To2500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/MINIAODSIM'),
	('WJets_HT-1200To2500-ext','/WJetsToLNu_HT-1200To2500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1/MINIAODSIM'),

	('WJets_HT-2500ToInf','/WJetsToLNu_HT-2500ToInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/MINIAODSIM'),
	('WJets_HT-2500ToInf-ext','/WJetsToLNu_HT-2500ToInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1/MINIAODSIM'),

	('WW','/WWToLNuQQ_13TeV-powheg/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/MINIAODSIM'),
	('WW-ext','/WWToLNuQQ_13TeV-powheg/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1/MINIAODSIM'),

	('WZ','/WZTo1L1Nu2Q_13TeV_amcatnloFXFX_madspin_pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v3/MINIAODSIM'),

	('SingleTop-t-channel-top','/ST_t-channel_top_4f_inclusiveDecays_13TeV-powhegV2-madspin-pythia8_TuneCUETP8M1/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/MINIAODSIM'),
	('SingleTop-t-channel-antitop','/ST_t-channel_antitop_4f_inclusiveDecays_13TeV-powhegV2-madspin-pythia8_TuneCUETP8M1/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/MINIAODSIM'),

	('SingleTop-s-channel','/ST_s-channel_4f_leptonDecays_13TeV-amcatnlo-pythia8_TuneCUETP8M1/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/MINIAODSIM'),

	('SingleTop-tW-channel-antitop','/ST_tW_antitop_5f_inclusiveDecays_13TeV-powheg-pythia8_TuneCUETP8M1/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1/MINIAODSIM'),
	('SingleTop-tW-channel-top','/ST_tW_top_5f_inclusiveDecays_13TeV-powheg-pythia8_TuneCUETP8M1/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1/MINIAODSIM')
]

SignalMCSampleDictionaryUnordered =[
	('WW-signal_MWW-600To800','/WWToLNuQQ_MWW-600To800_PtW-180_aTGC_ShowerReconfig_13TeV-amcatnloFXFX-madspin-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v2/MINIAODSIM'),
	('WW-signal_MWW-800ToInf','/WWToLNuQQ_MWW-800_PtW-180_aTGC_ShowerReconfig_13TeV-amcatnloFXFX-madspin-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/MINIAODSIM'),
	('WZ-signal_MWZ-600To800','/WZToLNuQQ_MWZ-600To800_PtZ-180_aTGC_ShowerReconfig_13TeV-amcatnloFXFX-madspin-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/MINIAODSIM'),
	('WZ-signal_MWZ-800ToInf','/WZToLNuQQ_MWZ-800_PtZ-180_aTGC_ShowerReconfig_13TeV-amcatnloFXFX-madspin-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/MINIAODSIM')
]

DataDictionaryMuonChannelUnordered = [
	('data-RunB_ver1','/SingleMuon/Run2016B-03Feb2017_ver1-v1/MINIAOD'),
	('data-RunB_ver2','/SingleMuon/Run2016B-03Feb2017_ver2-v2/MINIAOD'),
	('data-RunC','/SingleMuon/Run2016C-03Feb2017-v1/MINIAOD'),
	('data-RunD','/SingleMuon/Run2016D-03Feb2017-v1/MINIAOD'),
	('data-RunE','/SingleMuon/Run2016E-03Feb2017-v1/MINIAOD'),
	('data-RunF','/SingleMuon/Run2016F-03Feb2017-v1/MINIAOD'),
	('data-RunG','/SingleMuon/Run2016G-03Feb2017-v1/MINIAOD'),
	('data-RunH_ver2','/SingleMuon/Run2016H-03Feb2017_ver2-v1/MINIAOD'),
	('data-RunH_ver3','/SingleMuon/Run2016H-03Feb2017_ver3-v1/MINIAOD')
]

DataDictionaryElectronChannelUnordered = [
	('data-RunB_ver1','/SingleElectron/Run2016B-03Feb2017_ver1-v1/MINIAOD'),
	('data-RunB_ver2','/SingleElectron/Run2016B-03Feb2017_ver2-v2/MINIAOD'),
	('data-RunC','/SingleElectron/Run2016C-03Feb2017-v1/MINIAOD'),
	('data-RunD','/SingleElectron/Run2016D-03Feb2017-v1/MINIAOD'),
	('data-RunE','/SingleElectron/Run2016E-03Feb2017-v1/MINIAOD'),
	('data-RunF','/SingleElectron/Run2016F-03Feb2017-v1/MINIAOD'),
	('data-RunG','/SingleElectron/Run2016G-03Feb2017-v1/MINIAOD'),
	('data-RunH_ver2','/SingleElectron/Run2016H-03Feb2017_ver2-v1/MINIAOD'),
	('data-RunH_ver3','/SingleElectron/Run2016H-03Feb2017_ver3-v1/MINIAOD')
]
	
MyJSON = "https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions16/13TeV/ReReco/Final/Cert_271036-284044_13TeV_23Sep2016ReReco_Collisions16_JSON.txt"
	
MCBackgroundsSampleDictionary=collections.OrderedDict(MCBackgroundsSampleDictionaryUnordered)
SignalMCSampleDictionary=collections.OrderedDict(SignalMCSampleDictionaryUnordered)
DataDictionaryMuonChannel=collections.OrderedDict(DataDictionaryMuonChannelUnordered)
DataDictionaryElectronChannel=collections.OrderedDict(DataDictionaryElectronChannelUnordered)

if __name__ == "__main__":
	submitJobs(MCBackgroundsSampleDictionary, SignalMCSampleDictionary, DataDictionaryElectronChannel, DataDictionaryMuonChannel, MyJSON,"271036-284044", True)

