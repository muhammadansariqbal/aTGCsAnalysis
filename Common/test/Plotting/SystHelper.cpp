#include "SystHelper.hpp"

#include <string>
#include <map>
#include <algorithm>
#include <vector>
#include <boost/algorithm/string/replace.hpp>
#include "boost/algorithm/string.hpp"
#include <TTreeFormula.h>

//#include <pair>

SystHelper::SystHelper(){}
SystHelper::SystHelper(std::string selection){
  NominalSelection = selection;

  //list of supported systematics
  ListOfSystematics.push_back("JEC");
  ListOfSystematics.push_back("LeptonEn");
  ListOfSystematics.push_back("LeptonRes");
  ListOfSystematics.push_back("UnclEn");
  ListOfSystematics.push_back("JER");

  //JEC
  VarsJEC.push_back("W_pt");
  VarsJEC.push_back("W_mass");
  VarsJEC.push_back("W_mt");
  VarsJEC.push_back("pfMETPhi");
  VarsJEC.push_back("pfMET");
  VarsJEC.push_back("jet_pt");
  VarsJEC.push_back("Mjpruned");
  VarsJEC.push_back("jet_mass_softdrop");
  VarsJEC.push_back("jet_mass");
  VarsJEC.push_back("MWW_SD");
  VarsJEC.push_back("deltaPhi_LeptonMet");
  VarsJEC.push_back("deltaPhi_WJetMet");
  VarsJEC.push_back("deltaPhi_WJetWlep");
  VarsJEC.push_back("jet_mass_softdrop_PUPPI");

  //En (lepton energy scale)
  VarsLeptonEn.push_back("l_pt");
  VarsLeptonEn.push_back("W_pt");
  VarsLeptonEn.push_back("W_mass");
  VarsLeptonEn.push_back("W_mt");
  VarsLeptonEn.push_back("pfMET");
  VarsLeptonEn.push_back("pfMETPhi");
  VarsLeptonEn.push_back("deltaPhi_LeptonMet");
  VarsLeptonEn.push_back("deltaPhi_WJetMet");
  VarsLeptonEn.push_back("deltaPhi_WJetWlep");
  VarsLeptonEn.push_back("MWW_SD");

  //lepton resolution
  VarsLeptonRes.push_back("l_pt");
  VarsLeptonRes.push_back("W_pt");
  VarsLeptonRes.push_back("W_mass");
  VarsLeptonRes.push_back("W_mt");
  VarsLeptonRes.push_back("MWW_SD");

  //MET
  VarsUnclEn.push_back("W_pt");
  VarsUnclEn.push_back("W_mass");
  VarsUnclEn.push_back("W_mt");
  VarsUnclEn.push_back("pfMET");
  VarsUnclEn.push_back("pfMETPhi");
  VarsUnclEn.push_back("deltaPhi_LeptonMet");
  VarsUnclEn.push_back("deltaPhi_WJetMet");
  VarsUnclEn.push_back("deltaPhi_WJetWlep");
  VarsUnclEn.push_back("MWW_SD");

  //JER
  VarsJER.push_back("pfMET");
  VarsJER.push_back("pfMETPhi");
  VarsJER.push_back("deltaPhi_LeptonMet");
  VarsJER.push_back("deltaPhi_WJetMet");
  VarsJER.push_back("jet_pt");
  VarsJER.push_back("jet_mass");
  VarsJER.push_back("Mjpruned");
  VarsJER.push_back("jet_mass_softdrop");
  VarsJER.push_back("MWW_SD");
  VarsJER.push_back("jet_mass_softdrop_PUPPI");

  //JetRes, please merge JER and JetRes!
  //VarsJetRes.push_back("MWW_SD");

  VariablesAffected.insert(std::pair<std::string, std::vector<std::string>>("JEC", VarsJEC));
  VariablesAffected.insert(std::pair<std::string, std::vector<std::string>>("LeptonEn", VarsLeptonEn));
  VariablesAffected.insert(std::pair<std::string, std::vector<std::string>>("LeptonRes", VarsLeptonRes));
  VariablesAffected.insert(std::pair<std::string, std::vector<std::string>>("UnclEn", VarsUnclEn));
  VariablesAffected.insert(std::pair<std::string, std::vector<std::string>>("JER", VarsJER));
  //VariablesAffected.insert(std::pair<std::string, std::vector<std::string>>("JetRes", VarsJetRes));

  //weighted systematics
  WeightNameSystematics.push_back("Mistag");
  WeightNameSystematics.push_back("BTag");
  WeightNameSystematics.push_back("LeptonID");
  //WeightNameSystematics.push_back("PU"); 

  //set up systematically varied selections
  selstring=selection;
  for (unsigned int iSyst = 0; iSyst < ListOfSystematics.size(); iSyst ++){
    selection_Up[ListOfSystematics.at(iSyst)] = selection;
    selection_Down[ListOfSystematics.at(iSyst)] = selection;
    //split the string of selection, this is needed because some of variables start with the same characters, thus we split the string to be sure we get correct variable
    std::vector<std::string> splittedSelectionUp;	
    boost::split(splittedSelectionUp,selection_Up[ListOfSystematics.at(iSyst)],boost::is_any_of(" *+=-:/)(|&<>0123456789."));
    //erase duplicates from the string
    std::sort(splittedSelectionUp.begin(), splittedSelectionUp.end());
    splittedSelectionUp.erase(std::unique(splittedSelectionUp.begin(), splittedSelectionUp.end()), splittedSelectionUp.end());
    for (unsigned int iS = 0; iS < splittedSelectionUp.size(); iS ++){
      if (splittedSelectionUp[iS].empty()) continue;
      bool found = false;
      for (unsigned int iVarSyst = 0 ; iVarSyst < VariablesAffected[ListOfSystematics.at(iSyst)].size(); iVarSyst ++){
	found = ( VariablesAffected[ListOfSystematics.at(iSyst)].at(iVarSyst) == splittedSelectionUp[iS] );
	if (found) {
	  boost::replace_all(selection_Up[ListOfSystematics.at(iSyst)],    VariablesAffected.at(ListOfSystematics.at(iSyst)).at(iVarSyst),  VariablesAffected.at(ListOfSystematics.at(iSyst)).at(iVarSyst) + "_" + ListOfSystematics.at(iSyst) + "Up");
	  boost::replace_all(selection_Down[ListOfSystematics.at(iSyst)],  VariablesAffected.at(ListOfSystematics.at(iSyst)).at(iVarSyst),  VariablesAffected.at(ListOfSystematics.at(iSyst)).at(iVarSyst) + "_"+ ListOfSystematics.at(iSyst) + "Down");
	  continue;
	}
      }
    }
  }    
}

