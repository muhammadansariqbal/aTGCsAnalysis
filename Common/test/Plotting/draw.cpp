#include "Plotter.hpp"
#include <boost/program_options.hpp>
#include <iostream>

namespace po = boost::program_options;

void draw(std::string channel, std::string region, std::string tag, string prefix, bool withData_, bool withMC_, bool withSystematics_, bool withSignal_, bool wantToWriteHists_)
{
	vector <Var> variables;
	Var var;
	var.logscale = false;
	var.VarName = "Mjpruned";
	var.Title = "m_{jet pruned}";
	var.SetRange(40., 200.);
	var.nBins=30;
	variables.push_back(var);

	var.VarName = "jet_tau21_PUPPI";
	var.Title = "#tau_{21} PUPPI";
	var.SetRange(0., 0.7);
	variables.push_back(var);

	var.VarName = "jet_tau32_PUPPI";
	var.Title = "#tau_{32} PUPPI";
	var.SetRange(0., 1.);
	variables.push_back(var);

	var.VarName = "jet_tau2tau1";
	var.Title = "#tau_{21}";
	var.SetRange(0., 0.7);
	variables.push_back(var);

	var.VarName = "jet_tau3tau2";
	var.Title = "#tau_{32}";
	var.SetRange(0., 1.);
	variables.push_back(var);

	var.VarName = "MWW_SD";
	var.Title = "m_{WV} (GeV)";
	var.SetRange(900., 4500.);
	var.logscale = true;
	variables.push_back(var);

	var.logscale = false;

	var.VarName = "nPV";
	var.Title = "n_{PV}";
	var.SetRange(0., 75.);
	var.nBins=25;
	variables.push_back(var);
	var.nBins=30;
	
	var.VarName = "l_pt";
	var.Title = "p_{T, lepton}";
	var.SetRange(50., 800.);
	variables.push_back(var);

	var.VarName = "l_eta";
	var.Title = "#eta_{lepton}";
	var.SetRange(-2.5, 2.5);
	variables.push_back(var);

	var.VarName = "l_phi";
	var.Title = "#phi_{lepton}";
	var.SetRange(-3.5, 3.5);
	variables.push_back(var);

	var.VarName = "jet_pt";
	var.Title = "p_{T, jet}";
	var.SetRange(200., 1000.);
	variables.push_back(var);

	var.VarName = "jet2_pt";
	var.Title = "p_{T, jet}";
	var.SetRange(30., 200.);
	variables.push_back(var);

	var.VarName = "jet2_btag";
	var.Title = "btag_{jet}";
	var.SetRange(0., 1.);
	variables.push_back(var);

	var.VarName = "jet_eta";
	var.Title = "#eta_{jet}";
	var.SetRange(-3., 3.);
	variables.push_back(var);

	var.VarName = "jet_phi";
	var.Title = "#phi_{lepton}";
	var.SetRange(-3.5, 3.5);
	variables.push_back(var);

	var.VarName = "W_pt";
	var.Title = "p_{T, W}";
	var.SetRange(60., 1000.);
	variables.push_back(var);

	var.VarName = "W_eta";
	var.Title = "#eta_{W}";
	var.SetRange(-3., 3.);
	variables.push_back(var);

	var.VarName = "W_phi";
	var.Title = "#phi_{W}";
	var.SetRange(-3.5, 3.5);
	variables.push_back(var);

	


	var.VarName = "W_mass";
	var.Title = "m_{W lep}";
	var.SetRange(75., 200.);
	variables.push_back(var);

	var.VarName = "W_mt";
	var.Title = "m_{T, W lep}";
	var.SetRange(0., 1100.);
	variables.push_back(var);

	var.VarName = "pfMET";
	var.Title = "MET";
	var.SetRange(0., 900.);
	variables.push_back(var);

	var.VarName = "pfMETPhi";
	var.Title = "#phi(MET)";
	var.SetRange(-3.2, 3.2);
	variables.push_back(var);

	var.VarName = "jet_mass_softdrop_PUPPI";
	var.Title = "m_{PUPPI SD} (GeV)";
	var.SetRange(40., 150.);
	variables.push_back(var);

	var.VarName = "njets";
	var.Title = "n_{jets}";
	var.SetRange(0., 6.);
	variables.push_back(var);

	var.VarName = "nbtag";
	var.Title = "n_{btag}";
	var.SetRange(0., 6.);
	variables.push_back(var);

	var.VarName = "nLooseEle";
	var.Title = "nLooseEle";
	var.SetRange(0., 2.);
	variables.push_back(var);

	var.VarName = "nLooseMu";
	var.Title = "nLooseMu";
	var.SetRange(0., 2.);
	variables.push_back(var);


	var.VarName = "NAK8jet";
	var.Title = "n_{fat jets}";
	var.SetRange(0., 6.);
	variables.push_back(var);

	var.VarName = "N_lep_W";
	var.Title = "N_{lep W}";
	var.SetRange(-1., 6.);
	variables.push_back(var);

	var.VarName = "charge_W_lep";
	var.Title = "charge(W lep)";
	var.SetRange(-2., 2.);
	variables.push_back(var);

	var.VarName = "deltaR_LeptonWJet";
	var.Title = "#DeltaR(Lep, WJet)";
	var.SetRange(0., 7.);
	variables.push_back(var);

	var.VarName = "deltaPhi_LeptonMet";
	var.Title = "#Delta#phi(Lep, MET)";
	var.SetRange(-3.2, 3.2);
	variables.push_back(var);

	var.VarName = "deltaPhi_WJetMet";
	var.Title = "#Delta#phi(WJet, MET)";
	var.SetRange(-3.2, 3.2);
	variables.push_back(var);

	var.VarName = "deltaPhi_WJetWlep";
	var.Title = "#Delta#phi(WJet, WLep)";
	var.SetRange(-3.2, 3.2);
	variables.push_back(var);

	Plotter p;

	if (channel == "mu")p = Plotter(MUON);
	else if (channel == "ele" )  p = Plotter(ELECTRON);
	else exit(0);
	if (region == "WJets") p.contReg = WJETS;
	else if (region == "ttbar") p.contReg = TTBAR;
	vector <Sample> samples;
	p.varToWrite = "MWW_SD";
	p.SetVar(variables);
	p.SetNbins(30);
	p.setLumi(35922., 1.);

	
	string defaulCuts = "(jet_pt > 200. && jet_tau21_PUPPI < 0.55  && jet_mass_softdrop_PUPPI < 150. && jet_mass_softdrop_PUPPI > 40. && W_pt > 200.  && abs(deltaR_LeptonWJet) > pi/2. && abs(deltaPhi_WJetMet) > 2. && abs(deltaPhi_WJetWlep) > 2. && MWW_SD > 900.";
	if (channel == "ele") defaulCuts += " && l_pt > 50. && pfMET > 110. )"; 
	else if (channel == "mu") defaulCuts += " && l_pt > 50. && pfMET > 40. )"; 
	else {
		std::cerr << "Invalid channel used, use ele or mu" << std::endl;
		exit(0);
	}
	string addOnCutWjets = defaulCuts +  " * ( (jet_mass_softdrop_PUPPI  < 65. || jet_mass_softdrop_PUPPI > 105. ) && nbtag == 0) ";
        string addOnCutTtbar = defaulCuts +  " * (nbtag > 0 )";

	string signalRegion = "(jet_pt > 200. && jet_tau21_PUPPI < 0.55  && jet_mass_softdrop_PUPPI < 105. && jet_mass_softdrop_PUPPI > 65. && W_pt > 200.  && abs(deltaR_LeptonWJet) > pi/2. && abs(deltaPhi_WJetMet) > 2. && abs(deltaPhi_WJetWlep) > 2. && MWW_SD > 900. && nbtag == 0";
	if (channel == "ele") signalRegion += " && l_pt > 50. && pfMET > 110. )"; 
	else if (channel == "mu") signalRegion += " && l_pt > 50. && pfMET > 40. )"; 
	else {
		std::cerr << "Invalid channel used, use ele or mu" << std::endl;
		exit(0);
	}

	string signalWWRegion = "(jet_pt > 200. && jet_tau21_PUPPI < 0.55  && jet_mass_softdrop_PUPPI < 85. && jet_mass_softdrop_PUPPI > 65. && W_pt > 200.  && abs(deltaR_LeptonWJet) > pi/2. && abs(deltaPhi_WJetMet) > 2. && abs(deltaPhi_WJetWlep) > 2. && MWW_SD > 900. && nbtag == 0";
	if (channel == "ele") signalWWRegion += " && l_pt > 50. && pfMET > 110. )";
        else if (channel == "mu") signalWWRegion += " && l_pt > 50. && pfMET > 40. )";
        else {
                std::cerr << "Invalid channel used, use ele or mu" << std::endl;
                exit(0);
        }

	string signalWZRegion = "(jet_pt > 200. && jet_tau21_PUPPI < 0.55  && jet_mass_softdrop_PUPPI < 105. && jet_mass_softdrop_PUPPI > 85. && W_pt > 200.  && abs(deltaR_LeptonWJet) > pi/2. && abs(deltaPhi_WJetMet) > 2. && abs(deltaPhi_WJetWlep) > 2. && MWW_SD > 900. && nbtag == 0";
	if (channel == "ele") signalWZRegion += " && l_pt > 50. && pfMET >110. )";
        else if (channel == "mu") signalWZRegion += " && l_pt > 50. && pfMET > 40. )";
        else {
                std::cerr << "Invalid channel used, use ele or mu" << std::endl;
                exit(0);
        }

	string TTBarEnrichedInclusive = "(jet_pt > 200.  &&  jet_tau21_PUPPI < 0.55  && jet_mass_softdrop_PUPPI < 200. && jet_mass_softdrop_PUPPI > 40. && W_pt > 200.  && abs(deltaR_LeptonWJet) > pi/2. && abs(deltaPhi_WJetMet) > 2. && abs(deltaPhi_WJetWlep) > 2. && MWW_SD > 900. ";
	if (channel == "ele") TTBarEnrichedInclusive += " && l_pt > 50. && pfMET > 110. )"; 
	else if (channel == "mu") TTBarEnrichedInclusive += " && l_pt > 50. && pfMET > 40. )"; 
	else {
		std::cerr << "Invalid channel used, use ele or mu" << std::endl;
		exit(0);
	}

	string TTBarEnrichedBTagVeto = "(jet_pt > 200.  &&  jet_tau21_PUPPI < 0.55  && jet_mass_softdrop_PUPPI < 200. && jet_mass_softdrop_PUPPI > 40. && W_pt > 200.  && abs(deltaR_LeptonWJet) > pi/2. && abs(deltaPhi_WJetMet) > 2. && abs(deltaPhi_WJetWlep) > 2. && MWW_SD > 900. && nbtag == 0 ";
	if (channel == "ele") TTBarEnrichedBTagVeto += " && l_pt > 50. && pfMET > 110. )"; 
	else if (channel == "mu") TTBarEnrichedBTagVeto += " && l_pt > 50. && pfMET > 40. )"; 
	else {
		std::cerr << "Invalid channel used, use ele or mu" << std::endl;
		exit(0);
	}

	std::string MCSelection,SignalSelection,DataSelection;

	// Apply trigger bit requirement
	if (channel == "ele") {
		DataSelection = "(bit_HLT_Ele_45 || bit_HLT_Ele_115) * ";
		//DataSelection = "bit_HLT_Ele_27_OR_45_OR_115 && ";
	} else {
		DataSelection = "";
	}

	if (region == "WJets"){
		MCSelection =  addOnCutWjets ;
		SignalSelection = "( " + addOnCutWjets + " )";
		DataSelection += addOnCutWjets;
	}
	else if(region == "ttbar"){
		MCSelection =  addOnCutTtbar;
		SignalSelection = "( " + addOnCutTtbar + " )";
		DataSelection += addOnCutTtbar;
	}
	else if(region == "TTBarEnrichedInclusive"){
		MCSelection =  TTBarEnrichedInclusive;
		SignalSelection = "( " + TTBarEnrichedInclusive + " )";
		DataSelection += TTBarEnrichedInclusive;
	}
	else if(region == "TTBarEnrichedBTagVeto"){
		MCSelection =  TTBarEnrichedBTagVeto;
		SignalSelection = "( " + TTBarEnrichedBTagVeto + " )";
		DataSelection += TTBarEnrichedBTagVeto;
	}
	else if(region == "signal"){
		MCSelection =  signalRegion;
		SignalSelection = "( " + signalRegion + " )";
		DataSelection += signalRegion;
	}
	else if(region == "signalWW"){
                MCSelection =  signalWWRegion;
                SignalSelection = "( " + signalWWRegion + " )";
                DataSelection += signalWWRegion;
        }
	else if(region == "signalWZ"){
                MCSelection =  signalWZRegion;
                SignalSelection = "( " + signalWZRegion + " )";
                DataSelection += signalWZRegion;
        }
	else std::cout << "This should not happen ..." << std::endl;

		
	/*
	 * Colors
	 * kWhite =0,   kBlack =1,   kGray=920,  kRed   =632, kGreen =416, kBlue=600, 
	 * kYellow=400, kMagenta=616, kCyan=432, kOrange=800, kSpring=820, kTeal=840,
	 * kAzure =860, kViolet =880, kPink=900
	 */
	
	Sample s, dataSample, signalSample;
	
	s.SetParameters("W+jets", MCSelection, kGreen+1);
 	/*s.SetFileNames(prefix + "WJets_Ht100To200_" + channel + ".root");
 	s.SetFileNames(prefix + "WJets_Ht200To400_" + channel + ".root");
 	s.SetFileNames(prefix + "WJets_Ht400To600_" + channel + ".root");
 	s.SetFileNames(prefix + "WJets_Ht600To800_" + channel + ".root");
 	s.SetFileNames(prefix + "WJets_Ht800To1200_" + channel + ".root");
 	s.SetFileNames(prefix + "WJets_Ht1200To2500_" + channel + ".root");
 	s.SetFileNames(prefix + "WJets_Ht2500ToInf_" + channel + ".root");*/
 	//rescale W+jets to the normalization from the fit in the pruned jet mass side
 	//if (channel == "ele")s.weight = 0.954;
 	//else if (channel == "mu")s.weight = 0.99;
 	//else {
 	//	std::cerr << "Wrong channel, use ele or mu" << std::endl;
 	//	exit(0);
 	//}
	s.SetFileNames(prefix + "WJets_Pt100To250_" + channel + ".root");
	s.SetFileNames(prefix + "WJets_Pt250To400_" + channel + ".root");
	s.SetFileNames(prefix + "WJets_Pt400To600_" + channel + ".root");
	s.SetFileNames(prefix + "WJets_Pt600ToInf_" + channel + ".root");
	samples.push_back(s);
	s.ReSet();

	s.SetParameters("t#bar{t}", MCSelection, kOrange);
 	s.SetFileNames(prefix + "ttbar_" + channel + ".root");
	samples.push_back(s);
	s.ReSet();

	s.SetParameters("WW", MCSelection, kRed);
 	s.SetFileNames( prefix + "WW_"+ channel + ".root");
	samples.push_back(s);
	s.ReSet();

	s.SetParameters("WZ", MCSelection, kRed+2);
 	s.SetFileNames( prefix + "WZ_"+ channel + ".root");
	samples.push_back(s);
	s.ReSet();

	s.SetParameters("Single top", MCSelection, kBlue);
        s.SetFileNames(prefix + "tW-ch-top_" + channel + ".root");
        s.SetFileNames(prefix + "tW-ch-antitop_" + channel + ".root");
        s.SetFileNames(prefix + "t-ch-top_" + channel + ".root");
        s.SetFileNames(prefix + "t-ch-antitop_" + channel + ".root");
        s.SetFileNames(prefix + "s-ch_" + channel + ".root");
        samples.push_back(s);
        s.ReSet();

	dataSample.SetParameters("data", DataSelection, kBlack);
 	dataSample.SetFileNames(prefix + "data_" + channel + ".root");

 	signalSample.SetParameters("#splitline{madgraph EWDim6}{c_{WWW} = 12 TeV^{-2}}", SignalSelection, kRed);
 	signalSample.SetFileNames(prefix + "WW-aTGC_"+ channel + ".root");
 	signalSample.SetFileNames(prefix + "WZ-aTGC_"+ channel + ".root");

	p.SetSamples(samples);
	p.DataSample = dataSample;
	p.SignalSample = signalSample;
	p.withSystematics = withSystematics_;
	p.withSignal = withSignal_;
	p.withData = withData_;
	p.withMC = withMC_;
	p.wantToWriteHists = wantToWriteHists_;
	if(region=="signalWW") p.cat="WW";
	else if(region=="signalWZ") p.cat="WZ";
 	p.Plotting(("plots_25ns_" + channel + "_" + tag + "/").c_str());

}

