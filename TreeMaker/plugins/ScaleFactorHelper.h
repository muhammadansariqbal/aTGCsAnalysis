#include "TH1.h"
#include "TH2.h"
#include "TGraphAsymmErrors.h"

#include <iostream>
#include <exception>
#include <sstream>
#include <string>
#include <algorithm>
#include <iterator>
#include <vector>
#include <utility>

/**
  * Abstract base class for dealing with scale factors
 **/
class ScaleFactorBase
{
public:
  ScaleFactorBase(): throw_oob_(false), uid(0) {}

  virtual TObject * getObjFromFile(std::string filename, std::string object_name) {
    edm::FileInPath full_filename(filename);
    TFile file(full_filename.fullPath().c_str());
    if (file.IsZombie()) {
      throw cms::Exception("FileNotFound") << "Cannot open file " + filename;
    }
    TObject * obj = (TObject*)file.Get(object_name.c_str());
    if (obj == nullptr) {
      throw cms::Exception("InvalidObject") << "Cannot get object " + object_name + " from file " + filename;
    }
    return obj;
  }

  virtual TH1 * getHistFromFile(std::string filename, std::string object_name) {
    // Ideally I'd like to just use the TObject version of this and dynamic_cast,
    // but for some reason that seg faults.
    edm::FileInPath full_filename(filename);
    TFile file(full_filename.fullPath().c_str());
    if (file.IsZombie()) {
      throw cms::Exception("FileNotFound") << "Cannot open file " + filename;
    }
    TH1 * hist = (TH1*) file.Get(object_name.c_str());
    if (hist == nullptr) {
      throw cms::Exception("InvalidObject") << "Cannot get object " + object_name + " from file " + filename;
    }
    hist->SetDirectory(0);
    return hist;
  }

  virtual int findBinNum(TAxis * ax, float value, bool oob_throw) {
    // Do FindBin() but can either:
    // - "oob_throw = true" : if out of range throw an exception
    // - "oob_throw = false" : if out of range set to bin 1 or N, as necessary
    int bin_num = ax->FindBin(value);
    if (bin_num == 0) {
      if (oob_throw) {
        throw cms::Exception("IndexError") << "Cannot get bin - value below low edge of first bin";
      } else {
        bin_num = 1;
      }
    } else if (bin_num == ax->GetNbins()+1) {
      if (oob_throw) {
        throw cms::Exception("IndexError") << "Cannot get bin - value above high edge of last bin";
      } else {
        bin_num = ax->GetNbins();
      }
    }
    return bin_num;
  }

  virtual float getBinContent(TH1 * h, float xval) {
    int xbin_num = findBinNum(h->GetXaxis(), xval, throw_oob_);
    return h->GetBinContent(xbin_num);
  };

  virtual float getBinError(TH1 * h, float xval) {
    int xbin_num = findBinNum(h->GetXaxis(), xval, throw_oob_);
    return h->GetBinError(xbin_num);
  };

  virtual float getBinContent(TH2 * h, float xval, float yval) {
    int xbin_num = findBinNum(h->GetXaxis(), xval, throw_oob_);
    int ybin_num = findBinNum(h->GetYaxis(), yval, throw_oob_);
    return h->GetBinContent(xbin_num, ybin_num);
  };

  virtual float getBinError(TH2 * h, float xval, float yval) {
    int xbin_num = findBinNum(h->GetXaxis(), xval, throw_oob_);
    int ybin_num = findBinNum(h->GetYaxis(), yval, throw_oob_);
    return h->GetBinError(xbin_num, ybin_num);
  };

  // TODO: template this?
  virtual std::string flatten_vector(std::vector<std::string> v, std::string delim) {
    std::stringstream s;
    std::copy(v.begin(), v.end(), std::ostream_iterator<std::string>(s, delim.c_str()));
    return s.str();
  }

  virtual float propagateUncert(const std::vector<std::pair<float, float>> & value_unc_pairs) {
    // Correctly calculate error for A*B*C*... where each has an associated uncertainty.
    // Input is pairs of {value, uncertainty}
    float total = 0;
    for (uint i = 0; i < value_unc_pairs.size(); i++) {
      float prod = 1.;
      for (uint j = 0; j < value_unc_pairs.size(); j++) {
        if (i == j) continue;
        prod *= value_unc_pairs.at(j).first;
      }
      float entry = value_unc_pairs.at(i).second * prod;
      total += std::pow(entry, 2);
    }
    return std::sqrt(total);
  }