void SystHelper::initTree(TTree* tree, std::string prefixEventWeight="totEventWeight"){
  nominalSelection = new TTreeFormula("nominalSelection", NominalSelection.c_str(), tree);
  for (uint iSyst =0; iSyst < ListOfSystematics.size(); iSyst++)
    {
      if (selectionUpInFormula[ListOfSystematics[iSyst]]!=0) delete selectionUpInFormula[ListOfSystematics[iSyst]];
      selectionUpInFormula[ListOfSystematics[iSyst]] = new TTreeFormula(("selection_Up" + ListOfSystematics[iSyst]).c_str(), selection_Up[ListOfSystematics[iSyst]].c_str(), tree);
      if (selectionDownInFormula[ListOfSystematics[iSyst]]!=0) delete selectionDownInFormula[ListOfSystematics[iSyst]];
      selectionDownInFormula[ListOfSystematics[iSyst]] = new TTreeFormula(("selection_Down" + ListOfSystematics[iSyst]).c_str(), selection_Down[ListOfSystematics[iSyst]].c_str(), tree);
     
    }

    //initialize weights for systematics
    for (uint wSyst = 0; wSyst < WeightNameSystematics.size(); wSyst ++)
    {
      weightsUp[WeightNameSystematics.at(wSyst)] = new Var();
      weightsDown[WeightNameSystematics.at(wSyst)] = new Var();

      weightsUp[WeightNameSystematics.at(wSyst)] -> VarName = prefixEventWeight + "_" + WeightNameSystematics.at(wSyst) + "Up";
      weightsDown[WeightNameSystematics.at(wSyst)]  -> VarName = prefixEventWeight + "_" + WeightNameSystematics.at(wSyst) + "Down";

      weightsUp[WeightNameSystematics.at(wSyst)] -> Initialize(tree);
      weightsDown[WeightNameSystematics.at(wSyst)]  -> Initialize(tree);

      
    }
}