int main(int argc, char* argv[]){

	bool withMC, withData, withSystematics, withSignal, wantToWriteHists;
	po::options_description desc("Allowed options");
	desc.add_options()
	    ("help", "produce help message")
	    ("channel", po::value<std::string>(), "channel to be used: ele or mu")
	    ("CR", po::value<std::string>(), "control region to make plots with")
	    ("output", po::value<std::string>(), "tag for the output directory")
	    ("input", po::value<std::string>(), "name of input directory")
	    ("withMC",  "use Monte Carlo or not")
	    ("withData",  "use data or not")
	    ("withSignal",  "draw signal or not")
	    ("withSystematics", "calculate systematics or not. If not statistical uncertainties are calculated and drawn.")
	    ("wantToWriteHists", "to write histograms to the local file")
	;

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);    

	if (vm.count("help")) {
    	cout << desc << "\n";
    	return 1;
	}
	
	//channel
	if(vm.count("channel")) {
	  if (vm["channel"].as<std::string>() != "ele" && vm["channel"].as<std::string>() != "mu") {
	  	std::cerr << "Not supported channel" << std::endl;
	  	exit(0);
	  }
    }
    else {
    	std::cerr << "Channel wasn't specified" << std::endl;
    	return 0;
    }

    //control region
    if(vm.count("CR")) {
	  if (vm["CR"].as<std::string>() != "ttbar" && vm["CR"].as<std::string>() != "WJets" && \
	  	vm["CR"].as<std::string>() != "TTBarEnrichedInclusive" && vm["CR"].as<std::string>() != "TTBarEnrichedBTagVeto" &&  \
	  	vm["CR"].as<std::string>() != "signal" && vm["CR"].as<std::string>() != "signalWW" && vm["CR"].as<std::string>() != "signalWZ") {
	  	std::cerr << "Not supported region" << std::endl;
	  	exit(0);
	  }	   
    }
	else {
    	std::cerr << "Control region wasn't specified" << std::endl;
    	return 0;
    }
    //tag for the output directory
    if(!vm.count("output")) {
    	std::cerr << "output tag wasn't specified" << std::endl;
    	return 0;
    }

    //input directory
    if(!vm.count("input")) {
    	std::cerr << "input directory wasn't specified" << std::endl;
    	return 0;
    }

    if(vm.count("withMC"))withMC = true;
    else withMC = false;

    if(vm.count("withData")) withData = true;
    else withData = false;

    if(vm.count("withSystematics")) withSystematics = true;
    else withSystematics = false;

    if(vm.count("withSignal")) withSignal = true;
    else withSignal = false;

    if(vm.count("wantToWriteHists")) wantToWriteHists = true;
    else wantToWriteHists = false;

    if(withData && vm["CR"].as<std::string>() == "signal"){
    	std::cerr << "You are blinded, dude" << std::endl;
    	return 0;
    }

   	draw(vm["channel"].as<std::string>(), vm["CR"].as<std::string>(), vm["output"].as<std::string>(), vm["input"].as<std::string>(), withData, withMC, withSystematics, withSignal, wantToWriteHists);
}


