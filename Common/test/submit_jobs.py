import os
import sys
import fileinput
import optparse
import sys
import shutil
import subprocess
import collections
import re


# Cached num files per dataset, to avoid unnecessary das_client calls
CACHED_DATASETS = {}


def DefineNJobs(sample): 
	N = 0
	if sample in CACHED_DATASETS:
		N = CACHED_DATASETS[sample]
	else:
		output = subprocess.check_output(['dasgoclient', '--query', "file dataset=" + sample])
		#for line in output.splitlines():
		#	if "count(file.name)=" in line :
		#		replacedline = line.replace("count(file.name)=","")
		N = int(len(output.splitlines()))
		CACHED_DATASETS[sample] = N

	minNFiles = 100
	if N < minNFiles :
		NFilesPerJob = 1
	else :
		# let's roughly assume that number of files is ~1000 in the worst case
		NFilesPerJob = int(N/minNFiles)
	print "N of files per job : " , sample , " " , NFilesPerJob	
	return NFilesPerJob


def copy_customise_analysis_config(template, output_filename, btag_eff_file=None, vtag_sf=None, runBtoF=None):
	"""Create a CMSSW config file by copying a template, and customising it.

	Optional values for BtagEffFile, VTagSF, and runBtoF (in met corrections).
	Setting any of these to None will use whatever is in the template config.

	For btag_eff_file, the str should be relative to aTGCsAnalysis/TreeMaker/data/
	"""
	if not any((btag_eff_file, vtag_sf, (runBtoF is True or runBtoF is False))):
		shutil.copy(template, output_filename)
	else:
		BTagEfficiencyPattern = "BtagEffFile = cms.string("
		VTagSFPattern = "VTagSF = cms.double("
		RunPattern = "runBtoF=True"  # hmm be careful with this...what if it changes in the template?
		with open(template) as configFile, open(output_filename, "w") as outFile:
			for line in configFile:
				if btag_eff_file and BTagEfficiencyPattern in line :
					replaceWith = "                                    BtagEffFile = cms.string(\"aTGCsAnalysis/TreeMaker/data/%s\"),\n" % btag_eff_file
					outFile.write(replaceWith)
				elif vtag_sf and VTagSFPattern in line:
					replaceWith = "                                    VTagSF = cms.double(%f)" % vtag_sf
					outFile.write(replaceWith)
				elif (runBtoF is True or runBtoF is False) and RunPattern in line:
					outFile.write(line.replace(RunPattern, "runBtoF=%s" % runBtoF))
				else :
					outFile.write(line)


def createConfigFile(processName, channel, isMC, isSignal, runBtoF=True):
	config_outdir = "analysisConfigs"
	if not os.path.exists(config_outdir):
		os.makedirs(config_outdir)

	# Some common settings
	TemplateFileName = "../analysis_" + channel + "_MC.py"
	ConfigFileName = config_outdir + "/analysis_" + channel + "_" + processName + "_MC.py"
	VTagSF = 1.03
	NoVTagSF = 1.0
	btag_file = "eff_" + re.sub(r'-ext[124]?', '', processName) + "_" + channel + ".root"

	if isSignal and isMC :
		ConfigFileName = config_outdir + "/analysis_" + channel + "_signal.py"
		copy_customise_analysis_config("../analysis_" + channel + "_signal.py", ConfigFileName, btag_eff_file=btag_file)

	elif "WZ" in processName:
		copy_customise_analysis_config(TemplateFileName, ConfigFileName, btag_eff_file=btag_file, vtag_sf=VTagSF)

	elif "WW" in processName:
		copy_customise_analysis_config(TemplateFileName, ConfigFileName, btag_eff_file=btag_file, vtag_sf=VTagSF)

	elif "ttbar" in processName:
		copy_customise_analysis_config(TemplateFileName, ConfigFileName, btag_eff_file="eff_ttbar_"+channel+".root", vtag_sf=VTagSF)

	elif "WJets" in processName:
		copy_customise_analysis_config(TemplateFileName, ConfigFileName, btag_eff_file=btag_file, vtag_sf=NoVTagSF)