void SystHelper::AddVar(Var* var, TH1D* refhist){
  std::string VarName = var->VarName;
  //create histograms for each of systematics and defined if the variable should be varied (is affected)
  // and if the histogram doesn't exist already
  int Nbins = refhist->GetNbinsX();
  for (unsigned int iSyst = 0; iSyst < ListOfSystematics.size(); iSyst ++){      
    std::pair<std::string,std::string> key(VarName,ListOfSystematics.at(iSyst));
    hist_SystUp[key] = new TH1D((VarName + "_" + ListOfSystematics.at(iSyst) + "Up").c_str(),(VarName + "_" +  ListOfSystematics.at(iSyst) + "Up" ).c_str(), Nbins, var->Range.low, var->Range.high);
    hist_SystDown[key] = new TH1D((VarName + "_" + ListOfSystematics.at(iSyst) + "Down").c_str(),(VarName + "_"+ ListOfSystematics.at(iSyst) + "Down" ).c_str(), Nbins, var->Range.low, var->Range.high);
    hist_SystUp[key] -> Sumw2();
    hist_SystDown[key] -> Sumw2();
  }
  //weighted systematics
  for (unsigned int iSyst = 0; iSyst < WeightNameSystematics.size(); iSyst ++){      
    std::pair<std::string,std::string> key(VarName,WeightNameSystematics.at(iSyst));
    hist_SystUp[key] = new TH1D((VarName + "_" + WeightNameSystematics.at(iSyst) + "Up").c_str(),(VarName + "_" +  WeightNameSystematics.at(iSyst) + "Up" ).c_str(), Nbins, var->Range.low, var->Range.high);
    hist_SystDown[key] = new TH1D((VarName + "_" + WeightNameSystematics.at(iSyst) + "Down").c_str(),(VarName + "_"+ WeightNameSystematics.at(iSyst) + "Down" ).c_str(), Nbins, var->Range.low, var->Range.high);
    hist_SystUp[key] -> Sumw2();
    hist_SystDown[key] -> Sumw2();
  }
}
void SystHelper::AddSyst(std::map<std::string, TH1D * > & Up, std::map<std::string, TH1D * > & Down){
  histsAdd_Up.push_back(Up);
  histsAdd_Down.push_back(Down);
}

void SystHelper::eval(Var* var, TH1D * hist_nominal){
  int Nbins = hist_nominal->GetNbinsX();

  
  std::vector<double> totalErrorQuadraticErrors(Nbins, 0.);
  //add statistical uncertainty from MC first
  for (unsigned int iBin = 0; iBin < Nbins; iBin++){
    totalErrorQuadraticErrors[iBin] = std::pow(hist_nominal -> GetBinError(iBin), 2);
  }
  std::string VarName = var->VarName;

  //now sum up the differences quadratically
  for (unsigned int iBin = 1; iBin <= hist_nominal -> GetNbinsX(); iBin ++){
    for (uint iSyst = 0; iSyst < ListOfSystematics.size(); iSyst++){
      std::pair<std::string,std::string> key(VarName,ListOfSystematics.at(iSyst));
      double errorUpQuadratic = pow(std::abs((hist_SystUp[key] -> GetBinContent(iBin)) - (hist_nominal -> GetBinContent(iBin))), 2);
      double errorDownQuadratic = pow(std::abs((hist_SystDown[key] -> GetBinContent(iBin)) - (hist_nominal -> GetBinContent(iBin))), 2);
      double errorQuadratic = std::max(errorUpQuadratic, errorDownQuadratic);
      totalErrorQuadraticErrors.at(iBin-1) += errorQuadratic;
      
    }

    for (uint wSyst = 0; wSyst < WeightNameSystematics.size(); wSyst++){
      std::pair<std::string,std::string> key(VarName,WeightNameSystematics.at(wSyst));
      double errorUpQuadratic = pow(std::abs((hist_SystUp[key] -> GetBinContent(iBin)) - (hist_nominal -> GetBinContent(iBin))), 2);
      double errorDownQuadratic = pow(std::abs((hist_SystDown[key] -> GetBinContent(iBin)) - (hist_nominal -> GetBinContent(iBin))), 2);
      double errorQuadratic = std::max(errorUpQuadratic, errorDownQuadratic);
      totalErrorQuadraticErrors.at(iBin-1) += errorQuadratic;      
    }
    //additional systematics 
    if (histsAdd_Up.size() != histsAdd_Down.size()) {
      std::cerr << "Are you kidding ? the size of up and down vectors isn't the same ..." << std::endl;
      exit(0);
    }
    for (uint iAdd = 0; iAdd < histsAdd_Up.size(); iAdd ++)
    {
      double errorUpQuadratic = pow(std::abs((histsAdd_Up.at(iAdd)[VarName] -> GetBinContent(iBin)) - (hist_nominal -> GetBinContent(iBin))), 2);
      double errorDownQuadratic = pow(std::abs((histsAdd_Down.at(iAdd)[VarName] -> GetBinContent(iBin))  - (hist_nominal -> GetBinContent(iBin))), 2);
      double errorQuadratic = std::max(errorUpQuadratic, errorDownQuadratic);
      totalErrorQuadraticErrors.at(iBin-1) += errorQuadratic;
      
    }
  }
  
  int iBin = 1; 
  //now set the uncertainty  	
  for (double & error : totalErrorQuadraticErrors) {
    error = std::sqrt(error);
    hist_nominal -> SetBinError(iBin, error);
    iBin++;
  }
}

