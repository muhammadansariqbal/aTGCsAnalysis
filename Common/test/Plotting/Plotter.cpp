#include "Plotter.hpp"

Plotter::Plotter()
{
//do nothing
}


Plotter::Plotter(CHANNEL channel_)
{
	channel = channel_;
  
}


void Plotter::SetNbins(int Nbins_)
{
	Nbins = Nbins_;
}

void Plotter::setLumi(double lumi_, double lumiNormFactor_)
{
	lumi=lumi_;
	lumiNormFactor=lumiNormFactor_;
}

void Plotter::SetSamples(vector <Sample> samples_)
{
	samples = samples_;
}

void Plotter::SetDataSample(Sample sample_)
{
	DataSample = sample_;
}

void Plotter::SetVar(vector <Var> variables_)
{
  
  variables = variables_;
  //make the object var to write histograms with systematics
  for (uint iVar = 0; iVar < variables.size(); iVar++){
    if(variables.at(iVar).VarName == varToWrite) varToWriteObj = &variables.at(iVar);
  }

}

void Plotter::Plotting(std::string OutPrefix_)
{
  system(("mkdir -p " + OutPrefix_ ).c_str());
  system(("mkdir -p " + OutPrefix_ + "/png").c_str());
  system(("mkdir -p " + OutPrefix_ + "/pdf").c_str());

  std::string channelName;
  if (channel == MUON) channelName = "mu";
  else channelName = "ele";

  if (wantToWriteHists && cat=="WW") fileToWriteHists = new TFile(("hists_WW_" + channelName + ".root").c_str(), "RECREATE");
  else if (wantToWriteHists && cat=="WZ") fileToWriteHists = new TFile(("hists_WZ_" + channelName + ".root").c_str(), "RECREATE");
  else if (wantToWriteHists) fileToWriteHists = new TFile(("hists_" + channelName + ".root").c_str(), "RECREATE");

  if (withSignal && cat=="WW") fileToWriteHistsSignal = new TFile(("hists_signal_WW_" + channelName + ".root").c_str(), "RECREATE");
  else if (withSignal && cat=="WZ") fileToWriteHistsSignal = new TFile(("hists_signal_WZ_" + channelName + ".root").c_str(), "RECREATE");
  else if (withSignal) fileToWriteHistsSignal = new TFile(("hists_signal_" + channelName + ".root").c_str(), "RECREATE");

  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(0);
  setTDRStyle();
  // Prepare plots for the various variables 
  TCanvas* c1;
  TPad* pad1;
  TPad* pad2;
  std::map<std::string,TLegend*> leg;
  std::map<std::string,TH1D*> data;
  std::map<std::string,TH1D*> signalHist;
  std::map<std::string,THStack*> hs;
  std::map<std::string,TH1D*> hist_summed;
  std::map<std::string, TH1D*> hist_PDFUp;
  std::map<std::string, TH1D*> hist_PDFDown;
  std::map<std::string, TH1D*> hist_ScaleUp;
  std::map<std::string, TH1D*> hist_ScaleDown;
  std::map<std::pair<std::string,std::string>, TH1D*> hist_per_process;
  std::map<std::string, TH1D*> hist_per_process_SystUp;
  std::map<std::string, TH1D*> hist_per_process_SystDown;
  TH1D* hist_per_process_PDFUp;
  TH1D* hist_per_process_PDFDown;
  TH1D* hist_per_process_ScaleUp;
  TH1D* hist_per_process_ScaleDown;

  std::vector<std::string> SignalParameters = {"cwww","ccw","cb"};
  std::map<std::string, TH1D*> signalHistPerParPositive, signalHistPerParNegative;
  TH1D * SMhist;
  //PDF uncertainties
  std::map<std::string, TH1D*> signalHistPerParPositive_PDFUp, signalHistPerParPositive_PDFDown, signalHistPerParNegative_PDFUp, signalHistPerParNegative_PDFDown;
  //scale uncertainties
  std::map<std::string, TH1D*> signalHistPerParPositive_ScaleUp, signalHistPerParPositive_ScaleDown, signalHistPerParNegative_ScaleUp, signalHistPerParNegative_ScaleDown;
  TH1D * SMhist_PDFUp,* SMhist_PDFDown;
  TH1D * SMhist_ScaleUp,* SMhist_ScaleDown;
  //map between aTGC  and map of  the type of systematics  and hist
  std::map<std::string,std::map<std::string, TH1D*>> signalHistPerParPositive_SystUp,signalHistPerParPositive_SystDown, signalHistPerParNegative_SystUp, signalHistPerParNegative_SystDown;
  std::map<std::string, TH1D*> SMhist_SystUp,SMhist_SystDown;
  
  SystHelper systematics;
  if(withMC && withSystematics || withSignal) systematics = SystHelper(samples[0].selection);
  
  for (uint var_i = 0; var_i < variables.size(); ++ var_i )   {
    
//  std::cout<<variables.at(var_i).VarName<<"  "<<variables.at(var_i).nBins<<std::endl;

    std::string vname = variables.at(var_i).VarName;
    leg[vname] = new TLegend(0.68,0.54,0.94,0.94);
    leg[vname] ->  SetFillColor(kWhite);
    
    if(withData){
      data[vname] = new TH1D((DataSample.Processname + variables.at(var_i).VarName + "_data").c_str(),(DataSample.Processname + variables.at(var_i).VarName + "_data").c_str(), variables.at(var_i).nBins,variables.at(var_i).Range.low, variables.at(var_i).Range.high);
      data[vname] -> Sumw2();
    }
    
    if(withSignal){
      signalHist[vname] = new TH1D(("signal_" + variables.at(var_i).VarName ).c_str(),("signal_" + variables.at(var_i).VarName ).c_str(), variables.at(var_i).nBins, variables.at(var_i).Range.low, variables.at(var_i).Range.high);
      signalHist[vname]-> Sumw2();
    }
    
    if(withMC){
      //for Monte-Carlo samples
      hs[vname] = new THStack("hs",(";"+ vname +";Number of events").c_str());	  
      // sum of all processes for a given variable
      hist_summed[vname] = new TH1D((vname + "summed").c_str(),( vname+ "summed").c_str(), variables.at(var_i).nBins, variables.at(var_i).Range.low, variables.at(var_i).Range.high);
      hist_summed[vname] -> Sumw2();
    }
    if(withSystematics)
    {
      //PDF Up
      hist_PDFUp[vname] = new TH1D((vname + "summed_PDFUp").c_str(),( vname+ "summed_PDFUp").c_str(), variables.at(var_i).nBins, variables.at(var_i).Range.low, variables.at(var_i).Range.high);
      hist_PDFUp[vname] -> Sumw2();
      //PDF Down
      hist_PDFDown[vname] = new TH1D((vname + "summed_PDFDown").c_str(),( vname+ "summed_PDFDown").c_str(), variables.at(var_i).nBins, variables.at(var_i).Range.low, variables.at(var_i).Range.high);
      hist_PDFDown[vname] -> Sumw2();

       //Scale Up
      hist_ScaleUp[vname] = new TH1D((vname + "summed_ScaleUp").c_str(),( vname+ "summed_ScaleUp").c_str(), variables.at(var_i).nBins, variables.at(var_i).Range.low, variables.at(var_i).Range.high);
      hist_ScaleUp[vname] -> Sumw2();
      //PDF Down
      hist_ScaleDown[vname] = new TH1D((vname + "summed_ScaleDown").c_str(),( vname+ "summed_ScaleDown").c_str(), variables.at(var_i).nBins, variables.at(var_i).Range.low, variables.at(var_i).Range.high);
      hist_ScaleDown[vname] -> Sumw2();
      //systematics
      systematics.AddVar( &(variables.at(var_i)) ,  hist_summed[vname]);
    }
  }
  if(withSystematics){
    systematics.AddSyst(hist_PDFUp, hist_PDFDown);
    systematics.AddSyst(hist_ScaleUp, hist_ScaleDown);
  }

  if(withSignal){
    SMhist = new TH1D("SMhist","SMhist", Nbins,varToWriteObj->Range.low, varToWriteObj->Range.high);
    SMhist -> Sumw2();

    SMhist_PDFUp = new TH1D("SMhist_PDFUp","SMhist_PDFUp", Nbins,varToWriteObj->Range.low, varToWriteObj->Range.high);
    SMhist_PDFDown = new TH1D("SMhist_PDFDown","SMhist_PDFDown", Nbins,varToWriteObj->Range.low, varToWriteObj->Range.high);
    SMhist_PDFUp -> Sumw2();
    SMhist_PDFDown -> Sumw2();

    SMhist_ScaleUp = new TH1D("SMhist_ScaleUp","SMhist_ScaleUp", Nbins,varToWriteObj->Range.low, varToWriteObj->Range.high);
    SMhist_ScaleDown = new TH1D("SMhist_ScaleDown","SMhist_ScaleDown", Nbins,varToWriteObj->Range.low, varToWriteObj->Range.high);
    SMhist_ScaleUp -> Sumw2();
    SMhist_ScaleDown -> Sumw2();

    for (uint iSyst =0; iSyst < systematics.ListOfSystematics.size(); iSyst++)
    {
      std::string theSyst = systematics.ListOfSystematics[iSyst];
      
      SMhist_SystUp[theSyst] = new TH1D(("SMhist_" + theSyst + "Up").c_str(),("SMhist_" + theSyst + "Up").c_str(), Nbins,varToWriteObj->Range.low, varToWriteObj->Range.high);
      SMhist_SystDown[theSyst] = new TH1D(("SMhist_" + theSyst + "Down").c_str(),("SMhist_" + theSyst + "Down").c_str(), Nbins,varToWriteObj->Range.low, varToWriteObj->Range.high);
      
      SMhist_SystUp[theSyst] -> Sumw2();
      SMhist_SystDown[theSyst] -> Sumw2();

    }

    for (uint wSyst =0; wSyst < systematics.WeightNameSystematics.size(); wSyst++)
    {
      std::string theSyst = systematics.WeightNameSystematics[wSyst];
      
      SMhist_SystUp[theSyst] = new TH1D(("SMhist_" + theSyst + "Up").c_str(),("SMhist_" + theSyst + "Up").c_str(), Nbins,varToWriteObj->Range.low, varToWriteObj->Range.high);
      SMhist_SystDown[theSyst] = new TH1D(("SMhist_" + theSyst + "Down").c_str(),("SMhist_" + theSyst + "Down").c_str(), Nbins,varToWriteObj->Range.low, varToWriteObj->Range.high);
      
      SMhist_SystUp[theSyst] -> Sumw2();
      SMhist_SystDown[theSyst] -> Sumw2();

    }
  }
  //signal nominal histograms: positive and negative; basically only point with single non-zero value are used.
  for (uint iPar = 0; iPar < SignalParameters.size() && withSignal; iPar++){
      signalHistPerParPositive[SignalParameters.at(iPar)] = new TH1D(("signalPositive_" + SignalParameters.at(iPar) ).c_str(),("signalPositive_" + SignalParameters.at(iPar) ).c_str(), Nbins,varToWriteObj->Range.low, varToWriteObj->Range.high);
      signalHistPerParNegative[SignalParameters.at(iPar)] = new TH1D(("signalNegative_" + SignalParameters.at(iPar) ).c_str(),("signalNegative_" + SignalParameters.at(iPar) ).c_str(), Nbins,varToWriteObj->Range.low, varToWriteObj->Range.high);

      //PDF
      signalHistPerParPositive_PDFUp[SignalParameters.at(iPar)] = new TH1D(("signalPositive_" + SignalParameters.at(iPar) +"_PDFUp").c_str(),("signalPositive_" + SignalParameters.at(iPar) +"_PDFUp" ).c_str(), Nbins,varToWriteObj->Range.low, varToWriteObj->Range.high);
      signalHistPerParPositive_PDFDown[SignalParameters.at(iPar)] = new TH1D(("signalPositive_" + SignalParameters.at(iPar) +"_PDFDown").c_str(),("signalPositive_" + SignalParameters.at(iPar) +"_PDFDown" ).c_str(), Nbins,varToWriteObj->Range.low, varToWriteObj->Range.high);
      signalHistPerParNegative_PDFUp[SignalParameters.at(iPar)] = new TH1D(("signalNegative_" + SignalParameters.at(iPar) + "_PDFUp").c_str(),("signalNegative_" + SignalParameters.at(iPar) + "_PDFUp").c_str(), Nbins,varToWriteObj->Range.low, varToWriteObj->Range.high);
      signalHistPerParNegative_PDFDown[SignalParameters.at(iPar)] = new TH1D(("signalNegative_" + SignalParameters.at(iPar) + "_PDFDown").c_str(),("signalNegative_" + SignalParameters.at(iPar) + "_PDFDown").c_str(), Nbins,varToWriteObj->Range.low, varToWriteObj->Range.high);

      signalHistPerParPositive_PDFUp[SignalParameters.at(iPar)] -> Sumw2();
      signalHistPerParPositive_PDFDown[SignalParameters.at(iPar)] -> Sumw2();
      signalHistPerParNegative_PDFUp[SignalParameters.at(iPar)] -> Sumw2();
      signalHistPerParNegative_PDFDown[SignalParameters.at(iPar)] -> Sumw2();

      //Scale
      signalHistPerParPositive_ScaleUp[SignalParameters.at(iPar)] = new TH1D(("signalPositive_" + SignalParameters.at(iPar) +"_ScaleUp").c_str(),("signalPositive_" + SignalParameters.at(iPar) +"_ScaleUp" ).c_str(), Nbins,varToWriteObj->Range.low, varToWriteObj->Range.high);
      signalHistPerParPositive_ScaleDown[SignalParameters.at(iPar)] = new TH1D(("signalPositive_" + SignalParameters.at(iPar) +"_ScaleDown").c_str(),("signalPositive_" + SignalParameters.at(iPar) +"_ScaleDown" ).c_str(), Nbins,varToWriteObj->Range.low, varToWriteObj->Range.high);
      signalHistPerParNegative_ScaleUp[SignalParameters.at(iPar)] = new TH1D(("signalNegative_" + SignalParameters.at(iPar) + "_ScaleUp").c_str(),("signalNegative_" + SignalParameters.at(iPar) + "_ScaleUp").c_str(), Nbins,varToWriteObj->Range.low, varToWriteObj->Range.high);
      signalHistPerParNegative_ScaleDown[SignalParameters.at(iPar)] = new TH1D(("signalNegative_" + SignalParameters.at(iPar) + "_ScaleDown").c_str(),("signalNegative_" + SignalParameters.at(iPar) + "_ScaleDown").c_str(), Nbins,varToWriteObj->Range.low, varToWriteObj->Range.high);

      signalHistPerParPositive_ScaleUp[SignalParameters.at(iPar)] -> Sumw2();
      signalHistPerParPositive_ScaleDown[SignalParameters.at(iPar)] -> Sumw2();
      signalHistPerParNegative_ScaleUp[SignalParameters.at(iPar)] -> Sumw2();
      signalHistPerParNegative_ScaleDown[SignalParameters.at(iPar)] -> Sumw2();
      
      
      //systematics
      for (uint iSyst =0; iSyst < systematics.ListOfSystematics.size(); iSyst++){
        std::string theSyst = systematics.ListOfSystematics[iSyst];
        signalHistPerParPositive_SystUp[SignalParameters.at(iPar)][theSyst] = new TH1D(("signalPositive_" + SignalParameters.at(iPar) + "_" + theSyst + "Up").c_str(),("signalPositive_" + SignalParameters.at(iPar) + theSyst + "Up").c_str(), Nbins,varToWriteObj->Range.low, varToWriteObj->Range.high);
        signalHistPerParPositive_SystDown[SignalParameters.at(iPar)][theSyst] = new TH1D(("signalPositive_" + SignalParameters.at(iPar) + "_" + theSyst + "Down").c_str(),("signalPositive_" + SignalParameters.at(iPar) + theSyst + "Down").c_str(), Nbins,varToWriteObj->Range.low, varToWriteObj->Range.high);
        signalHistPerParNegative_SystUp[SignalParameters.at(iPar)][theSyst] = new TH1D(("signalNegative_" + SignalParameters.at(iPar) + "_" + theSyst + "Up").c_str(),("signalNegative_" + SignalParameters.at(iPar) + theSyst + "Up").c_str(), Nbins,varToWriteObj->Range.low, varToWriteObj->Range.high);
        signalHistPerParNegative_SystDown[SignalParameters.at(iPar)][theSyst] = new TH1D(("signalNegative_" + SignalParameters.at(iPar) + "_" + theSyst + "Down").c_str(),("signalNegative_" + SignalParameters.at(iPar) + theSyst + "Down").c_str(), Nbins,varToWriteObj->Range.low, varToWriteObj->Range.high);

        signalHistPerParPositive_SystUp[SignalParameters.at(iPar)][theSyst] -> Sumw2();
        signalHistPerParPositive_SystDown[SignalParameters.at(iPar)][theSyst] -> Sumw2();
        signalHistPerParNegative_SystUp[SignalParameters.at(iPar)][theSyst] -> Sumw2();
        signalHistPerParNegative_SystDown[SignalParameters.at(iPar)][theSyst] -> Sumw2();
      }
      //weighted systematics
      for (uint wSyst =0; wSyst < systematics.WeightNameSystematics.size(); wSyst++){
        std::string theSyst = systematics.WeightNameSystematics[wSyst];
        signalHistPerParPositive_SystUp[SignalParameters.at(iPar)][theSyst] = new TH1D(("signalPositive_" + SignalParameters.at(iPar) + "_" + theSyst + "Up").c_str(),("signalPositive_" + SignalParameters.at(iPar) + theSyst + "Up").c_str(), Nbins,varToWriteObj->Range.low, varToWriteObj->Range.high);
        signalHistPerParPositive_SystDown[SignalParameters.at(iPar)][theSyst] = new TH1D(("signalPositive_" + SignalParameters.at(iPar) + "_" + theSyst + "Down").c_str(),("signalPositive_" + SignalParameters.at(iPar) + theSyst + "Down").c_str(), Nbins,varToWriteObj->Range.low, varToWriteObj->Range.high);
        signalHistPerParNegative_SystUp[SignalParameters.at(iPar)][theSyst] = new TH1D(("signalNegative_" + SignalParameters.at(iPar) +  "_" + theSyst + "Up").c_str(),("signalNegative_" + SignalParameters.at(iPar) + theSyst + "Up").c_str(), Nbins,varToWriteObj->Range.low, varToWriteObj->Range.high);
        signalHistPerParNegative_SystDown[SignalParameters.at(iPar)][theSyst] = new TH1D(("signalNegative_" + SignalParameters.at(iPar) + "_" + theSyst + "Down").c_str(),("signalNegative_" + SignalParameters.at(iPar) + theSyst + "Down").c_str(), Nbins,varToWriteObj->Range.low, varToWriteObj->Range.high);

        signalHistPerParPositive_SystUp[SignalParameters.at(iPar)][theSyst] -> Sumw2();
        signalHistPerParPositive_SystDown[SignalParameters.at(iPar)][theSyst] -> Sumw2();
        signalHistPerParNegative_SystUp[SignalParameters.at(iPar)][theSyst] -> Sumw2();
        signalHistPerParNegative_SystDown[SignalParameters.at(iPar)][theSyst] -> Sumw2();
      }
  
  }//end of initialiazing histograms for signal positive and negative


  //begin loop over data files
  for (uint file_i = 0; file_i < DataSample.filenames.size() && withData; ++file_i){
    TFile file((DataSample.filenames.at(file_i)).c_str(), "READ");
    TTree * tree = (TTree*)file.Get("treeDumper/BasicTree");
    TTreeFormula *dataSelection = new TTreeFormula("dataSelection",DataSample.selection.c_str(),tree);//that should be without any weights!
    
    //initialize variables.
    for(auto var = variables.begin(); var != variables.end() ; var++){
      var->Initialize(tree);
    }
    //event loop
    Long64_t nentries = tree->GetEntriesFast();
    for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t nb = tree->GetEntry(jentry);
      if(nb<0) break; // reached the end of the ntuple
      
      // fill variables
      for(auto var = variables.begin(); var != variables.end() ; var++)
      {
	       std::string vname = var->VarName;
	       if(dataSelection -> EvalInstance())data[vname]->Fill(var->value());//check if the event passeds the selection, and if true fill the histogram
      }
    }
  } // end of the loop over data files
  
  //begin the loop over signal files      
    for (uint file_i = 0; file_i < SignalSample.filenames.size() && withSignal; ++file_i){ 
    TFile file((SignalSample.filenames.at(file_i)).c_str(), "READ");
    TTree * tree = (TTree*)file.Get("BasicTree");std::cout<<(SignalSample.filenames.at(file_i)).c_str()<<std::endl;
    TTreeFormula *signalSelection = new TTreeFormula("signalSelection",SignalSample.selection.c_str(),tree);//that should be without any weights!
    Double_t totEventWeight;
    std::vector<double> * aTGCWeights = 0;
    std::vector<double> *PDFWeights = 0;
    std::vector<double> *ScaleWeights = 0;
    tree -> SetBranchAddress("PDFWeights", &PDFWeights);
    tree -> SetBranchAddress("ScaleWeights", &ScaleWeights);
    tree -> SetBranchAddress("totEventWeight", &totEventWeight);
    tree -> SetBranchAddress("aTGCWeights", &aTGCWeights);
    //map of aTGC and vector of PDF hists
    std::map<std::string,std::vector<TH1D*>> histsPDFSignalPerFilePositive,histsPDFSignalPerFileNegative ;
    //map of aTGC and vector of Scale hists
    std::map<std::string,std::vector<TH1D*>> histsScaleSignalPerFilePositive,histsScaleSignalPerFileNegative ;
    std::vector<TH1D*> SMhistsPDF, SMhistsScale;
    
    //initialize variables.
    for(auto var = variables.begin(); var != variables.end() ; var++){
      var->Initialize(tree);
    }

    std::pair<std::string,std::string> key(varToWriteObj->VarName,std::string(""));

    for (uint iSyst = 0;iSyst < systematics.ListOfSystematics.size(); iSyst ++)
    {  
        key.second=systematics.ListOfSystematics.at(iSyst);  
        if (systematics.isAffectedBySystematic(*varToWriteObj, systematics.ListOfSystematics.at(iSyst))){
          Var *varUp = new Var();
          Var *varDown = new Var();
          varUp->VarName = (varToWriteObj -> VarName )+ "_" + systematics.ListOfSystematics.at(iSyst) + "Up";
          varDown->VarName = (varToWriteObj -> VarName )+ "_" + systematics.ListOfSystematics.at(iSyst) + "Down";
          varUp->Initialize(tree);
          varDown->Initialize(tree);
          SystematicsVarMapUp[key] = varUp;
          SystematicsVarMapDown[key] = varDown;
        }
        else {
          SystematicsVarMapUp[key] = &(*varToWriteObj);
          SystematicsVarMapDown[key] = &(*varToWriteObj);
        } 
    }
    systematics.initTree(tree, "totEventWeight");
    //event loop
    Long64_t nentries = tree->GetEntriesFast();std::cout<<nentries<<std::endl;int zeros=0;
    for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t nb = tree->GetEntry(jentry);
      if(nb<0) break; // reached the end of the ntuple

      for(uint iATGC = 0; iATGC < SignalParameters.size() && jentry == 0; iATGC++)
      {
        //temporary fix for PDF to get PDF4LHC recommendations
        for (uint iPDF =0; iPDF < PDFWeights -> size() ; iPDF ++ )
        {
           TH1D *tempPositive = new TH1D(("PDFhistPositive" + SignalParameters.at(iATGC) + varToWriteObj->VarName+ std::to_string(iPDF)).c_str(), ("PDFhistPositive"  + SignalParameters.at(iATGC) +  varToWriteObj->VarName + std::to_string(iPDF)).c_str(), Nbins, varToWriteObj->Range.low, varToWriteObj->Range.high);
           tempPositive -> Sumw2();
           histsPDFSignalPerFilePositive[SignalParameters.at(iATGC)].push_back(tempPositive);

           TH1D *tempNegative = new TH1D(("PDFhistNegative" + SignalParameters.at(iATGC) + varToWriteObj->VarName+ std::to_string(iPDF)).c_str(), ("PDFhistNegative"  + SignalParameters.at(iATGC) +  varToWriteObj->VarName + std::to_string(iPDF)).c_str(), Nbins, varToWriteObj->Range.low, varToWriteObj->Range.high);
           tempNegative -> Sumw2();
           histsPDFSignalPerFileNegative[SignalParameters.at(iATGC)].push_back(tempNegative);
        }  
        //temporary fix 
        for (uint iScale =1; iScale < ScaleWeights -> size() ; iScale ++ )
        {
           TH1D *tempPositive = new TH1D(("ScalehistPositive" + SignalParameters.at(iATGC) + varToWriteObj->VarName+ std::to_string(iScale)).c_str(), ("ScalehistPositive"  + SignalParameters.at(iATGC) +  varToWriteObj->VarName + std::to_string(iScale)).c_str(), Nbins, varToWriteObj->Range.low, varToWriteObj->Range.high);
           tempPositive -> Sumw2();
           histsScaleSignalPerFilePositive[SignalParameters.at(iATGC)].push_back(tempPositive);

           TH1D *tempNegative = new TH1D(("ScalehistNegative" + SignalParameters.at(iATGC) + varToWriteObj->VarName+ std::to_string(iScale)).c_str(), ("ScalehistNegative"  + SignalParameters.at(iATGC) +  varToWriteObj->VarName + std::to_string(iScale)).c_str(), Nbins, varToWriteObj->Range.low, varToWriteObj->Range.high);
           tempNegative -> Sumw2();
           histsScaleSignalPerFileNegative[SignalParameters.at(iATGC)].push_back(tempNegative);
        }  
      }
      for (uint iPDF =0; iPDF < PDFWeights -> size() && jentry == 0; iPDF ++ )
      {
          TH1D *temp = new TH1D(("PDFhistSM" + varToWriteObj->VarName+ std::to_string(iPDF)).c_str(), ("PDFhistSM" +  varToWriteObj->VarName + std::to_string(iPDF)).c_str(), Nbins, varToWriteObj->Range.low, varToWriteObj->Range.high);
          temp -> Sumw2();
          SMhistsPDF.push_back(temp);
      }

      for (uint iScale =1; iScale < ScaleWeights -> size() && jentry == 0; iScale ++ )
      {
          TH1D *temp = new TH1D(("ScalehistSM" + varToWriteObj->VarName+ std::to_string(iScale)).c_str(), ("ScalehistSM" +  varToWriteObj->VarName + std::to_string(iScale)).c_str(), Nbins, varToWriteObj->Range.low, varToWriteObj->Range.high);
           temp -> Sumw2();
           SMhistsScale.push_back(temp);
      }
      // fill variables
      // if(aTGCWeights->size()<124){std::cout<<"Entry in tree: "<<jentry<<" Number of faulty entries: "<<zeros++<<" aTGCWeights size: "<<aTGCWeights->size()<<std::endl;for(int i=aTGCWeights->size();i<124;i++)aTGCWeights->push_back(0);}
      for(auto var = variables.begin(); var != variables.end() ; var++)
      {
	       std::string vname = var->VarName;
	       if(signalSelection -> EvalInstance()) signalHist[vname]->Fill(var->value(),SignalSample.weight*totEventWeight*(aTGCWeights->at(123)));//check if the event passeds the selection, and if true fill the histogram
      }
      //start filling up hists for different aTGC points
      for (uint iPar =0; iPar < SignalParameters.size()  ; iPar ++)
      {
        if (SignalParameters.at(iPar) == "cwww"){
            if(signalSelection -> EvalInstance()) signalHistPerParPositive[SignalParameters.at(iPar)]->Fill(varToWriteObj->value(),totEventWeight*(aTGCWeights->at(11)) );
            if(signalSelection -> EvalInstance()) signalHistPerParNegative[SignalParameters.at(iPar)]->Fill(varToWriteObj->value(),totEventWeight*(aTGCWeights->at(111)) );
            systematics.fillHist(varToWriteObj, SystematicsVarMapUp, SystematicsVarMapDown, signalHistPerParPositive_SystUp[SignalParameters.at(iPar)], signalHistPerParPositive_SystDown[SignalParameters.at(iPar)], totEventWeight*(aTGCWeights->at(11)) , (aTGCWeights->at(11)));
            systematics.fillHist(varToWriteObj, SystematicsVarMapUp, SystematicsVarMapDown, signalHistPerParNegative_SystUp[SignalParameters.at(iPar)], signalHistPerParNegative_SystDown[SignalParameters.at(iPar)], totEventWeight*(aTGCWeights->at(111)) , (aTGCWeights->at(111)));
            for (uint iPDF = 0; iPDF < PDFWeights -> size() && signalSelection -> EvalInstance(); iPDF++)
            {
              histsPDFSignalPerFilePositive[SignalParameters.at(iPar)].at(iPDF) -> Fill(varToWriteObj->value(),totEventWeight*PDFWeights->at(iPDF)*(aTGCWeights->at(11)) );
              histsPDFSignalPerFileNegative[SignalParameters.at(iPar)].at(iPDF) -> Fill(varToWriteObj->value(),totEventWeight*PDFWeights->at(iPDF)*(aTGCWeights->at(111)) );
            }
            for (uint iScale = 1; iScale < ScaleWeights -> size() && signalSelection -> EvalInstance(); iScale++)
            {
              histsScaleSignalPerFilePositive[SignalParameters.at(iPar)].at(iScale-1) -> Fill(varToWriteObj->value(),totEventWeight*ScaleWeights->at(iScale)*(aTGCWeights->at(11)) );
              histsScaleSignalPerFileNegative[SignalParameters.at(iPar)].at(iScale-1) -> Fill(varToWriteObj->value(),totEventWeight*ScaleWeights->at(iScale)*(aTGCWeights->at(111)) );
            }
           }
        else if (SignalParameters.at(iPar) == "ccw"){
            if(signalSelection -> EvalInstance()) signalHistPerParPositive[SignalParameters.at(iPar)]->Fill(varToWriteObj->value(),totEventWeight*(aTGCWeights->at(51)) );
            if(signalSelection -> EvalInstance()) signalHistPerParNegative[SignalParameters.at(iPar)]->Fill(varToWriteObj->value(),totEventWeight*(aTGCWeights->at(71)) );
            systematics.fillHist(varToWriteObj, SystematicsVarMapUp, SystematicsVarMapDown, signalHistPerParPositive_SystUp[SignalParameters.at(iPar)], signalHistPerParPositive_SystDown[SignalParameters.at(iPar)], totEventWeight*(aTGCWeights->at(51)) , (aTGCWeights->at(51)) );
            systematics.fillHist(varToWriteObj, SystematicsVarMapUp, SystematicsVarMapDown, signalHistPerParNegative_SystUp[SignalParameters.at(iPar)], signalHistPerParNegative_SystDown[SignalParameters.at(iPar)], totEventWeight*(aTGCWeights->at(71)) , (aTGCWeights->at(71)) );
            for (uint iPDF = 0; iPDF < PDFWeights -> size() && signalSelection -> EvalInstance() ; iPDF++)
            {
              histsPDFSignalPerFilePositive[SignalParameters.at(iPar)].at(iPDF) -> Fill(varToWriteObj->value(),totEventWeight*PDFWeights->at(iPDF)*(aTGCWeights->at(51)) );
              histsPDFSignalPerFileNegative[SignalParameters.at(iPar)].at(iPDF) -> Fill(varToWriteObj->value(),totEventWeight*PDFWeights->at(iPDF)*(aTGCWeights->at(71)) );
            }
            for (uint iScale = 1; iScale < ScaleWeights -> size() && signalSelection -> EvalInstance(); iScale++)
            {
              histsScaleSignalPerFilePositive[SignalParameters.at(iPar)].at(iScale-1) -> Fill(varToWriteObj->value(),totEventWeight*ScaleWeights->at(iScale)*(aTGCWeights->at(51)) );
              histsScaleSignalPerFileNegative[SignalParameters.at(iPar)].at(iScale-1) -> Fill(varToWriteObj->value(),totEventWeight*ScaleWeights->at(iScale)*(aTGCWeights->at(71)) );
            }
           }
        else if (SignalParameters.at(iPar) == "cb"){
            if(signalSelection -> EvalInstance()) signalHistPerParPositive[SignalParameters.at(iPar)]->Fill(varToWriteObj->value(),totEventWeight*(aTGCWeights->at(59)) );
            if(signalSelection -> EvalInstance()) signalHistPerParNegative[SignalParameters.at(iPar)]->Fill(varToWriteObj->value(),totEventWeight*(aTGCWeights->at(63)) );
            systematics.fillHist(varToWriteObj, SystematicsVarMapUp, SystematicsVarMapDown, signalHistPerParPositive_SystUp[SignalParameters.at(iPar)], signalHistPerParPositive_SystDown[SignalParameters.at(iPar)], totEventWeight*(aTGCWeights->at(59)) , (aTGCWeights->at(59)));
            systematics.fillHist(varToWriteObj, SystematicsVarMapUp, SystematicsVarMapDown, signalHistPerParNegative_SystUp[SignalParameters.at(iPar)], signalHistPerParNegative_SystDown[SignalParameters.at(iPar)], totEventWeight*(aTGCWeights->at(63)) , (aTGCWeights->at(63)));
            for (uint iPDF = 0; iPDF < PDFWeights -> size() && signalSelection -> EvalInstance() ; iPDF++)
            {
              histsPDFSignalPerFilePositive[SignalParameters.at(iPar)].at(iPDF) -> Fill(varToWriteObj->value(),totEventWeight*PDFWeights->at(iPDF)*(aTGCWeights->at(59)) );
              histsPDFSignalPerFileNegative[SignalParameters.at(iPar)].at(iPDF) -> Fill(varToWriteObj->value(),totEventWeight*PDFWeights->at(iPDF)*(aTGCWeights->at(63)) );
            }
            for (uint iScale = 1; iScale < ScaleWeights -> size() && signalSelection -> EvalInstance(); iScale++)
            {
              histsScaleSignalPerFilePositive[SignalParameters.at(iPar)].at(iScale-1) -> Fill(varToWriteObj->value(),totEventWeight*ScaleWeights->at(iScale)*(aTGCWeights->at(59)) );
              histsScaleSignalPerFileNegative[SignalParameters.at(iPar)].at(iScale-1) -> Fill(varToWriteObj->value(),totEventWeight*ScaleWeights->at(iScale)*(aTGCWeights->at(63)) );
            }
           }
          else  throw std::runtime_error("parameter is not supported, something is confused");

      }//end of filling different aTGC points 

      if (signalSelection -> EvalInstance()) SMhist->Fill(varToWriteObj->value(),totEventWeight*(aTGCWeights->at(61)) );
      systematics.fillHist(varToWriteObj, SystematicsVarMapUp, SystematicsVarMapDown, SMhist_SystUp, SMhist_SystDown, totEventWeight*(aTGCWeights->at(61)) , (aTGCWeights->at(61)) );
      for (uint iPDF = 0; iPDF < PDFWeights -> size() && signalSelection -> EvalInstance(); iPDF++)
      {
         SMhistsPDF.at(iPDF) -> Fill(varToWriteObj->value(),totEventWeight*PDFWeights->at(iPDF)*(aTGCWeights->at(61)) );
      }

      for (uint iScale = 1; iScale < ScaleWeights -> size() && signalSelection -> EvalInstance(); iScale++)
      {
        SMhistsScale.at(iScale-1) -> Fill(varToWriteObj->value(),totEventWeight*ScaleWeights->at(iScale)*(aTGCWeights->at(61)) );
      }

      PDFWeights -> clear();
      ScaleWeights -> clear(); 
    }//end of event loop

    for (uint iPar=0; iPar < SignalParameters.size(); iPar++)
    {
      //PDF uncertainties
      signalHistPerParPositive_PDFUp[SignalParameters.at(iPar)] -> Add(makePDF4LHC(histsPDFSignalPerFilePositive[SignalParameters.at(iPar)], "up"));
      signalHistPerParPositive_PDFDown[SignalParameters.at(iPar)] -> Add(makePDF4LHC(histsPDFSignalPerFilePositive[SignalParameters.at(iPar)], "down"));

      signalHistPerParNegative_PDFUp[SignalParameters.at(iPar)] -> Add(makePDF4LHC(histsPDFSignalPerFileNegative[SignalParameters.at(iPar)], "up"));
      signalHistPerParNegative_PDFDown[SignalParameters.at(iPar)] -> Add(makePDF4LHC(histsPDFSignalPerFileNegative[SignalParameters.at(iPar)], "down"));


      //Scale uncertainties
      signalHistPerParPositive_ScaleUp[SignalParameters.at(iPar)] -> Add(makeEnvelope(histsScaleSignalPerFilePositive[SignalParameters.at(iPar)], "up"));
      signalHistPerParPositive_ScaleDown[SignalParameters.at(iPar)] -> Add(makeEnvelope(histsScaleSignalPerFilePositive[SignalParameters.at(iPar)], "down"));

      signalHistPerParNegative_ScaleUp[SignalParameters.at(iPar)] -> Add(makeEnvelope(histsScaleSignalPerFileNegative[SignalParameters.at(iPar)], "up"));
      signalHistPerParNegative_ScaleDown[SignalParameters.at(iPar)] -> Add(makeEnvelope(histsScaleSignalPerFileNegative[SignalParameters.at(iPar)], "down"));


    }

    SMhist_PDFUp -> Add(makePDF4LHC(SMhistsPDF, "up"));
    SMhist_PDFDown -> Add(makePDF4LHC(SMhistsPDF, "down"));

    SMhist_ScaleUp -> Add(makeEnvelope(SMhistsScale, "up"));
    SMhist_ScaleDown -> Add(makeEnvelope(SMhistsScale, "down"));

  }// end loop over signal files
  if(withSignal){

    fileToWriteHistsSignal -> cd();
    for (uint iPar = 0; iPar < SignalParameters.size(); iPar++)
    {
      signalHistPerParPositive[SignalParameters[iPar]]->Write();
      signalHistPerParNegative[SignalParameters[iPar]]->Write();

      for (uint iSyst =0; iSyst < systematics.ListOfSystematics.size(); iSyst++)
      {
        std::string theSyst = systematics.ListOfSystematics[iSyst];
        signalHistPerParPositive_SystUp[SignalParameters.at(iPar)][theSyst] -> Write();
        signalHistPerParPositive_SystDown[SignalParameters.at(iPar)][theSyst] -> Write();

        signalHistPerParNegative_SystUp[SignalParameters.at(iPar)][theSyst] -> Write();
        signalHistPerParNegative_SystDown[SignalParameters.at(iPar)][theSyst] -> Write();
      }
      for (uint wSyst =0; wSyst < systematics.WeightNameSystematics.size(); wSyst++)
      {
        std::string theSyst = systematics.WeightNameSystematics[wSyst];
        signalHistPerParPositive_SystUp[SignalParameters.at(iPar)][theSyst] -> Write();
        signalHistPerParPositive_SystDown[SignalParameters.at(iPar)][theSyst] -> Write();

        signalHistPerParNegative_SystUp[SignalParameters.at(iPar)][theSyst] -> Write();
        signalHistPerParNegative_SystDown[SignalParameters.at(iPar)][theSyst] -> Write();
      }
      //PDF uncertainties
      signalHistPerParPositive_PDFUp[SignalParameters.at(iPar)] -> Write();
      signalHistPerParPositive_PDFDown[SignalParameters.at(iPar)] -> Write();

      signalHistPerParNegative_PDFUp[SignalParameters.at(iPar)] -> Write();
      signalHistPerParNegative_PDFDown[SignalParameters.at(iPar)] -> Write();

      //Scale uncertainties
      signalHistPerParPositive_ScaleUp[SignalParameters.at(iPar)] -> Write();
      signalHistPerParPositive_ScaleDown[SignalParameters.at(iPar)] -> Write();

      signalHistPerParNegative_ScaleUp[SignalParameters.at(iPar)] -> Write();
      signalHistPerParNegative_ScaleDown[SignalParameters.at(iPar)] -> Write();
    }

    //SM hists

    SMhist ->Write();

    SMhist_PDFUp -> Write();
    SMhist_PDFDown -> Write();
    
    SMhist_ScaleUp -> Write();
    SMhist_ScaleDown -> Write();

    for (uint iSyst =0; iSyst < systematics.ListOfSystematics.size(); iSyst++)
    {
      std::string theSyst = systematics.ListOfSystematics[iSyst];
      SMhist_SystUp[theSyst] -> Write();
      SMhist_SystDown[theSyst] -> Write();        
    }

    for (uint wSyst =0; wSyst < systematics.WeightNameSystematics.size(); wSyst++)
    {
      std::string theSyst = systematics.WeightNameSystematics[wSyst];
      SMhist_SystUp[theSyst] -> Write();
      SMhist_SystDown[theSyst] -> Write();        
    }
  }
  
  //Monte carlo samples
  //beginning of cycle over processes
  boost::progress_display show_progress(samples.size());
  for (uint process_i = 0; process_i < samples.size() && withMC; process_i++)
  {
    std::string process = samples[process_i].Processname;
    //create a buffer for the process		  	
    for(auto var = variables.begin(); var != variables.end() ; var++)
    {
      std::string vname = var-> VarName;      
      std::pair<std::string,std::string> key(vname,process);
      hist_per_process[key] = new TH1D((process + "_" + vname).c_str(),(process + "_" + vname).c_str(), var->nBins,var->Range.low, var->Range.high);
      hist_per_process[key] -> Sumw2();
    }
    //create histograms for systematics per process
    for (uint iSyst = 0;iSyst < systematics.ListOfSystematics.size() && withSystematics && wantToWriteHists; iSyst ++)
    {
       std::string theSyst = systematics.ListOfSystematics.at(iSyst);
       hist_per_process_SystUp[theSyst] = new TH1D((process + "_" + theSyst + "Up").c_str(),(process + "_" + theSyst + "Up").c_str(), Nbins,varToWriteObj->Range.low, varToWriteObj->Range.high);
       hist_per_process_SystDown[theSyst] = new TH1D((process + "_" + theSyst + "Down").c_str(),(process + "_" + theSyst+ "Down").c_str(), Nbins,varToWriteObj->Range.low, varToWriteObj->Range.high);
       hist_per_process_SystUp[theSyst] -> Sumw2();
       hist_per_process_SystDown[theSyst] -> Sumw2();

    }

    for (uint iSyst = 0;iSyst < systematics.WeightNameSystematics.size() && withSystematics && wantToWriteHists; iSyst ++)
    {
       std::string theSyst = systematics.WeightNameSystematics.at(iSyst);
       hist_per_process_SystUp[theSyst] = new TH1D((process + "_" + theSyst + "Up").c_str(),(process + "_" + theSyst + "Up").c_str(), Nbins,varToWriteObj->Range.low, varToWriteObj->Range.high);
       hist_per_process_SystDown[theSyst] = new TH1D((process + "_" + theSyst + "Down").c_str(),(process + "_" + theSyst+ "Down").c_str(), Nbins,varToWriteObj->Range.low, varToWriteObj->Range.high);
       hist_per_process_SystUp[theSyst] -> Sumw2();
       hist_per_process_SystDown[theSyst] -> Sumw2();

    }
    if (wantToWriteHists){
      //PDF systematics
       hist_per_process_PDFUp = new TH1D((process + "_PDFUp").c_str(),(process + "_PDFUp").c_str(), Nbins,varToWriteObj->Range.low, varToWriteObj->Range.high);
       hist_per_process_PDFDown = new TH1D((process + "_PDFDown").c_str(),(process + "_PDFDown").c_str(), Nbins,varToWriteObj->Range.low, varToWriteObj->Range.high);
       hist_per_process_PDFUp -> Sumw2();
       hist_per_process_PDFDown -> Sumw2();
      //Scale systematics
       hist_per_process_ScaleUp = new TH1D((process + "_ScaleUp").c_str(),(process + "_ScaleUp").c_str(), Nbins,varToWriteObj->Range.low, varToWriteObj->Range.high);
       hist_per_process_ScaleDown = new TH1D((process + "_ScaleDown").c_str(),(process + "_ScaleDown").c_str(), Nbins,varToWriteObj->Range.low, varToWriteObj->Range.high);
       hist_per_process_ScaleUp -> Sumw2();
       hist_per_process_ScaleDown -> Sumw2();
    }
    //loop over files for the given process
    for (uint file_i = 0; file_i < (samples.at(process_i)).filenames.size(); ++file_i)
    {
      TFile file((samples.at(process_i)).filenames.at(file_i).c_str(), "READ");
      TTree * tree = (TTree*)file.Get("BasicTree");
      Double_t totEventWeight;
      double genWeight, lumiWeight,PUWeight;
      std::vector<double> *PDFWeights = 0;
      std::vector<double> *ScaleWeights = 0;
      tree -> SetBranchAddress("totEventWeight", &totEventWeight);
      tree -> SetBranchAddress("PDFWeights", &PDFWeights);
      tree -> SetBranchAddress("ScaleWeights", &ScaleWeights);

      TTreeFormula *MCSampleSelection = new TTreeFormula("MCSampleSelection",samples.at(process_i).selection.c_str(),tree);//that should be without any weights!
      std::map<std::string, vector<TH1D*>> histsPDFPerFile;
      std::map<std::string, vector<TH1D*>> histsScalePerFile;
	  
      //initialize variables.
      for(auto var = variables.begin(); var != variables.end() ; var++)
      {
	       var->Initialize(tree);
         std::pair<std::string,std::string> key(var->VarName,std::string(""));


         if(withSystematics){
          for (uint iSyst = 0;iSyst < systematics.ListOfSystematics.size(); iSyst ++)
           {  
              key.second=systematics.ListOfSystematics.at(iSyst);  
              if (systematics.isAffectedBySystematic(*var, systematics.ListOfSystematics.at(iSyst))){
                Var *varUp = new Var();
                Var *varDown = new Var();
                varUp->VarName = (var -> VarName )+ "_" + systematics.ListOfSystematics.at(iSyst) + "Up";
                varDown->VarName = (var -> VarName )+ "_" + systematics.ListOfSystematics.at(iSyst) + "Down";
                varUp->Initialize(tree);
                varDown->Initialize(tree);
                SystematicsVarMapUp[key] = varUp;
                SystematicsVarMapDown[key] = varDown;
              }
              else {
                SystematicsVarMapUp[key] = &(*var);
                SystematicsVarMapDown[key] = &(*var);
              } 
           }
         }
      }//end of initializing variables

      if(withSystematics)systematics.initTree(tree,"totEventWeight");

      //event loop
      Long64_t nentries = tree->GetEntriesFast();
      for (Long64_t jentry=0; jentry<nentries;jentry++) 
      {
	       Long64_t nb = tree->GetEntry(jentry);
         if(nb<0) break; // reached the end of the ntuple
         if (jentry == 0 && withSystematics){
              for(auto var = variables.begin(); var!= variables.end(); var++)
              {
                for (uint iPDF =0; iPDF < PDFWeights -> size(); iPDF ++ )
                {
                  //this is a hack that should be disregarded, hack is done to keep the number of PDF variation *exactly* 100
                  if(PDFWeights -> size() == 101 && iPDF == 0) continue;
                  if(PDFWeights -> size() == 102 && (iPDF == 100 || iPDF == 101)) continue;
                  TH1D *temp = new TH1D(("PDFhist" + var->VarName+ std::to_string(iPDF)).c_str(), ("PDFhist"+  var->VarName + std::to_string(iPDF)).c_str(), var->nBins, var ->Range.low, var->Range.high);
                  temp -> Sumw2();
                  histsPDFPerFile[var->VarName].push_back(temp);
                }  
                for (uint iScale =1; iScale < ScaleWeights -> size(); iScale ++ )
                {
                  if(iScale!=5 && iScale!=7)
                  {
                    TH1D *temp = new TH1D(("Scalehist" + var->VarName+ std::to_string(iScale)).c_str(), ("Scalehist"+  var->VarName + std::to_string(iScale)).c_str(), var->nBins, var ->Range.low, var->Range.high);
                    temp -> Sumw2();
                    histsScalePerFile[var->VarName].push_back(temp);
                  }
                } 
              }
          }
	
	       // fill variables
          std::string process = samples[process_i].Processname;
          std::pair<std::string,std::string> key("",process);
         if(MCSampleSelection -> EvalInstance()) {
          for(auto var = variables.begin(); var != variables.end() ; var++)
          {
      	     key.first = var -> VarName;
      	     hist_per_process[key]->Fill(var->value(), totEventWeight*(samples.at(process_i).weight));//check if the event passeds the selection, and if true fill the histogram
              for (uint iPDF =0; iPDF < PDFWeights -> size() && withSystematics; iPDF ++ )
              {
                //this is a hack that should be disregarded, hack is done to keep the number of PDF variation *exactly* 100
                 if(PDFWeights -> size() == 101 ){
                  if(iPDF == 0) continue;
                  else histsPDFPerFile[var->VarName].at(iPDF-1) -> Fill(var->value(), (samples.at(process_i).weight)*totEventWeight*PDFWeights->at(iPDF));
                }
                else if(PDFWeights -> size() == 102 ){
                  if(iPDF == 100 || iPDF == 101) continue;
                  else histsPDFPerFile[var->VarName].at(iPDF) -> Fill(var->value(), (samples.at(process_i).weight)*totEventWeight*PDFWeights->at(iPDF));
                }
                else histsPDFPerFile[var->VarName].at(iPDF) -> Fill(var->value(), (samples.at(process_i).weight)*totEventWeight*PDFWeights->at(iPDF));
              }
              int histsScalePerFileCounter=0;  
              for (uint iScale =1; iScale < ScaleWeights -> size() && withSystematics; iScale ++ )
              {
                if(iScale!=5 && iScale!=7)
                {
                  //this is a temporary fix, should be disregarded               
                 if( boost::algorithm::contains((samples.at(process_i)).filenames.at(file_i),"tW" ) ) histsScalePerFile[var->VarName].at(histsScalePerFileCounter) -> Fill(var->value(), (samples.at(process_i).weight)*totEventWeight);
                 else histsScalePerFile[var->VarName].at(histsScalePerFileCounter) -> Fill(var->value(), (samples.at(process_i).weight)*totEventWeight*ScaleWeights->at(iScale));
                 histsScalePerFileCounter++;
                }
              }
	  }
	}
       if(withSystematics)systematics.fill(&variables, SystematicsVarMapUp, SystematicsVarMapDown,(samples.at(process_i).weight)*totEventWeight, (samples.at(process_i).weight));
       if(withSystematics && wantToWriteHists)systematics.fillHist(varToWriteObj, SystematicsVarMapUp, SystematicsVarMapDown, hist_per_process_SystUp, hist_per_process_SystDown, (samples.at(process_i).weight)*totEventWeight, (samples.at(process_i).weight));
       PDFWeights->clear();
       ScaleWeights->clear();
      }//end of event loop
      //create envelopes for PDF variation
      for(auto var = variables.begin(); var != variables.end() && withSystematics ; var++)
      {
        TH1D *histPDFPDF4LHCUp = makePDF4LHC(histsPDFPerFile[var ->VarName], "up");
        TH1D *histPDFPDF4LHCDown = makePDF4LHC(histsPDFPerFile[var ->VarName], "down");

        TH1D *histScaleEnvelopeUp = makeEnvelope(histsScalePerFile[var ->VarName], "up");
        TH1D *histScaleEnvelopeDown = makeEnvelope(histsScalePerFile[var ->VarName], "down");
        
        hist_PDFUp[var->VarName] -> Add(histPDFPDF4LHCUp);
        hist_PDFDown[var->VarName] -> Add(histPDFPDF4LHCDown);

        hist_ScaleUp[var->VarName] -> Add(histScaleEnvelopeUp);
        hist_ScaleDown[var->VarName] -> Add(histScaleEnvelopeDown);
        
        if(var->VarName == varToWrite && wantToWriteHists)hist_per_process_PDFUp -> Add(histPDFPDF4LHCUp);
        if(var->VarName == varToWrite && wantToWriteHists)hist_per_process_PDFDown -> Add(histPDFPDF4LHCDown);

        if(var->VarName == varToWrite && wantToWriteHists)hist_per_process_ScaleUp -> Add(histScaleEnvelopeUp);
        if(var->VarName == varToWrite && wantToWriteHists)hist_per_process_ScaleDown -> Add(histScaleEnvelopeDown);
      }//end of creating envelopes
    }// end of the loop for the given process      
    ++show_progress; 

    if(wantToWriteHists){
      std::pair<std::string,std::string> keyToWrite(varToWrite,process);
      fileToWriteHists->cd();
      hist_per_process[keyToWrite] -> Write();
      for (uint iSyst = 0;iSyst < systematics.ListOfSystematics.size() && withSystematics; iSyst ++)
      {
       std::string theSyst = systematics.ListOfSystematics.at(iSyst);
       hist_per_process_SystUp[theSyst] -> Write();
       hist_per_process_SystDown[theSyst] -> Write();
      }
      for (uint iSyst = 0;iSyst < systematics.WeightNameSystematics.size() && withSystematics; iSyst ++)
      {
       std::string theSyst = systematics.WeightNameSystematics.at(iSyst);
       hist_per_process_SystUp[theSyst] -> Write();
       hist_per_process_SystDown[theSyst] -> Write();
      }
      hist_per_process_PDFUp -> Write();
      hist_per_process_PDFDown -> Write();

      hist_per_process_ScaleUp -> Write();
      hist_per_process_ScaleDown -> Write();
    }
  }//end of cycle over processes
  
  
  //condition histograms
  for(auto var = variables.begin(); var != variables.end() ; var++){
    std::string vname = var -> VarName;

    if(withData){
      leg[vname]->AddEntry(data[vname], "Data","PE");
      data[vname]->SetFillColor(78);
      if (var -> logscale) data[vname]-> GetYaxis() -> SetRangeUser(0.1, (data[vname] -> GetMaximum())*7.);
      else  data[vname]-> GetYaxis() -> SetRangeUser(0., (data[vname] -> GetMaximum())*1.5);
      char strBuffer[50];
      sprintf(strBuffer,"Events / %d GeV",(int)round((var->Range.high-var->Range.low)/var->nBins));
      //sprintf(strBuffer,"Events / %.2f GeV",(var->Range.high-var->Range.low)/var->nBins);
      data[vname]->GetYaxis()->SetTitle(strBuffer);
      data[vname]->GetYaxis()->SetTitleSize(0.075);
      data[vname]->GetYaxis()->SetTitleOffset(0.75);
      data[vname]->SetMarkerColor(DataSample.color);
      data[vname]->SetMarkerStyle(20);
      data[vname]->GetXaxis() -> SetLabelSize(0.);
      data[vname]->GetXaxis() -> SetLabelOffset(100000.);
    }
    
    if(withSignal){
      signalHist[vname] -> SetLineColor(SignalSample.color);
      signalHist[vname] -> SetLineWidth(2.);
    }

    // Stack needs to be filled in the opposite order to draw first on top and so on
    for (int process_i = samples.size()-1; process_i >=0 && withMC; process_i--){
      std::string process = samples[process_i].Processname;
      std::pair<std::string,std::string> key(vname,process);

      hs[vname] -> Add(hist_per_process[key]);
      hist_summed[vname] -> Add(hist_per_process[key]);
      hist_per_process[key] -> SetFillColor(samples.at(process_i).color);
      hist_per_process[key] -> SetLineColor(samples.at(process_i).color);
      //hist_per_process[key] -> GetYaxis() -> SetRangeUser(0.1, (hist_per_process[key] -> GetMaximum())*1.5);
      hist_per_process[key] -> SetLineColor(kBlack);
      hist_per_process[key] -> SetLineWidth(1.);
    }
    // Legend in correct order gives first on top and so on
    for (uint process_i = 0; process_i < samples.size() && withMC; process_i++){
      std::string process = samples[process_i].Processname;
      std::pair<std::string,std::string> key(vname,process);
      leg[vname]->AddEntry(hist_per_process[key], (samples.at(process_i).Processname).c_str(),"f");
    }
  }
          
  //cosmetics
  for(auto var = variables.begin(); var != variables.end() ; var++)
  {
    std::string vname = var -> VarName;
    if(withSystematics)systematics.eval(&(*var), hist_summed[vname]);
    c1=  new TCanvas("c1","canvas",960,800);
    pad1 = new TPad("pad1","This is pad1",0.0,0.25,0.995,1.0);
    pad2 = new TPad("pad2","This is pad2",0.0,0.02,0.995,0.25);
    
    c1 -> cd();
    if(withSignal)leg[vname] -> AddEntry(signalHist[vname], SignalSample.Processname.c_str()); 	  	  	
    if(var->logscale) pad1 -> SetLogy();
    pad1->Draw();
    if(withMC)pad2->Draw();
    pad1 -> cd();
    
    if(withData)
    {
       if(vname=="MWW_SD"){
          data[vname] -> Sumw2(kFALSE);	
          data[vname] -> SetBinErrorOption(TH1::kPoisson);
       }
       data[vname] -> Draw("E1X0");
       hs[vname]->Draw("hist SAME s(0,0)");
       hist_summed[vname] -> SetFillColor(kBlack);
       hist_summed[vname] -> SetFillStyle(3018);
       hist_summed[vname] -> Draw("E2 SAME");
       if(withSignal)signalHist[vname] -> Draw("hist SAME");
       data[vname] -> Draw("E1X0 SAME");
       data[vname] -> GetXaxis() -> Draw("SAME");
       if(!withSystematics && withMC)hist_summed[vname] -> GetXaxis() -> SetLabelSize(0.2);
       if(!withSystematics && withMC)hist_summed[vname] -> GetXaxis() -> Draw("SAME");
    } 
    else { 
      if(withMC)hist_summed[vname]->Draw("hist");
      if(withMC)hist_summed[vname] -> GetXaxis() -> SetLabelSize(0.0);
      if(withMC)hist_summed[vname]-> GetYaxis() -> SetRangeUser(0., (hist_summed[vname] -> GetMaximum())*1.5); 
      if(withMC)hs[vname]->Draw("histSAME");
      if(withMC)hist_summed[vname] -> SetFillColor(kBlack);
      if(withMC)hist_summed[vname] -> SetFillStyle(3018);
      if(withMC)hist_summed[vname] -> Draw("E2 SAME");
      
      if(withSignal)signalHist[vname] -> Draw("HISTSAME");

    }
    c1 -> cd();

    // Channel and CR name
    TPaveText *pt = new TPaveText(0.1875,0.82,0.625,0.90, "blNDC");
    pt -> SetFillStyle(0);
    pt -> SetBorderSize(0);
    pt -> SetTextAlign(12);
    pt -> SetTextSize(0.040);
    if (channel == ELECTRON) pt -> AddText("Electron channel");
    else if (channel == MUON) pt -> AddText("Muon channel");
    else std::cerr << "no channel set..." << std::endl;
    if (contReg == WJETS) pt -> AddText("W+jets control region");
    else if (contReg == TTBAR) pt -> AddText("t#bar{t} control region");
    else pt -> AddText("Signal region");
    pt -> Draw("SAME");

    // Chi2 Test
    /*if(withMC && withData){

      TH1D* dataForChi2 = (TH1D*) data[vname]->Clone();
      TH1D* MCForChi2 = (TH1D*) hist_summed[vname]->Clone();

      for(int i=1; i<=dataForChi2->GetNbinsX(); i++)
        if(MCForChi2->GetBinContent(i)<0.1){
          MCForChi2->SetBinContent(i,0);
          dataForChi2->SetBinContent(i,0);
        }

      double chi2 = dataForChi2->Chi2Test(MCForChi2,"UW P CHI2/NDF");
      
      TPaveText *pt2 = new TPaveText(0.52,0.82,0.72,0.90, "blNDC");
      pt2 -> SetFillStyle(0);
      pt2 -> SetBorderSize(0);
      pt2 -> SetTextAlign(32);
      pt2 -> SetTextSize(0.060);
      pt2 -> AddText(Form("#chi^{2}_{#nu}=%4.2f",chi2));
      pt2 -> Draw("SAME");
    }*/
    
    pad1 -> SetTopMargin(0.07);
    pad1 -> SetBottomMargin(0.03);
    pad1 -> SetRightMargin(0.05);
    if(withMC){
      pad2 -> SetRightMargin(0.05);
      pad2 -> SetTopMargin(0);
      pad2 -> SetBottomMargin(0.5);
      pad2 -> cd();
    }

    
    //TH1D *data_dif = new TH1D((vname + "_dif").c_str(),( vname + "_dif").c_str(), var->nBins,var->Range.low, var->Range.high);
    //data_dif -> Sumw2();
    TH1D *data_dif_MCerr = new TH1D((vname + "_dif_MCerror").c_str(),( vname + "_dif_MCerror").c_str(), var->nBins,var->Range.low, var->Range.high);
    data_dif_MCerr -> Sumw2();
    data_dif_MCerr -> SetFillColor(kGray);
    
    /*if(withMC && withData){
      for (int iBin = 1; iBin <= hist_summed[vname] -> GetNbinsX(); ++iBin)
      {
          if (hist_summed[vname] -> GetBinContent(iBin) == 0. || data[vname] -> GetBinContent(iBin) == 0) data_dif -> SetBinContent(iBin,10000000.);
          else {
              data_dif -> SetBinContent(iBin, ((data[vname] -> GetBinContent(iBin)) - (hist_summed[vname] -> GetBinContent(iBin)))/(hist_summed[vname] -> GetBinContent(iBin)));
              data_dif -> SetBinError(iBin, (data[vname]-> GetBinError(iBin))/(hist_summed[vname] -> GetBinContent(iBin)));
          }
     }
     //std::cout<<vname<<std::endl;
     //std::cout<<"Data Integral: "<<data[vname]->Integral()<<std::endl;
     //std::cout<<"MC Integral: "<<hist_summed[vname]->Integral()<<std::endl;
    }*/
    
    
    for (int iBin = 1; iBin <= var->nBins && withMC ; ++iBin)
    {
        if (hist_summed[vname] -> GetBinContent(iBin) == 0.) {
	           data_dif_MCerr -> SetBinContent(iBin, 0.);
	           data_dif_MCerr -> SetBinError(iBin, 0.);
        }
        else {
        	  data_dif_MCerr -> SetBinContent(iBin, 0.);
        	  data_dif_MCerr -> SetBinError(iBin, (hist_summed[vname] -> GetBinError(iBin))/hist_summed[vname] -> GetBinContent(iBin) );
        }
    }

    if(withMC){
      data_dif_MCerr -> SetMaximum(1.);
      data_dif_MCerr ->  SetMinimum(-1.);
      data_dif_MCerr -> GetYaxis() -> SetNdivisions(5);
      data_dif_MCerr -> GetYaxis() -> SetLabelSize(0.15);
      data_dif_MCerr -> GetXaxis() -> SetLabelSize(0.2);
      data_dif_MCerr -> GetYaxis()->SetTitle("#frac{Data - MC}{MC}");
      data_dif_MCerr -> GetXaxis()->SetTitle((var->Title).c_str());
      data_dif_MCerr -> GetXaxis()->SetTitleOffset(0.7);
      data_dif_MCerr -> GetXaxis()->SetTitleSize(0.3);
      data_dif_MCerr -> GetYaxis()->SetTitleOffset(0.3);
      data_dif_MCerr -> GetYaxis()->SetTitleSize(0.2);
      data_dif_MCerr -> Draw("E2");
    }
  
    if(withData){
      //data_dif -> Sumw2(kFALSE);
      //data_dif -> SetBinErrorOption(TH1::kPoisson);
      //data_dif -> SetMarkerStyle(20);
      //data_dif -> Draw("E1X0 SAME");
      const double alpha = 1 - 0.6827;
      TGraphAsymmErrors * data_dif_graph = new TGraphAsymmErrors(data[vname]);
      data_dif_graph -> SetMarkerStyle(20);
      for (int i = 0; i < data_dif_graph->GetN(); ++i) {
        int N = data_dif_graph->GetY()[i];
        double MC = hist_summed[vname] -> GetBinContent(i+1);
        double L =  (N==0) ? 0  : (ROOT::Math::gamma_quantile(alpha/2,N,1.));
        double U =  ROOT::Math::gamma_quantile_c(alpha/2,N+1,1);
        if (N==0 || MC<0.01) data_dif_graph->SetPoint(i, data_dif_graph->GetX()[i], 100000.);
        else data_dif_graph->SetPoint(i, data_dif_graph->GetX()[i], (N-MC)/MC);
        data_dif_graph->SetPointEXlow(i, 0);
        data_dif_graph->SetPointEXhigh(i, 0);
        data_dif_graph->SetPointEYlow(i, (N-L)/MC);
        data_dif_graph->SetPointEYhigh(i, (U-N)/MC);
      }
      data_dif_graph->Draw("0P SAME");
    }
    TLine *line = new TLine(var->Range.low,0.,var->Range.high,0.);
    line -> Draw("SAME");

    
    //if(withSystematics)leg[vname] -> AddEntry(data_dif_MCerr, "Syst. unc.", "f");
    if(withSystematics)leg[vname] -> AddEntry(hist_summed[vname], "Stat.#oplussyst.", "f");
    c1 -> cd();
    leg[vname]->Draw("SAME");
    
    
    CMS_lumi( c1, 4, 0 );
    c1 -> SaveAs((OutPrefix_  + "png/"+ var->VarName + ".png").c_str());
    c1 -> Update();
    c1 -> Print((OutPrefix_  + "pdf/"+ var->VarName + ".pdf").c_str());
    c1 -> Clear();
    delete c1;
  }///end of cosmetics 
}
