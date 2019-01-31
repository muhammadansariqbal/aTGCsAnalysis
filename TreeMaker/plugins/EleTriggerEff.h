#ifndef ELETRIGGEREFF_H
#define ELETRIGGEREFF_H

#include <vector>
#include <exception>

#include "TH2.h"
#include "TCanvas.h"

using std::vector;

// Store the combined electron trigger efficiencies for MC & Data
// This is taken from B2G-16-029 but their "orthogonal method" plots in backup of preapproval:
// https://indico.cern.ch/event/616149/contributions/2486779/attachments/1418259/2172181/preapproval_B2G-16-029_Huang_Huang_20170224.pdf
// And is only applicable for the combo 
// HLT_Ele45_WPLoose_Gsf_v* OR HLT_Ele115_CaloIdVT_GsfTrkIdT_v* OR HLT_Ele27_WPTight_Gsf_v*
// Their main trigger eff plots are for only Ele115

const static vector<float> ptBins = {45, 50, 55, 70, 100, 120, 180, 250, 1000};
const static vector<float> etaBins = {0, 0.8, 1.44, 1.566, 2, 2.4};

const static vector<vector<float>> data_eff = {
  {0.780594, 0.799735, 0.75, 0.679181, 0.574766}, // pt=45 to 50
  {0.825967, 0.833597, 0.916667, 0.757463, 0.593407}, // pt=50 to 55
  {0.866183, 0.856773, 0.714286, 0.739193, 0.698765},
  {0.882139, 0.893051, 0.8, 0.789272, 0.700637},
  {0.903775, 0.901726, 0.818182, 0.806897, 0.775862},
  {0.961097, 0.966073, 1, 0.972678, 0.975248},
  {0.959327, 0.968254, 1, 0.982906, 0.985294},
  {0.953846, 0.955696, 1, 1, 0.964286}
};
const static vector<vector<float>> data_eff_err = {
  {0.0122356, 0.0145744, 0.125, 0.0272702, 0.033795}, // pt=45 to 50
  {0.0115049, 0.0148267, 0.0797856, 0.026182, 0.03641}, // pt=50 to 55
  {0.00683513, 0.00891791, 0.0853735, 0.0166671, 0.0227977},
  {0.00561985, 0.00686098, 0.0676123, 0.0145745, 0.0211026},
  {0.00802318, 0.0108482, 0.116291, 0.0231795, 0.0316137},
  {0.00431834, 0.00527255, 0, 0.00852125, 0.0109318},
  {0.00739763, 0.00901765, 0, 0.0119835, 0.0145974},
  {0.0116386, 0.0163701, 0, 0, 0.0350707}
};

const static vector<vector<float>> mc_eff = {
  {0.812521, 0.805187, 0.83871, 0.792846, 0.721951},
  {0.823198, 0.853138, 0.83871, 0.80602, 0.743961},
  {0.864873, 0.878969, 0.821429, 0.849629, 0.784289},
  {0.899037, 0.912407, 0.787234, 0.86172, 0.82563},
  {0.936532, 0.943288, 0.857143, 0.890555, 0.909938},
  {0.986214, 0.986737, 0.976744, 0.988249, 0.989924},
  {0.985003, 0.989474, 0.989474, 0.983333, 0.981308},
  {0.98951, 0.992857, 1, 1, 1}
};

const static vector<vector<float>> mc_eff_err = {
  {0.00712223, 0.0095084, 0.0660586, 0.0156451, 0.0156451},
  {0.00739143, 0.00873795, 0.0660586, 0.0161697, 0.0161697},
  {0.00421471, 0.00526206, 0.041788, 0.00928167, 0.00928167},
  {0.00328547, 0.00411706, 0.0422123, 0.00818417, 0.00818417},
  {0.00420852, 0.00527572, 0.06613, 0.0120883, 0.0120883},
  {0.00167258, 0.00222687, 0.0229838, 0.00369406, 0.00369406},
  {0.00317322, 0.00370198, 0.0644061, 0.0082636, 0.0082636},
  {0.00425981, 0.00503269, 0, 0, 0 }
};

TH2F * electron_data_eff() {
  TH2F * mapEleEffData = new TH2F("mapEleEffData", "Electron trigger efficiency (data);|#eta^{e}|;p_{T}^{e} [GeV]", etaBins.size()-1, &etaBins[0], ptBins.size()-1, &ptBins[0]);
  if (data_eff.size() != ptBins.size()-1) throw std::runtime_error("Wrong pt size");
  if (data_eff_err.size() != ptBins.size()-1) throw std::runtime_error("Wrong pt size");
  
  for (uint ptInd=0; ptInd<ptBins.size()-1; ptInd++) {
    if (data_eff.at(ptInd).size() != etaBins.size()-1) throw std::runtime_error("Wrong eta size");
    if (data_eff_err.at(ptInd).size() != etaBins.size()-1) throw std::runtime_error("Wrong eta size");

    for (uint etaInd=0; etaInd<etaBins.size()-1; etaInd++) {
      mapEleEffData->SetBinContent(etaInd+1, ptInd+1, data_eff.at(ptInd).at(etaInd));
      mapEleEffData->SetBinError(etaInd+1, ptInd+1, data_eff_err.at(ptInd).at(etaInd));
    }
  }
  return mapEleEffData;
}

TH2F * electron_mc_eff() {
  TH2F * mapEleEffMC = new TH2F("mapEleEffMC", "Electron trigger efficiency (MC);|#eta^{e}|;p_{T}^{e} [GeV]", etaBins.size()-1, &etaBins[0], ptBins.size()-1, &ptBins[0]);
  if (mc_eff.size() != ptBins.size()-1) throw std::runtime_error("Wrong pt size");
  if (mc_eff_err.size() != ptBins.size()-1) throw std::runtime_error("Wrong pt size");

  for (uint ptInd=0; ptInd<ptBins.size()-1; ptInd++) {
    if (mc_eff.at(ptInd).size() != etaBins.size()-1) throw std::runtime_error("Wrong eta size");
    if (mc_eff_err.at(ptInd).size() != etaBins.size()-1) throw std::runtime_error("Wrong eta size");

    for (uint etaInd=0; etaInd<etaBins.size()-1; etaInd++) {
      mapEleEffMC->SetBinContent(etaInd+1, ptInd+1, mc_eff.at(ptInd).at(etaInd));
      mapEleEffMC->SetBinError(etaInd+1, ptInd+1, mc_eff_err.at(ptInd).at(etaInd));
    }
  }
  return mapEleEffMC;
}

#endif
