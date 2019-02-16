#include <TFile.h>
#include <TTree.h>
#include <string>

void removeTree(string fileName)
{
	TFile *file=new TFile(fileName.c_str(),"update");
	gDirectory->Delete("BasicTree;1");
	file->Close();

}

void deleteWeightSamples()
{

	std::string prefix = "/afs/cern.ch/work/m/maiqbal/private/aTGC/Samples_80X_Working/";

	removeTree(prefix + "WJets_Pt100To250_ele.root");
	removeTree(prefix + "WJets_Pt250To400_ele.root");
	removeTree(prefix + "WJets_Pt400To600_ele.root");
	removeTree(prefix + "WJets_Pt600ToInf_ele.root");
	removeTree(prefix + "ttbar_ele.root");
	removeTree(prefix + "WW_ele.root");
	removeTree(prefix + "WZ_ele.root");
	removeTree(prefix + "s-ch_ele.root");
	removeTree(prefix + "t-ch-top_ele.root");
	removeTree(prefix + "t-ch-antitop_ele.root");
	removeTree(prefix + "tW-ch-top_ele.root");
	removeTree(prefix + "tW-ch-antitop_ele.root");
	removeTree(prefix + "WW-aTGC_MWW-600To800_ele.root");
	removeTree(prefix + "WW-aTGC_MWW-800ToInf_ele.root");
	removeTree(prefix + "WZ-aTGC_MWZ-600To800_ele.root");
	removeTree(prefix + "WZ-aTGC_MWZ-800ToInf_ele.root");
	
	removeTree(prefix + "WJets_Pt100To250_mu.root");
	removeTree(prefix + "WJets_Pt250To400_mu.root");
	removeTree(prefix + "WJets_Pt400To600_mu.root");
	removeTree(prefix + "WJets_Pt600ToInf_mu.root");
	removeTree(prefix + "ttbar_mu.root");
	removeTree(prefix + "WW_mu.root");
	removeTree(prefix + "WZ_mu.root");
	removeTree(prefix + "s-ch_mu.root");
	removeTree(prefix + "t-ch-top_mu.root");
	removeTree(prefix + "t-ch-antitop_mu.root");
	removeTree(prefix + "tW-ch-top_mu.root");
	removeTree(prefix + "tW-ch-antitop_mu.root");
	removeTree(prefix + "WW-aTGC_MWW-600To800_mu.root");
	removeTree(prefix + "WW-aTGC_MWW-800ToInf_mu.root");
	removeTree(prefix + "WZ-aTGC_MWZ-600To800_mu.root");
	removeTree(prefix + "WZ-aTGC_MWZ-800ToInf_mu.root");

}