  virtual void printVectorOfPairs(std::vector<std::pair<float, float>> vals) {
    // Helper method to check contents of vector of pairs of floats
    for (uint i = 0; i < vals.size(); i++) {
      std::cout << vals.at(i).first << " : " << vals.at(i).second << std::endl;
    }
  }

protected:
  bool throw_oob_;
  int uid; // For creating hists with unique names
};


/**
 * Class to handle scale factor from a TGraphAsymmErrors, since it's a pain
 * as doesn't have convenient FindBin().
 * Also needs to handle different up & down errors.
 **/
class ScaleFactorSourceTGraph : ScaleFactorBase {
public:
  ScaleFactorSourceTGraph(const std::string & filename, const std::string & graph_name) {
    gr_sf.reset((TGraphAsymmErrors*) getObjFromFile(filename, graph_name));
    hist_sf_err_up.reset(histogramFromGraph(gr_sf.get(), "up"));
    hist_sf_err_down.reset(histogramFromGraph(gr_sf.get(), "down"));
  }

  float getScaleFactor(float xval) {
    return getBinContent(hist_sf_err_up.get(), xval);
  }

  float getScaleFactorUncert(float xval, std::string variation) {
    if (variation == "up") {
      return getBinError(hist_sf_err_up.get(), xval);
    } else if (variation == "down") {
      return getBinError(hist_sf_err_down.get(), xval);
    } else {
      throw cms::Exception("InvalidArgument") << "getScaleFactorUncert() variation must be up or down";
    }
  }

  virtual TH1F * histogramFromGraph(TGraphAsymmErrors * g, const std::string & error="up") {
    // Convert graph to a histogram. Sets bin errors to either be up or down errors
    if (error != "up" && error != "down") throw cms::Exception("InvalidValue") << "histogramFromGraph error must be up or down";

    // Setup the binning
    // IMPORTANT: TGraphs are 0-indexed, whilst TH1s are 1-indexed. Insanity.
    std::vector<float> bins = {};
    uint n_bins = g->GetN();
    double * x = g->GetX();
    double * ex_low = g->GetEXlow();
    for (uint i = 0; i < n_bins; i++) {
      bins.push_back(x[i] - ex_low[i]);
    }
    bins.push_back(x[n_bins-1] + g->GetErrorXhigh(n_bins-1)); // get the upper edge of the last bin

    TH1F * h = new TH1F(TString::Format("hist_%s_%s", g->GetName(), error.c_str()), g->GetTitle(), n_bins, &bins[0]);
    h->SetDirectory(0);

    // Fill the hist
    double * y = g->GetY();
    for (uint i=1; i <= n_bins; i++) {
      h->SetBinContent(i, y[i-1]);
      if (error == "up") h->SetBinError(i, g->GetErrorYhigh(i-1));
      else if (error == "down") h->SetBinError(i, g->GetErrorYlow(i-1));
    }
    return h;
  }

private:
  std::unique_ptr<TGraphAsymmErrors> gr_sf;
  std::unique_ptr<TH1F> hist_sf_err_up;  // use to hold graph data but with bin error = upwards error
  std::unique_ptr<TH1F> hist_sf_err_down;  // ditto with bin error = downwards error
};

/**
 * Specific implementation for Muons
 * Note that one instance of this class is designed for one specific ID/ISO working point
 **/
