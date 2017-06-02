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

  virtual TH1 * getHistFromFile(std::string filename, std::string object_name) {
    edm::FileInPath full_filename(filename);
    TFile file(full_filename.fullPath().c_str());
    TH1 * hist = (TH1*)file.Get(object_name.c_str());
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

  virtual TH1F * histogramFromGraph(TGraphAsymmErrors * g, const std::string & error="up") {
    // Convert graph to a histogram. Sets bin errors to either be up or down errors
    if (error != "up" && error != "down") throw cms::Exception("InvalidValue") << "histogramFromGraph error must be up or down";

    // Setup the binning
    std::vector<float> bins = {};
    uint n_bins = g->GetN();
    double * x = g->GetX();
    double * ex_low = g->GetEXlow();
    for (uint i = 0; i < n_bins; i++) {
      bins.push_back(x[i] - ex_low[i]);
    }
    bins.push_back(x[n_bins-1] + g->GetErrorXhigh(n_bins)); // do the upper edge of the last bin

    TH1F * h = new TH1F(TString::Format("hist_%d", uid), g->GetTitle(), n_bins, &bins[0]);
    uid++;
    h->SetDirectory(0);

    // Fill the hist
    double * y = g->GetY();
    for (uint i=1; i <= n_bins; i++) {
      h->SetBinContent(i, y[i-1]);
      if (error == "up") h->SetBinError(i, g->GetErrorYhigh(i));
      else if (error == "down") h->SetBinError(i, g->GetErrorYhigh(i));
    }
    return h;
  }

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
    gr_tracking_sf_eta.reset((TGraphAsymmErrors*) getHistFromFile(tracking_sf_filename, "ratio_eff_eta3_dr030e030_corr"));
    hist_tracking_sf_eta_err_up.reset(histogramFromGraph(gr_tracking_sf_eta.get(), "up"));
    hist_tracking_sf_eta_err_down.reset(histogramFromGraph(gr_tracking_sf_eta.get(), "down"));

    gr_tracking_sf_nPV.reset((TGraphAsymmErrors*) getHistFromFile(tracking_sf_filename, "ratio_eff_vtx_dr030e030_corr"));
    hist_tracking_sf_nPV_err_up.reset(histogramFromGraph(gr_tracking_sf_nPV.get(), "up"));
    hist_tracking_sf_nPV_err_down.reset(histogramFromGraph(gr_tracking_sf_nPV.get(), "down"));

    hist_id_sf_nPV.reset((TH1F*) getHistFromFile(id_sf_filename, "MC_NUM_"+id+"_DEN_genTracks_PAR_vtx/tag_nVertices_ratio"));
    std::string pt_eta_hist_name = "pt_abseta_ratio";
    std::string pt_tag = "pt";
    if (id == "HighPtID") {pt_eta_hist_name = "pair_ne_ratio"; pt_tag = "newpt";}
    hist_id_sf_pt_eta.reset((TH2F*) getHistFromFile(id_sf_filename, "MC_NUM_"+id+"_DEN_genTracks_PAR_"+pt_tag+"_eta/"+pt_eta_hist_name));

    std::string iso_id(id);
    // FIXME: handle non-valid combinations?
    if (iso_id == "HighPtID") iso_id = "highptID"; // since people can't name their TDirectories consistently
    hist_iso_sf_nPV.reset((TH1F*) getHistFromFile(iso_sf_filename, iso+"_"+iso_id+"_vtx/tag_nVertices_ratio"));
    hist_iso_sf_pt_eta.reset((TH2F*) getHistFromFile(iso_sf_filename, iso+"_"+iso_id+"_"+pt_tag+"_eta/"+pt_eta_hist_name));

    hist_trigger_sf_pt_eta.reset((TH2F*) getHistFromFile(trigger_sf_filename, "Mu50_OR_TkMu50_PtEtaBins/pt_abseta_ratio"));
  };

  // These are the recommended systematic uncertainties as *fractions* not %
  // Update as necessary from: https://twiki.cern.ch/twiki/bin/view/CMS/MuonReferenceEffsRun2#Results_for_2016_data
  // NB: these are 2015! Still waiting for 2016 recommendations
  const float systematic_ID = 0.01;
  const float systematic_Iso = 0.005;
  const float systematic_Trigger = 0.005;

  float getTrackingScaleFactor(float eta, float phi, int nVertices) {
    // NB although lumi and/or phi graphs in ROOT files, only recommended to use eta & nPV
    // (prob wise to check this periodically)
    float eta_sf = getBinContent(hist_tracking_sf_eta_err_up.get(), eta);
    float npv_sf = getBinContent(hist_tracking_sf_nPV_err_up.get(), nVertices);
    return eta_sf * npv_sf;
  };

  float getTrackingScaleFactorUncert(float eta, float phi, int nVertices, const std::string & variation) {
    // Get statistical + systematic uncertainty on SF
    // Tracking SF are the only ones that come with different up/down errors

    // Central values don't depend on the up/down error so doesn't matter which hist we use
    float eta_sf = getBinContent(hist_tracking_sf_eta_err_up.get(), eta);
    float npv_sf = getBinContent(hist_tracking_sf_nPV_err_up.get(), nVertices);

    float eta_sf_uncert(0.), npv_sf_uncert(0.);
    if (variation == "up") {
      eta_sf_uncert = getBinError(hist_tracking_sf_eta_err_up.get(), eta);
      npv_sf_uncert = getBinError(hist_tracking_sf_nPV_err_up.get(), nVertices);
    } else if (variation == "down") {
      eta_sf_uncert = getBinError(hist_tracking_sf_eta_err_down.get(), eta);
      npv_sf_uncert = getBinError(hist_tracking_sf_nPV_err_down.get(), nVertices);
    }
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
    float tracking_sf = getTrackingScaleFactor(eta, phi, nVertices);
    float id_sf = getIDScaleFactor(pt, eta, nVertices);
    float iso_sf = getIsoScaleFactor(pt, eta, nVertices);
    float trigger_sf = getTriggerScaleFactor(pt, eta);
    float total_var = 0;
    if (variation == "up" || variation == "down") {
      std::vector<std::pair<float, float>> vals = {
        std::make_pair(tracking_sf, getTrackingScaleFactorUncert(eta, phi, nVertices, variation)),
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
  std::unique_ptr<TGraphAsymmErrors> gr_tracking_sf_eta;
  std::unique_ptr<TH1F> hist_tracking_sf_eta_err_up; // used to hold graph data, with bin error = graph up error
  std::unique_ptr<TH1F> hist_tracking_sf_eta_err_down; // similar but bin error = graph down error
  std::unique_ptr<TGraphAsymmErrors> gr_tracking_sf_nPV;
  std::unique_ptr<TH1F> hist_tracking_sf_nPV_err_up;
  std::unique_ptr<TH1F> hist_tracking_sf_nPV_err_down;

  std::unique_ptr<TH1F> hist_id_sf_nPV;
  std::unique_ptr<TH2F> hist_id_sf_pt_eta;

  std::unique_ptr<TH1F> hist_iso_sf_nPV;
  std::unique_ptr<TH2F> hist_iso_sf_pt_eta;

  std::unique_ptr<TH2F> hist_trigger_sf_pt_eta;
};


class ElectronScaleFactor : ScaleFactorBase {
public:
  ElectronScaleFactor() {};

  float getScaleFactor();

private:
  TH1F * tracking_sf;
  TH1F * id_sf;
  TH1F * iso_sf;
};