void SystHelper::fill(std::vector<Var>* variables,std::map<std::pair<std::string, std::string>, Var*> & SystematicsVarMapUp_, std::map<std::pair<std::string, std::string>, Var*> & SystematicsVarMapDown_, double weight, double addWeightForWeightedSyst){
  
  for (uint iSyst =0; iSyst < ListOfSystematics.size(); iSyst++)
  {
    std::pair<std::string,std::string> key("",ListOfSystematics.at(iSyst));
    if(selectionUpInFormula[ListOfSystematics.at(iSyst)] -> EvalInstance()){
      for (auto var = variables->begin();var != variables->end(); var++){
        key.first= var -> VarName;
        hist_SystUp[key] -> Fill(SystematicsVarMapUp_[key]->value(), weight);
      }
    }

    if(selectionDownInFormula[ListOfSystematics.at(iSyst)] -> EvalInstance()){
      for (auto var = variables->begin();var != variables->end(); var++){
        key.first= var -> VarName;
        hist_SystDown[key] -> Fill(SystematicsVarMapDown_[key]->value(), weight);
      }
    } 
  }

  for (unsigned int wSyst = 0; wSyst < WeightNameSystematics.size(); wSyst ++)
  {  
    std::pair<std::string,std::string> key("",WeightNameSystematics.at(wSyst));     
    if (nominalSelection -> EvalInstance()){
      for (auto var = variables->begin();var != variables->end(); var++){
        key.first = var -> VarName;
        hist_SystUp[key] -> Fill(var -> value(), addWeightForWeightedSyst*(weightsUp[WeightNameSystematics.at(wSyst)] -> value()));
        hist_SystDown[key] -> Fill(var -> value(), addWeightForWeightedSyst*(weightsDown[WeightNameSystematics.at(wSyst)] -> value()));
      }
    }
  }
}

void SystHelper::fillHist(Var * var,std::map<std::pair<std::string, std::string>, Var*> & SystematicsVarMapUp_, std::map<std::pair<std::string, std::string>, Var*> & SystematicsVarMapDown_,  std::map<std::string, TH1D*> & histSystUp_,  std::map<std::string, TH1D*> & histSystDown_, double weight, double addWeightForWeightedSyst)
{
  
  for (uint iSyst =0; iSyst < ListOfSystematics.size(); iSyst++)
  {
    std::pair<std::string,std::string> key(var -> VarName,ListOfSystematics.at(iSyst));
    if(selectionUpInFormula[ListOfSystematics.at(iSyst)] -> EvalInstance()) histSystUp_[ListOfSystematics.at(iSyst)] -> Fill(SystematicsVarMapUp_[key]->value(), weight);
    if(selectionDownInFormula[ListOfSystematics.at(iSyst)] -> EvalInstance()) histSystDown_[ListOfSystematics.at(iSyst)] -> Fill(SystematicsVarMapDown_[key]->value(), weight);     
  }

  for (uint wSyst =0; wSyst < WeightNameSystematics.size(); wSyst++)
  {
    std::pair<std::string,std::string> key(var -> VarName,WeightNameSystematics.at(wSyst));
    if(nominalSelection -> EvalInstance()) histSystUp_[WeightNameSystematics.at(wSyst)] -> Fill(var->value(), addWeightForWeightedSyst*(weightsUp[WeightNameSystematics.at(wSyst)] -> value()));
    if(nominalSelection -> EvalInstance()) histSystDown_[WeightNameSystematics.at(wSyst)] -> Fill(var->value(), addWeightForWeightedSyst*(weightsDown[WeightNameSystematics.at(wSyst)] -> value()));     
  }
}

bool SystHelper::isAffectedBySystematic(Var var, std::string systematic) {
  bool isAffected;
  if (std::find(VariablesAffected[systematic].begin(), VariablesAffected[systematic].end(), var.VarName) != VariablesAffected[systematic].end()) isAffected = true;
  else isAffected = false;
  return isAffected;

}