#	elif "ZJets" in processName:
#		copy_customise_analysis_config(TemplateFileName, ConfigFileName, btag_eff_file=btag_file, vtag_sf=NoVTagSF)

	elif "SingleTop-t-channel-top" in processName:
		copy_customise_analysis_config(TemplateFileName, ConfigFileName, btag_eff_file=btag_file, vtag_sf=NoVTagSF)

	elif "SingleTop-t-channel-antitop" in processName:
		copy_customise_analysis_config(TemplateFileName, ConfigFileName, btag_eff_file=btag_file, vtag_sf=NoVTagSF)

	elif "SingleTop-tW-channel-top" in processName:
		copy_customise_analysis_config(TemplateFileName, ConfigFileName, btag_eff_file=btag_file, vtag_sf=VTagSF)

	elif "SingleTop-tW-channel-antitop" in processName:
		copy_customise_analysis_config(TemplateFileName, ConfigFileName, btag_eff_file=btag_file, vtag_sf=VTagSF)

	elif "SingleTop-s-channel" in processName:
		copy_customise_analysis_config(TemplateFileName, ConfigFileName, btag_eff_file=btag_file, vtag_sf=NoVTagSF)

	elif isMC :
		copy_customise_analysis_config(TemplateFileName, ConfigFileName, btag_eff_file=btag_file, vtag_sf=VTagSF)

	elif not isMC :
		# DATA
		ConfigFileName = config_outdir + "/analysis_" + channel + "_" + processName + ".py"
		copy_customise_analysis_config("../analysis_" + channel + ".py", ConfigFileName, runBtoF=runBtoF)

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
		rtrn = subprocess.call("crab submit -c " + os.path.dirname(os.path.abspath(__file__)) + "/" + outDir + "/"  + processName + "-" + channel + ".py", shell=True)
		if rtrn == 0:
			print "\033[0;40;32mtask:", processName + "-" + channel, "was submitted!\033[0m"
		else:
			print "\033[0;40;31mtask:", processName + "-" + channel, "was not submitted!\033[0m"
	return;


parser =  optparse.OptionParser()
parser.add_option('-p', '--Feature', dest="Feature", default='my_feature')
(options, arg) = parser.parse_args()


def submitJobs(MCBackgroundsSampleDictionary, SignalMCSampleDictionary, DataDictionaryElectronChannel, DataDictionaryMuonChannel, JSONFile, YourRunRange,wantToSubmit=False):
#	for key in MCBackgroundsSampleDictionary:
#		ConfigFileName = createConfigFile(key, "mu", isMC=True, isSignal=False)
#		print key, " ", ConfigFileName
#		createFileForJob(key, "mu", MCBackgroundsSampleDictionary[key], options.Feature, ConfigFileName,  "crabConfigs",  JSONFile, YourRunRange, True, False, wantToSubmit)
#		ConfigFileName = createConfigFile(key, "ele", isMC=True, isSignal=False)
#		createFileForJob(key, "ele", MCBackgroundsSampleDictionary[key], options.Feature, ConfigFileName,  "crabConfigs",  JSONFile, YourRunRange, True, False, wantToSubmit)
#	for key in SignalMCSampleDictionary:
#		ConfigFileName = createConfigFile(key, "mu", isMC=True, isSignal=True)
#		createFileForJob(key, "mu", SignalMCSampleDictionary[key], options.Feature, ConfigFileName,  "crabConfigs",  JSONFile, YourRunRange, True, True, wantToSubmit)
#		ConfigFileName = createConfigFile(key, "ele", isMC=True, isSignal=True)
#		createFileForJob(key, "ele", SignalMCSampleDictionary[key], options.Feature, ConfigFileName, "crabConfigs", JSONFile, YourRunRange, True, True, wantToSubmit)
	for key in DataDictionaryElectronChannel:
		runBtoF = not ("RunG" in key or "RunH" in key)
		ConfigFileName = createConfigFile(key, "ele", isMC=False, isSignal=False, runBtoF=runBtoF)
		createFileForJob(key, "ele", DataDictionaryElectronChannel[key], options.Feature, ConfigFileName,  "crabConfigs",  JSONFile, YourRunRange, False, True, wantToSubmit)
	for key in DataDictionaryMuonChannel:
		runBtoF = not ("RunG" in key or "RunH" in key)
		ConfigFileName = createConfigFile(key, "mu", isMC=False, isSignal=False, runBtoF=runBtoF)
		createFileForJob(key, "mu", DataDictionaryMuonChannel[key], options.Feature, ConfigFileName,  "crabConfigs",  JSONFile, YourRunRange, False, True, wantToSubmit)

