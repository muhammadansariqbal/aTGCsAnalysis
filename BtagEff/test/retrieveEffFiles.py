import os
import sys
import fileinput
import optparse
import sys
import re

#This is a script to make a life of analyzer easier. 
# 1) check the status of the task
# 2) if the task is 100% complete retrieve it
# 3) merge files and clean the directory
# 4) move the file the configurable directory

def checkTaskStatus(taskName):
	tmp = os.popen("crab status crabConfigs/crab_projects/crab_" + taskName +  " | grep 'finished'").read()
	print "\033[0;40;32m checking the status of the task : " + taskName +  "\033[0m"
	print "\033[0;40;32m " + tmp + " \033[0m"
	print re.findall("\d+", tmp) 
	return re.findall("\d+", tmp)[0]


def RetrieveTask(taskName, outputFileName, outDir):
	if checkTaskStatus(taskName) == '100':
 		#os.system("crab getoutput crabConfigs/crab_projects/crab_" + taskName)
 		tmpOut = os.popen("crab getoutput crabConfigs/crab_projects/crab_" + taskName ).read()
 		if tmpOut.find("All files successfully retrieved") != -1 :
 			print "\033[0;40;32mAll files successfully retrieved : " + taskName +  "\033[0m"
 		else :
 			print "\033[0;40;31mNot all of files were retrieved, let's try once more! \033[0m"
 			tmpOut = os.popen("crab getoutput crabConfigs/crab_projects/crab_" + taskName ).read()
 			if tmpOut.find("All files successfully retrieved") != -1 :
 				print "\033[0;40;32mAll files successfully retrieved : " + taskName +  "\033[0m"
 			else :
 				print "\033[0;40;31mAfter 2 attempts not all of files were retrieved! \033[0m"
 				sys.exit(0)
 		os.chdir("crabConfigs/crab_projects/crab_" + taskName + "/results/")
 		os.system("hadd " + outputFileName + ".root" + " btag_tree_*.root")
 		os.system("rm btag_tree_*.root")
 		os.system("mv " + outputFileName + ".root  " +  outDir )
 		os.chdir("../../../../")
 		print "\033[0;40;32m task : "  +  taskName +  " retrieved successfully. \033[0m"
 	else :
 		os.system("crab resubmit -d crabConfigs/crab_projects/crab_" + taskName)	
 		print "\033[0;40;31m task is not retrieved as it's not 100% finished : " + taskName +  "\033[0m"


TaskDictionaryName = {
	"WW":"WW",
	"WZ":"WZ",
	"WW-signal": "WW-aTGC",
	"WZ-signal": "WZ-aTGC",
	"ttbar": "ttbar",

	"WJets_Pt-100To250":"WJets_Pt-100To250",
	"WJets_Pt-100To250-ext1":"WJets_Pt-100To250-ext1",
	"WJets_Pt-100To250-ext4":"WJets_Pt-100To250-ext4",

	"WJets_Pt-250To400":"WJets_Pt-250To400",
	"WJets_Pt-250To400-ext1":"WJets_Pt-250To400-ext1",
	"WJets_Pt-250To400-ext4":"WJets_Pt-250To400-ext4",

	"WJets_Pt-400To600":"WJets_Pt-400To600",
	"WJets_Pt-400To600-ext1":"WJets_Pt-400To600-ext1",

	"WJets_Pt-600ToInf":"WJets_Pt-600ToInf",
	"WJets_Pt-600ToInf-ext1":"WJets_Pt-600ToInf-ext1",

}


def Retrieval(feature, outDir):
	if not os.path.exists(outDir):
		os.makedirs(outDir)

	for TaskName, OutName in TaskDictionaryName.items():
		RetrieveTask(TaskName  +  "_mu_" + feature, OutName + "_mu", outDir)
		RetrieveTask(TaskName  +  "_ele_" + feature, OutName + "_ele", outDir )


Retrieval("btag_eff_withLeptonSF_withCuts", "/afs/cern.ch/work/m/maiqbal/private/aTGC/Samples_80X_BtagEffs" )