class MuonScaleFactor : ScaleFactorBase {
public:
  MuonScaleFactor(const std::string & tracking_sf_filename,
                  const std::string & id_sf_filename,
                  const std::string & iso_sf_filename,
                  const std::string & trigger_sf_filename,
                  const std::string & id,  // ID and Isolation working point
                  const std::string & iso) {

    // Possible ID and isolation options, to get correct TDirectory in ROOT files
    // TODO: maybe the dirs should just be user configurable? But then lots of ctor args...
    std::vector<std::string> id_options = {"LooseID", "MediumID2016", "MediumID", "HighPtID"};
    std::vector<std::string> iso_options = {"LooseISO", "tkLooseISO", "TightISO"};

    // Check if id and iso args are acceptable
    if (std::find(id_options.begin(), id_options.end(), id) == id_options.end()) {
      throw std::runtime_error("MuonScaleFactor id string must be one of {" + flatten_vector(id_options, ", ") + "}");
    }
    if (std::find(iso_options.begin(), iso_options.end(), iso) == iso_options.end()) {
      throw std::runtime_error("MuonScaleFactor iso string must be one of {" + flatten_vector(iso_options, ", ") + "}");
    }

    // This is a bit of a hack - to easily retrieve the scale factors & errors, we first convert
    // our graph into a histogram, since TGraph doesn't have a simple FindBin().
    // Using getHistFromFile() on a TGraph is a bit of a hack, but casting the ptr seems to work...
    tracking_sf_eta.reset(new ScaleFactorSourceTGraph(tracking_sf_filename, "ratio_eff_eta3_dr030e030_corr"));
    tracking_sf_nPV.reset(new ScaleFactorSourceTGraph(tracking_sf_filename, "ratio_eff_vtx_dr030e030_corr"));

    hist_id_sf_nPV.reset((TH1F*) getHistFromFile(id_sf_filename, "MC_NUM_"+id+"_DEN_genTracks_PAR_vtx/tag_nVertices_ratio"));
    std::string pt_eta_hist_name = "pt_abseta_ratio";
    if (id == "HighPtID") {pt_eta_hist_name = "pair_ne_ratio";}
    hist_id_sf_pt_eta.reset((TH2F*) getHistFromFile(id_sf_filename, "MC_NUM_"+id+"_DEN_genTracks_PAR_pt_eta/"+pt_eta_hist_name));

    std::string iso_id(id);
    // FIXME: handle non-valid combinations?
    if (iso_id == "HighPtID") iso_id = "highptID"; // since people can't name their TDirectories consistently
    hist_iso_sf_nPV.reset((TH1F*) getHistFromFile(iso_sf_filename, iso+"_"+iso_id+"_vtx/tag_nVertices_ratio"));
    hist_iso_sf_pt_eta.reset((TH2F*) getHistFromFile(iso_sf_filename, iso+"_"+iso_id+"_pt_eta/"+pt_eta_hist_name));

    hist_trigger_sf_pt_eta.reset((TH2F*) getHistFromFile(trigger_sf_filename, "Mu50_OR_TkMu50_PtEtaBins/pt_abseta_ratio"));
  };

  // These are the recommended systematic uncertainties as *fractions* not %
  // Update as necessary from: https://twiki.cern.ch/twiki/bin/view/CMS/MuonReferenceEffsRun2#Results_for_2016_data
  // NB: these are 2015! Still waiting for 2016 recommendations
  const float systematic_ID = 0.01;
  const float systematic_Iso = 0.005;
  const float systematic_Trigger = 0.005;

  float getTrackingScaleFactor(float eta, int nVertices) {
    // NB although lumi and/or phi graphs in ROOT files, only recommended to use eta & nPV
    // (prob wise to check this periodically)
    return tracking_sf_eta->getScaleFactor(eta) * tracking_sf_nPV->getScaleFactor(nVertices);
  };

  float getTrackingScaleFactorUncert(float eta, int nVertices, const std::string & variation) {
    // Get statistical + systematic uncertainty on SF
    // Tracking SF are the only ones that come with different up/down errors
    float eta_sf = tracking_sf_eta->getScaleFactor(eta);
    float npv_sf = tracking_sf_nPV->getScaleFactor(nVertices);
    float eta_sf_uncert = tracking_sf_eta->getScaleFactorUncert(eta, variation);
    float npv_sf_uncert = tracking_sf_nPV->getScaleFactorUncert(nVertices, variation);

    std::vector<std::pair<float, float>> vals = {
      std::make_pair(eta_sf, eta_sf_uncert),
      std::make_pair(npv_sf, npv_sf_uncert),
    };
    float stat_uncert = propagateUncert(vals);
    return stat_uncert;
  };

  float getIDScaleFactor(float pt, float eta, int nVertices) {
    float npv_sf = getBinContent(hist_id_sf_nPV.get(), nVertices);
    float pt_eta_sf = getBinContent(hist_id_sf_pt_eta.get(), pt, fabs(eta));
    return npv_sf * pt_eta_sf;
  }