MCBackgroundsSampleDictionaryUnordered =[
	('ttbar-powheg','/TT_TuneCUETP8M2T4_13TeV-powheg-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/MINIAODSIM'),

#	('WJets_HT-100To200','/WJetsToLNu_HT-100To200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/MINIAODSIM'),
#	('WJets_HT-100To200-ext1','/WJetsToLNu_HT-100To200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1/MINIAODSIM'),
#	('WJets_HT-100To200-ext2','/WJetsToLNu_HT-100To200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext2-v1/MINIAODSIM'),

#	('WJets_HT-200To400','/WJetsToLNu_HT-200To400_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/MINIAODSIM'),
#	('WJets_HT-200To400-ext1','/WJetsToLNu_HT-200To400_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1/MINIAODSIM'),
#	('WJets_HT-200To400-ext2','/WJetsToLNu_HT-200To400_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext2-v1/MINIAODSIM'),

#	('WJets_HT-400To600','/WJetsToLNu_HT-400To600_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/MINIAODSIM'),
#	('WJets_HT-400To600-ext','/WJetsToLNu_HT-400To600_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1/MINIAODSIM'),

#	('WJets_HT-600To800','/WJetsToLNu_HT-600To800_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/MINIAODSIM'),
#	('WJets_HT-600To800-ext','/WJetsToLNu_HT-600To800_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1/MINIAODSIM'),

#	('WJets_HT-800To1200','/WJetsToLNu_HT-800To1200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/MINIAODSIM'),
#	('WJets_HT-800To1200-ext','/WJetsToLNu_HT-800To1200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1/MINIAODSIM'),

#	('WJets_HT-1200To2500','/WJetsToLNu_HT-1200To2500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/MINIAODSIM'),
#	('WJets_HT-1200To2500-ext','/WJetsToLNu_HT-1200To2500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1/MINIAODSIM'),

#	('WJets_HT-2500ToInf','/WJetsToLNu_HT-2500ToInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/MINIAODSIM'),
#	('WJets_HT-2500ToInf-ext','/WJetsToLNu_HT-2500ToInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1/MINIAODSIM'),

#	('ZJets','/DYJetsToLL_M-50_Zpt-150toInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/MINIAODSIM')

	('WJets_Pt-100To250','/WJetsToLNu_Pt-100To250_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/MINIAODSIM'),
	('WJets_Pt-100To250-ext1','/WJetsToLNu_Pt-100To250_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1/MINIAODSIM'),
	('WJets_Pt-100To250-ext4','/WJetsToLNu_Pt-100To250_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext4-v1/MINIAODSIM'),

	('WJets_Pt-250To400','/WJetsToLNu_Pt-250To400_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/MINIAODSIM'),
	('WJets_Pt-250To400-ext1','/WJetsToLNu_Pt-250To400_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1/MINIAODSIM'),
	('WJets_Pt-250To400-ext4','/WJetsToLNu_Pt-250To400_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext4-v1/MINIAODSIM'),

	('WJets_Pt-400To600','/WJetsToLNu_Pt-400To600_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/MINIAODSIM'),
	('WJets_Pt-400To600-ext1','/WJetsToLNu_Pt-400To600_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1/MINIAODSIM'),

	('WJets_Pt-600ToInf','/WJetsToLNu_Pt-600ToInf_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/MINIAODSIM'),
	('WJets_Pt-600ToInf-ext1','/WJetsToLNu_Pt-600ToInf_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1/MINIAODSIM'),

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

# 2016

#DataDictionaryMuonChannelUnordered = [
#	('data-RunB_ver1','/SingleMuon/Run2016B-03Feb2017_ver1-v1/MINIAOD'),
#	('data-RunB_ver2','/SingleMuon/Run2016B-03Feb2017_ver2-v2/MINIAOD'),
#	('data-RunC','/SingleMuon/Run2016C-03Feb2017-v1/MINIAOD'),
#	('data-RunD','/SingleMuon/Run2016D-03Feb2017-v1/MINIAOD'),
#	('data-RunE','/SingleMuon/Run2016E-03Feb2017-v1/MINIAOD'),
#	('data-RunF','/SingleMuon/Run2016F-03Feb2017-v1/MINIAOD'),
#	('data-RunG','/SingleMuon/Run2016G-03Feb2017-v1/MINIAOD'),
#	('data-RunH_ver2','/SingleMuon/Run2016H-03Feb2017_ver2-v1/MINIAOD'),
#	('data-RunH_ver3','/SingleMuon/Run2016H-03Feb2017_ver3-v1/MINIAOD')
#]

#DataDictionaryElectronChannelUnordered = [
#	('data-RunB_ver1','/SingleElectron/Run2016B-03Feb2017_ver1-v1/MINIAOD'),
#	('data-RunB_ver2','/SingleElectron/Run2016B-03Feb2017_ver2-v2/MINIAOD'),
#	('data-RunC','/SingleElectron/Run2016C-03Feb2017-v1/MINIAOD'),
#	('data-RunD','/SingleElectron/Run2016D-03Feb2017-v1/MINIAOD'),
#	('data-RunE','/SingleElectron/Run2016E-03Feb2017-v1/MINIAOD'),
#	('data-RunF','/SingleElectron/Run2016F-03Feb2017-v1/MINIAOD'),
#	('data-RunG','/SingleElectron/Run2016G-03Feb2017-v1/MINIAOD'),
#	('data-RunH_ver2','/SingleElectron/Run2016H-03Feb2017_ver2-v1/MINIAOD'),
#	('data-RunH_ver3','/SingleElectron/Run2016H-03Feb2017_ver3-v1/MINIAOD')
#]

# 2017

#DataDictionaryMuonChannelUnordered = [
#	('data-RunB','/SingleMuon/Run2017B-17Nov2017-v1/MINIAOD'),
#	('data-RunC','/SingleMuon/Run2017C-17Nov2017-v1/MINIAOD'),
#	('data-RunD','/SingleMuon/Run2017D-17Nov2017-v1/MINIAOD'),
#	('data-RunE','/SingleMuon/Run2017E-17Nov2017-v1/MINIAOD'),
#	('data-RunF','/SingleMuon/Run2017F-17Nov2017-v1/MINIAOD')
#]

#DataDictionaryElectronChannelUnordered = [
#	('data-RunB','/SingleElectron/Run2017B-17Nov2017-v1/MINIAOD'),
#	('data-RunC','/SingleElectron/Run2017C-17Nov2017-v1/MINIAOD'),
#	('data-RunD','/SingleElectron/Run2017D-17Nov2017-v1/MINIAOD'),
#	('data-RunE','/SingleElectron/Run2017E-17Nov2017-v1/MINIAOD'),
#	('data-RunF','/SingleElectron/Run2017F-17Nov2017-v1/MINIAOD')
#]

# 2018

DataDictionaryMuonChannelUnordered = [
	('data-RunA','/SingleMuon/Run2018A-17Sep2018-v2/MINIAOD'),
	('data-RunB','/SingleMuon/Run2018B-17Sep2018-v1/MINIAOD'),
	('data-RunC','/SingleMuon/Run2018C-17Sep2018-v1/MINIAOD'),
	('data-RunD','/SingleMuon/Run2018D-PromptReco-v2/MINIAOD'),
]

DataDictionaryElectronChannelUnordered = [
	('data-RunA','/EGamma/Run2018A-17Sep2018-v2/MINIAOD'),
	('data-RunB','/EGamma/Run2018B-17Sep2018-v1/MINIAOD'),
	('data-RunC','/EGamma/Run2018C-17Sep2018-v1/MINIAOD'),
	('data-RunD','/EGamma/Run2018D-PromptReco-v2/MINIAOD'),
]
	
#MyJSON = "https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions16/13TeV/ReReco/Final/Cert_271036-284044_13TeV_23Sep2016ReReco_Collisions16_JSON.txt"
#MyJSON = "https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions17/13TeV/ReReco/Cert_294927-306462_13TeV_EOY2017ReReco_Collisions17_JSON.txt"
MyJSON = "https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions18/13TeV/ReReco/Cert_314472-325175_13TeV_17SeptEarlyReReco2018ABC_PromptEraD_Collisions18_JSON.txt"
	
MCBackgroundsSampleDictionary=collections.OrderedDict(MCBackgroundsSampleDictionaryUnordered)
SignalMCSampleDictionary=collections.OrderedDict(SignalMCSampleDictionaryUnordered)
DataDictionaryMuonChannel=collections.OrderedDict(DataDictionaryMuonChannelUnordered)
DataDictionaryElectronChannel=collections.OrderedDict(DataDictionaryElectronChannelUnordered)

if __name__ == "__main__":
	submitJobs(MCBackgroundsSampleDictionary, SignalMCSampleDictionary, DataDictionaryElectronChannel, DataDictionaryMuonChannel, MyJSON,"271036-325175", True)

