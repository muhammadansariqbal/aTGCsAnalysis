#include "aTGCsAnalysis/TreeMaker/interface/BTagCalibrationStandalone.h"
#include "TEfficiency.h"
#include "TH2.h"

/*
* the class calculates b-tag weight for the event
* what is being done following the description:
* https://twiki.cern.ch/twiki/bin/view/CMS/BTagSFMethods#1a_Event_reweighting_using_scale
* if the string of the efficiency file is empty then the returned weight is 1
* else the proper calculation is being done
* Ivan Shvetsov, March 2016
* muhsam ernahrt sich das Eichhornchen
*/
using namespace btag;
enum VARIATION {
	UP, DOWN, NOMINAL};

enum BTagUncertaintyType{
	BTAG, MISTAG, NOMINALTYPE};
 class BTagHelper{

	BTagCalibration calib;
	float MaxBJetPt;
	TH2D *hist_eff;
	TEfficiency *eff_b, *eff_c, *eff_udsg;
	double DiscrCut;
	std::string DiscrName;
	std::string BtagEffFileName;
	bool DONOTHING;
public:
	std::unique_ptr<BTagCalibrationReader>  reader, reader_up, reader_down;
	BTagHelper(std::string BtagEffFileName_, double BtagDiscrCut):
		DiscrCut(BtagDiscrCut)
	{
		if (BtagEffFileName_.empty()) DONOTHING = true;
		else DONOTHING = false;
		if (!DONOTHING){
			//scale factors from CSV file
			edm::FileInPath CSVFile("aTGCsAnalysis/TreeMaker/data/CSVv2_Moriond17_B_H.csv");
			edm::FileInPath EfficiencyFile(BtagEffFileName_);
			calib = BTagCalibration("CSVv2", CSVFile.fullPath());
			//nominal
			reader.reset(new BTagCalibrationReader(BTagEntry::OP_TIGHT, "central"));
			reader->load(calib, BTagEntry::FLAV_B, "mujets");
			reader->load(calib, BTagEntry::FLAV_C, "mujets");
			reader->load(calib, BTagEntry::FLAV_UDSG, "incl");
			//up
			reader_up.reset(new BTagCalibrationReader(BTagEntry::OP_TIGHT, "up"));
			reader_up->load(calib, BTagEntry::FLAV_B, "mujets");
			reader_up->load(calib, BTagEntry::FLAV_C, "mujets");
			reader_up->load(calib, BTagEntry::FLAV_UDSG, "incl");
			//down
			reader_down.reset(new BTagCalibrationReader(BTagEntry::OP_TIGHT, "down"));
			reader_down->load(calib, BTagEntry::FLAV_B, "mujets");
			reader_down->load(calib, BTagEntry::FLAV_C, "mujets");
			reader_down->load(calib, BTagEntry::FLAV_UDSG, "incl");
			MaxBJetPt = 1000;
			//get measured efficiencies
			TFile effFile(EfficiencyFile.fullPath().c_str());
			eff_b = (TEfficiency*)effFile.Get("BtagAnalyzer/BTaggingEff_b");
			eff_c = (TEfficiency*)effFile.Get("BtagAnalyzer/BTaggingEff_c");
			eff_udsg = (TEfficiency*)effFile.Get("BtagAnalyzer/BTaggingEff_udsg");
			hist_eff = (TH2D*) eff_b->GetPassedHistogram();//get histogram to get binning info
			DiscrName = "pfCombinedInclusiveSecondaryVertexV2BJetTags";
		}
	}
	template<class T> double getScaleFactor(T jet, VARIATION var=NOMINAL, BTagUncertaintyType BTagUncertaintyType_=NOMINALTYPE){
		float jetPt = jet.pt();
		uint flav = abs(jet.hadronFlavour());
      	if (jetPt>MaxBJetPt) jetPt = MaxBJetPt;
      	double jet_scalefactor;
      	if (var == NOMINAL){
      		if (BTagUncertaintyType_ != NOMINALTYPE) throw cms::Exception("InvalidValue") << "BTagUncertaintyType is not valid, should be NOMINALTYPE in this case." << std::endl;
	      	if (flav == 5) jet_scalefactor = reader->eval(BTagEntry::FLAV_B, jet.eta(), jetPt);
	      	else if (flav == 4) jet_scalefactor = reader->eval(BTagEntry::FLAV_C, jet.eta(), jetPt);
	      	else jet_scalefactor = reader->eval(BTagEntry::FLAV_UDSG, jet.eta(), jetPt);
	      }
	    else if (var == UP){
	    	if (BTagUncertaintyType_ == BTAG){
	    		if (flav == 5) jet_scalefactor = reader_up->eval(BTagEntry::FLAV_B, jet.eta(), jetPt);
	      		else if (flav == 4) jet_scalefactor = reader_up->eval(BTagEntry::FLAV_C, jet.eta(), jetPt);
	      		else jet_scalefactor = reader->eval(BTagEntry::FLAV_UDSG, jet.eta(), jetPt);
	      	}
	      	else if (BTagUncertaintyType_ == MISTAG){
	    		if (flav == 5) jet_scalefactor = reader->eval(BTagEntry::FLAV_B, jet.eta(), jetPt);
	      		else if (flav == 4) jet_scalefactor = reader->eval(BTagEntry::FLAV_C, jet.eta(), jetPt);
	      		else jet_scalefactor = reader_up->eval(BTagEntry::FLAV_UDSG, jet.eta(), jetPt);
	      	}
	      	else throw cms::Exception("InvalidValue") << "BTagUncertaintyType is not valid." << std::endl;
	    }
	    else if (var == DOWN){
	    	if (BTagUncertaintyType_ == BTAG){
	    		if (flav == 5) jet_scalefactor = reader_down->eval(BTagEntry::FLAV_B, jet.eta(), jetPt);
	      		else if (flav == 4) jet_scalefactor = reader_down->eval(BTagEntry::FLAV_C, jet.eta(), jetPt);
	      		else jet_scalefactor = reader->eval(BTagEntry::FLAV_UDSG, jet.eta(), jetPt);
	      	}
	      	else if (BTagUncertaintyType_ == MISTAG){
	    		if (flav == 5) jet_scalefactor = reader->eval(BTagEntry::FLAV_B, jet.eta(), jetPt);
	      		else if (flav == 4) jet_scalefactor = reader->eval(BTagEntry::FLAV_C, jet.eta(), jetPt);
	      		else jet_scalefactor = reader_down->eval(BTagEntry::FLAV_UDSG, jet.eta(), jetPt);
	      	}
	      	else throw cms::Exception("InvalidValue") << "BTagUncertaintyType is not valid." << std::endl;
	    }
	    else throw cms::Exception("InvalidValue") << "Variation type is not valid." << std::endl;
		return jet_scalefactor;
	}
	template<class T> double getEfficiency(T jet){
		int ptBin = hist_eff ->GetXaxis() -> FindBin(jet.pt());
		int etaBin = hist_eff ->GetYaxis() -> FindBin(jet.eta());
		if (jet.pt() >= hist_eff -> GetXaxis() -> GetBinUpEdge(hist_eff -> GetNbinsX()) ) ptBin =  hist_eff -> GetNbinsX();//protection in case we go above the binning limit
		double efficiency;
		uint flav = abs(jet.hadronFlavour());
		if (flav == 5) efficiency = eff_b->GetEfficiency(eff_b->GetGlobalBin(ptBin,etaBin));
		else if (flav == 4) efficiency = eff_c->GetEfficiency(eff_c->GetGlobalBin(ptBin,etaBin));
		else efficiency = eff_udsg->GetEfficiency(eff_udsg->GetGlobalBin(ptBin,etaBin));
		return efficiency;
	}
	template<class T> double getEventWeight(edm::Handle<edm::View<T>> jets, VARIATION var=NOMINAL, BTagUncertaintyType BTagUncertaintyType_=NOMINALTYPE){
		if (DONOTHING) return 1.;
		else {
			double probabMC = 1., probabData = 1.;
			for(unsigned int iBtag = 0; iBtag < jets -> size(); iBtag ++)
			{
				if (jets->at(iBtag).bDiscriminator(DiscrName) > DiscrCut){
					probabMC *= getEfficiency(jets->at(iBtag));
					probabData *= getEfficiency(jets->at(iBtag)) * (getScaleFactor(jets->at(iBtag),var, BTagUncertaintyType_));
				}
				else {
					probabMC *= (1 - getEfficiency(jets->at(iBtag)));
					probabData *= (1 - getEfficiency(jets->at(iBtag))*getScaleFactor(jets->at(iBtag),var, BTagUncertaintyType_));
				}			

			}
			double weight = probabData/probabMC;
			// If NaN set as 0, since it will probably fail event cuts anyway
			if (weight != weight) weight = 0;
			return weight;
		}
	}

};
