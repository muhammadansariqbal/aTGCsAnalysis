#include "JetMETCorrections/Modules/interface/JetResolution.h"
#include <random>

template<class T> class JetResolutionSmearer{
	bool isMC;
	JME::JetResolution resolution_pt;
	JME::JetResolution resolution_phi;
	JME::JetResolutionScaleFactor resolution_sf;
	double rho;
	std::default_random_engine generator;
public:
	JetResolutionSmearer(bool isMC_){
		isMC = isMC_;
		std::string prefix  = "aTGCsAnalysis/TreeMaker/data/";
		edm::FileInPath ResolutionFile_Pt(prefix + (isMC?"Spring16_25nsV10_MC/Spring16_25nsV10_MC_PtResolution_AK8PFchs.txt":"Spring16_25nsV6_DATA/Spring16_25nsV6_DATA_PtResolution_AK8PFchs.txt"));
		edm::FileInPath ResolutionFile_Phi(prefix + (isMC?"Spring16_25nsV10_MC/Spring16_25nsV10_MC_PhiResolution_AK8PFchs.txt":"Spring16_25nsV6_DATA/Spring16_25nsV6_DATA_PtResolution_AK8PFchs.txt"));
		edm::FileInPath SF_File(prefix + (isMC?"Spring16_25nsV10_MC/Spring16_25nsV10_MC_SF_AK8PFPuppi.txt":"Spring16_25nsV6_DATA/Spring16_25nsV6_DATA_SF_AK8PFchs.txt"));
		resolution_pt = JME::JetResolution(ResolutionFile_Pt.fullPath());
		resolution_phi = JME::JetResolution(ResolutionFile_Phi.fullPath());
		resolution_sf = JME::JetResolutionScaleFactor(SF_File.fullPath());
	}
	void setRhoAndSeed(double rho_, const edm::Event& iEvent){
		rho = rho;
		generator.seed(iEvent.eventAuxiliary().event());
	};
	double resolutionPt( T jet){
		JME::JetParameters parameters;
		parameters.setJetPt(jet.pt());
		parameters.setJetEta(jet.eta());
		parameters.setRho(rho);
		double r = resolution_pt.getResolution(parameters);
		return r;
	}
	double resolutionPhi( T jet){
		JME::JetParameters parameters;
		parameters.setJetPt(jet.pt());
		parameters.setJetEta(jet.eta());
		parameters.setRho(rho);
		double r = resolution_phi.getResolution(parameters);
		return r;
	}
	double ScaleFactor(T jet, Variation variation = Variation::NOMINAL){
		JME::JetParameters parameters = {{JME::Binning::JetEta, jet.eta()}};
		float sf = resolution_sf.getScaleFactor(parameters,variation);
		return sf;
	}
	double smearedPt(T jet, Variation variation = Variation::NOMINAL){
		double resolutionPt_ = resolutionPt(jet);
		double SF = ScaleFactor(jet,variation);
		double width = resolutionPt_ * sqrt(SF*SF - 1.);
  		std::normal_distribution<double> distribution(jet.pt(),width);
  		double pt = distribution(generator);
  		return pt;

	}
	//this is basically not supposed to be used in the analysis, consider only pt resolution uncetainty
	double smearedPhi(T jet, Variation variation = Variation::NOMINAL){
		double resolutionPhi_ = resolutionPhi(jet);
		double SF = ScaleFactor(jet,variation);
		double width = resolutionPhi_ * sqrt(SF*SF - 1.);
  		std::normal_distribution<double> distribution(jet.phi(),width);
  		double phi = distribution(generator);
  		return phi;

	}
	math::XYZTLorentzVector  LorentzVectorWithSmearedPt(T jet, Variation variation = Variation::NOMINAL){
		math::XYZTLorentzVector smearedP4;
		double pt = smearedPt(jet,variation);
		double SF_ = pt/jet.pt();
		smearedP4 = SF_*(jet.p4());
		return smearedP4;

	}
	double smearedCorrection(T jet, Variation variation = Variation::NOMINAL){
		double correction = smearedPt(jet, variation)/jet.pt();
		return correction;
	}

};