  float getIDScaleFactorUncert(float pt, float eta, int nVertices) {
    // Get statistical + systematic uncertainty on SF
    float npv_sf = getBinContent(hist_id_sf_nPV.get(), nVertices);
    float npv_sf_uncert = getBinError(hist_id_sf_nPV.get(), nVertices);
    float pt_eta_sf = getBinContent(hist_id_sf_pt_eta.get(), pt, fabs(eta));
    float pt_eta_sf_uncert = getBinError(hist_id_sf_pt_eta.get(), pt, fabs(eta));
    std::vector<std::pair<float, float>> vals = {
      std::make_pair(pt_eta_sf, pt_eta_sf_uncert),
      std::make_pair(npv_sf, npv_sf_uncert),
    };
    float stat_uncert = propagateUncert(vals);
    float syst_uncert = systematic_ID * npv_sf * pt_eta_sf;
    return std::hypot(stat_uncert, syst_uncert);
  }

  float getIsoScaleFactor(float pt, float eta, int nVertices) {
    float npv_sf = getBinContent(hist_iso_sf_nPV.get(), nVertices);
    float pt_eta_sf = getBinContent(hist_iso_sf_pt_eta.get(), pt, fabs(eta));
    return npv_sf * pt_eta_sf;
  }

  float getIsoScaleFactorUncert(float pt, float eta, int nVertices) {
    // Get statistical + systematic uncertainty on SF
    float npv_sf = getBinContent(hist_iso_sf_nPV.get(), nVertices);
    float npv_sf_uncert = getBinError(hist_iso_sf_nPV.get(), nVertices);
    float pt_eta_sf = getBinContent(hist_iso_sf_pt_eta.get(), pt, fabs(eta));
    float pt_eta_sf_uncert = getBinError(hist_iso_sf_pt_eta.get(), pt, fabs(eta));
    std::vector<std::pair<float, float>> vals = {
      std::make_pair(pt_eta_sf, pt_eta_sf_uncert),
      std::make_pair(npv_sf, npv_sf_uncert),
    };
    float stat_uncert = propagateUncert(vals);
    float syst_uncert = systematic_Iso * npv_sf * pt_eta_sf;
    return std::hypot(stat_uncert, syst_uncert);
  }

  float getTriggerScaleFactor(float pt, float eta) {
    float pt_eta_sf = getBinContent(hist_trigger_sf_pt_eta.get(), pt, fabs(eta));
    return pt_eta_sf;
  }

  float getTriggerScaleFactorUncert(float pt, float eta) {
    // Get statistical + systematic uncertainty on SF
    float pt_eta_sf_uncert = getBinError(hist_trigger_sf_pt_eta.get(), pt, fabs(eta));
    float syst_uncert = systematic_Trigger * getTriggerScaleFactor(pt, eta);
    return std::hypot(pt_eta_sf_uncert, syst_uncert);
  }

  float getScaleFactor(float pt, float eta, float phi, int nVertices, const std::string & variation="") {
    // Variation here is SF +/- uncertainty, where uncertainty is BOTH statistical & systematic
    // Systematic variations may need updating as necessary, see
    // https://twiki.cern.ch/twiki/bin/view/CMS/MuonReferenceEffsRun2
    float tracking_sf = getTrackingScaleFactor(eta, nVertices);
    float id_sf = getIDScaleFactor(pt, eta, nVertices);
    float iso_sf = getIsoScaleFactor(pt, eta, nVertices);
    float trigger_sf = getTriggerScaleFactor(pt, eta);
    float total_var = 0;
    if (variation == "up" || variation == "down") {
      std::vector<std::pair<float, float>> vals = {
        std::make_pair(tracking_sf, getTrackingScaleFactorUncert(eta, nVertices, variation)),
        std::make_pair(id_sf, getIDScaleFactorUncert(pt, eta, nVertices)),
        std::make_pair(iso_sf, getIsoScaleFactorUncert(pt, eta, nVertices)),
        std::make_pair(trigger_sf, getTriggerScaleFactorUncert(pt, eta))
      };
      total_var = propagateUncert(vals);
      if (variation == "down") {
        total_var *= -1.;
      }
    } else if (variation != "") {
      throw cms::Exception("InvalidValue") << "variation arg must be \"\", \"up\", or \"down\"";
    }
    return (tracking_sf * id_sf * iso_sf * trigger_sf) + total_var;
  };

private:
  std::unique_ptr<ScaleFactorSourceTGraph> tracking_sf_eta;
  std::unique_ptr<ScaleFactorSourceTGraph> tracking_sf_nPV;

  std::unique_ptr<TH1F> hist_id_sf_nPV;
  std::unique_ptr<TH2F> hist_id_sf_pt_eta;

  std::unique_ptr<TH1F> hist_iso_sf_nPV;
  std::unique_ptr<TH2F> hist_iso_sf_pt_eta;

