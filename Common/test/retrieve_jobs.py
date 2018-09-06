import os
import sys
import fileinput
import optparse
import sys
import re
import collections

#This is a script to make a life of analyzer easier. 
# 1) check the status of the task
# 2) if the task is 100% complete retrieve it
# 3) merge files and clean the directory
# 4) move the file the configurable directory

def checkTaskStatus(taskName):
	tmp = os.popen("crab status crab_projects/crab_" + taskName +  " | grep 'finished'").read()
	print "\033[0;40;32m checking the status of the task : " + taskName +  "\033[0m"
	print "\033[0;40;32m " + tmp + " \033[0m"
	return re.findall("\d+", tmp)[0]


def RetrieveTask(taskName, outputFileName, outDir):
	if checkTaskStatus(taskName) == '100':
 		tmpOut = os.popen("crab getoutput crab_projects/crab_" + taskName ).read()
 		if tmpOut.find("All files successfully retrieved") != -1 :
 			print "\033[0;40;32mAll files successfully retrieved : " + taskName +  "\033[0m"
 		else :
 			print "\033[0;40;31mNot all of files were retrieved, let's try once more! \033[0m"
 			tmpOut = os.popen("crab getoutput crab_projects/crab_" + taskName ).read()
 			if tmpOut.find("All files successfully retrieved") != -1 :
 				print "\033[0;40;32mAll files successfully retrieved : " + taskName +  "\033[0m"
 			else :
 				print "\033[0;40;31mAfter 2 attempts not all of files were retrieved! \033[0m"
 				sys.exit(0)
 		os.chdir("crab_projects/crab_" + taskName + "/results/")
 		os.system("hadd " + outputFileName + ".root" + "  tree_*.root")
 		os.system("rm tree_*.root")
 		os.system("mv " + outputFileName + ".root  " +  outDir )
 		os.chdir("../../../")
 		print "\033[0;40;32m task : "  +  taskName +  " retrieved successfully. \033[0m"
 	else :
 		os.system("crab resubmit -d crab_projects/crab_" + taskName)	
 		print "\033[0;40;31m task is not retrieved as it's not 100% finished : " + taskName +  "\033[0m"



TaskDictionaryNameUnordered = [
	("ttbar-powheg", "ttbar"),

	("WJets_HT-100To200","WJets_Ht100To200"),
	("WJets_HT-100To200-ext1","WJets_Ht100To200-ext1"),
	("WJets_HT-100To200-ext2","WJets_Ht100To200-ext2"),

	("WJets_HT-200To400","WJets_Ht200To400"),
	("WJets_HT-200To400-ext1","WJets_Ht200To400-ext1"),
	("WJets_HT-200To400-ext2","WJets_Ht200To400-ext2"),

	("WJets_HT-400To600","WJets_Ht400To600"),
	("WJets_HT-400To600-ext","WJets_Ht400To600-ext"),

	("WJets_HT-600To800","WJets_Ht600To800"),
	("WJets_HT-600To800-ext","WJets_Ht600To800-ext"),

	("WJets_HT-800To1200","WJets_Ht800To1200"),
	("WJets_HT-800To1200-ext","WJets_Ht800To1200-ext"),

	("WJets_HT-1200To2500","WJets_Ht1200To2500"),
	("WJets_HT-1200To2500-ext","WJets_Ht1200To2500-ext"),

	("WJets_HT-2500ToInf","WJets_Ht2500ToInf"),
	("WJets_HT-2500ToInf-ext","WJets_Ht2500ToInf-ext"),

#	("ZJets","ZJets"),

#	("WJets_Pt-100To250","WJets_Pt100To250"),
#	("WJets_Pt-100To250-ext1","WJets_Pt100To250-ext1"),
#	("WJets_Pt-100To250-ext4","WJets_Pt100To250-ext4"),

#	("WJets_Pt-250To400","WJets_Pt250To400"),
#	("WJets_Pt-250To400-ext1","WJets_Pt250To400-ext1"),
#	("WJets_Pt-250To400-ext4","WJets_Pt250To400-ext4"),

#	("WJets_Pt-400To600","WJets_Pt400To600"),
#	("WJets_Pt-400To600-ext1","WJets_Pt400To600-ext1"),

#	("WJets_Pt-600ToInf","WJets_Pt600ToInf"),
#	("WJets_Pt-600ToInf-ext1","WJets_Pt600ToInf-ext1"),

	("WW","WW"),
	("WW-ext","WW-ext"),

	("WZ","WZ"),

	("SingleTop-t-channel-top","t-ch-top"),
	("SingleTop-t-channel-antitop","t-ch-antitop"),

	("SingleTop-s-channel","s-ch"),

	("SingleTop-tW-channel-top","tW-ch-top"),
	("SingleTop-tW-channel-antitop","tW-ch-antitop"),

	("WW-signal_MWW-600To800","WW-aTGC_MWW-600To800"),
	("WW-signal_MWW-800ToInf","WW-aTGC_MWW-800ToInf"),
	("WZ-signal_MWZ-600To800","WZ-aTGC_MWZ-600To800"),
	("WZ-signal_MWZ-800ToInf","WZ-aTGC_MWZ-800ToInf"),

	("data-RunB_ver2","data-RunB_ver2"),
	("data-RunC","data-RunC"),
	("data-RunD","data-RunD"),
	("data-RunE","data-RunE"),
	("data-RunF","data-RunF"),
	("data-RunG","data-RunG"),
	("data-RunH_ver2","data-RunH_ver2"),
	("data-RunH_ver3","data-RunH_ver3")
]

TaskDictionaryName=collections.OrderedDict(TaskDictionaryNameUnordered)

def Retrieval(feature, outDir):
	if not os.path.exists(outDir):
		os.makedirs(outDir)

	for TaskName, OutName in TaskDictionaryName.items():
		RetrieveTask(TaskName + "_mu_" + feature, OutName + "_mu", outDir)
		RetrieveTask(TaskName + "_ele_" + feature, OutName + "_ele", outDir )

if __name__ == '__main__':
	Retrieval("my_feature", "/afs/cern.ch/work/m/maiqbal/private/aTGC/Samples_80X_Working/" )
