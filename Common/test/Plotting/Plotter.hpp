#ifndef  PLOTTER
#define PLOTTER

#include <TFile.h>
#include <TTree.h>
#include <TChain.h>
#include <TTreeFormula.h>
#include <TH1.h>
#include <TH2.h>
#include <THStack.h>
#include <TPad.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TPaveText.h>
#include <TLegend.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <map>
#include <algorithm>
#include <stdio.h>      
#include <stdlib.h>  
#include <Sample.hpp>
#include <var.hpp>
#include <TASImage.h>
#include <tdrstyle.hpp>
#include <CMS_lumi.hpp>
#include <PDFUtils.hpp>
#include <boost/algorithm/string/replace.hpp>
#include "boost/algorithm/string.hpp"
#include <boost/progress.hpp>
#include "SystHelper.hpp"
#include <TExec.h>

/*
 * Class to make data-MC comparison plots
 */

typedef unsigned int uint;

enum CHANNEL {
	ELECTRON,	MUON
};

enum CR {
	DEFAULT,	WJETS,	TTBAR
};

class Plotter
{
   	int Nbins;
	double lumi, lumiNormFactor;
	vector <Sample> samples;
	CHANNEL channel;
	vector <Var> variables;
	std::map<std::pair<std::string, std::string>, Var*> SystematicsVarMapUp, SystematicsVarMapDown;

	public:
	CR contReg = DEFAULT;
	TFile *fileToWriteHists;
	TFile *fileToWriteHistsSignal;
	std::string varToWrite;
	Var *varToWriteObj;
	Sample DataSample;
	Sample SignalSample;
	bool withData = true;
	bool withSystematics = true;
	bool withSignal = true;
	bool withMC = true;
	bool wantToWriteHists = false;
	std::string cat="default";
	
	Plotter();
	Plotter(CHANNEL channel_);
	void SetNbins(int Nbins_);
	void setLumi(double, double);
	void SetVar(vector <Var> variables_);
	void SetSamples(vector <Sample> samples_);	
	void SetDataSample(Sample sample_);
	void Plotting(std::string OutPrefix_);
	void PlottingVar(std::string OutPrefix_, Var var);
};

#endif