  std::unique_ptr<TH2F> hist_trigger_sf_pt_eta;
};


/**
 * Specific implementation for Electron SF
 **/
class ElectronScaleFactor : ScaleFactorBase {
public:
  ElectronScaleFactor(const std::string & sf_filename) {
    sf_nPV_barrel.reset(new ScaleFactorSourceTGraph(sf_filename, "SF_Nvtx_Barrel"));
    sf_nPV_endcap.reset(new ScaleFactorSourceTGraph(sf_filename, "SF_Nvtx_Endcap"));

    sf_et_barrel.reset(new ScaleFactorSourceTGraph(sf_filename, "SF_Et_Barrel"));
    sf_et_endcap.reset(new ScaleFactorSourceTGraph(sf_filename, "SF_Et_Endcap"));

    sf_phi_barrel.reset(new ScaleFactorSourceTGraph(sf_filename, "SF_Phi_Barrel"));
    sf_phi_endcap.reset(new ScaleFactorSourceTGraph(sf_filename, "SF_Phi_Endcap"));

    sf_eta.reset(new ScaleFactorSourceTGraph(sf_filename, "SF_Eta_Ordered"));
  };

  float getScaleFactor(float et, float eta, float phi, int nVertices, const std::string & variation="") {
    // Get total SF, optionally with combined stat and systematic uncert added/subtracted
    std::vector<std::pair<float, float>> vals;
    vals.resize(4);

    ScaleFactorSourceTGraph *sfs_nPV, *sfs_et, *sfs_phi;
    float syst_uncert(0.);

    if (fabs(eta) <= BARREL_ETA_MAX) {
      sfs_nPV = sf_nPV_barrel.get();
      sfs_et = sf_et_barrel.get();
      sfs_phi = sf_phi_barrel.get();

      // HEEP Syst uncert as recommended by EGamma POG:
      // https://twiki.cern.ch/twiki/bin/view/CMS/EgammaIDRecipesRun2#Electron_efficiencies_and_scale
      if (et < 90) {
        syst_uncert = 0.01;
      } else if (et < 1000) {
        syst_uncert = 0.02;
      } else {
        syst_uncert = 0.03;
      }
    } else {
      sfs_nPV = sf_nPV_endcap.get();
      sfs_et = sf_et_endcap.get();
      sfs_phi = sf_phi_endcap.get();

      // Syst uncert
      if (et < 90) {
        syst_uncert = 0.01;
      } else if (et < 300) {
        syst_uncert = 0.02;
      } else {
        syst_uncert = 0.04;
      }
    }

    float eta_sf = sf_eta->getScaleFactor(eta);
    float nPV_sf = sfs_nPV->getScaleFactor(nVertices);
    float et_sf = sfs_et->getScaleFactor(et);
    float phi_sf = sfs_phi->getScaleFactor(phi);

    float total_sf = eta_sf * nPV_sf * et_sf * phi_sf;

    if (variation == "") {
      return total_sf;
    }

    syst_uncert *= total_sf;

    vals.push_back(std::make_pair(eta_sf, sf_eta->getScaleFactorUncert(eta, variation)));
    vals.push_back(std::make_pair(nPV_sf, sfs_nPV->getScaleFactorUncert(nVertices, variation)));
    vals.push_back(std::make_pair(et_sf, sfs_et->getScaleFactorUncert(et, variation)));
    vals.push_back(std::make_pair(phi_sf, sfs_phi->getScaleFactorUncert(phi, variation)));

    float stat_uncert = propagateUncert(vals);

    float total_uncert = std::hypot(stat_uncert, syst_uncert);
    if (variation == "down") {
        total_uncert *= -1;
    }
    return total_sf + total_uncert;
  };

private:
  const float BARREL_ETA_MAX = 1.4442;

  std::unique_ptr<ScaleFactorSourceTGraph> sf_nPV_barrel;
  std::unique_ptr<ScaleFactorSourceTGraph> sf_nPV_endcap;

  std::unique_ptr<ScaleFactorSourceTGraph> sf_et_barrel;
  std::unique_ptr<ScaleFactorSourceTGraph> sf_et_endcap;

  std::unique_ptr<ScaleFactorSourceTGraph> sf_phi_barrel;
  std::unique_ptr<ScaleFactorSourceTGraph> sf_phi_endcap;

  std::unique_ptr<ScaleFactorSourceTGraph> sf_eta;
};
