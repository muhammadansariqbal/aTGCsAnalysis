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
            throw std::runtime_error("Cannot get object " + object_name + " from file " + filename);
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
                throw cms::Exception("Cannot get bin - value below low edge of first bin");
            } else {
                bin_num = 1;
            }
        } else if (bin_num == ax->GetNbins()+1) {
            if (oob_throw) {
                throw cms::Exception("Cannot get bin - value above high edge of last bin");
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
        if (error != "up" && error != "down") throw cms::Exception("histogramFromGraph error must be up or down");

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

        gr_tracking_sf_phi.reset((TGraphAsymmErrors*) getHistFromFile(tracking_sf_filename, "ratio_eff_phi_dr030e030_corr"));
        hist_tracking_sf_phi_err_up.reset(histogramFromGraph(gr_tracking_sf_phi.get(), "up"));
        hist_tracking_sf_phi_err_down.reset(histogramFromGraph(gr_tracking_sf_phi.get(), "down"));

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

    float getTrackingScaleFactor(float eta, float phi, int nVertices) {
        float eta_sf = getBinContent(hist_tracking_sf_eta_err_up.get(), eta);
        float phi_sf = getBinContent(hist_tracking_sf_phi_err_up.get(), phi);
        float npv_sf = getBinContent(hist_tracking_sf_nPV_err_up.get(), nVertices);
        return eta_sf * phi_sf * npv_sf;
    };

    float getIDScaleFactor(float pt, float eta, int nVertices) {
        float npv_sf = getBinContent(hist_id_sf_nPV.get(), nVertices);
        float pt_eta_sf = getBinContent(hist_id_sf_pt_eta.get(), pt, fabs(eta));
        return npv_sf * pt_eta_sf;
    }

    float getIsoScaleFactor(float pt, float eta, int nVertices) {
        float npv_sf = getBinContent(hist_iso_sf_nPV.get(), nVertices);
        float pt_eta_sf = getBinContent(hist_iso_sf_pt_eta.get(), pt, fabs(eta));
        return npv_sf * pt_eta_sf;
    }

    float getTriggerScaleFactor(float pt, float eta) {
        float pt_eta_sf = getBinContent(hist_trigger_sf_pt_eta.get(), pt, fabs(eta));
        return pt_eta_sf;
    }

    float getScaleFactor(float pt, float eta, float phi, int nVertices) {
        return (getTrackingScaleFactor(eta, phi, nVertices)
                * getIDScaleFactor(pt, eta, nVertices)
                * getIsoScaleFactor(pt, eta, nVertices)
                * getTriggerScaleFactor(pt, eta));
    };

    // TODO some variation up/down

private:
    std::unique_ptr<TGraphAsymmErrors> gr_tracking_sf_eta;
    std::unique_ptr<TH1F> hist_tracking_sf_eta_err_up; // used to hold graph data, with bin error = graph up error
    std::unique_ptr<TH1F> hist_tracking_sf_eta_err_down; // similar but bin error = graph down error
    std::unique_ptr<TGraphAsymmErrors> gr_tracking_sf_phi;
    std::unique_ptr<TH1F> hist_tracking_sf_phi_err_up;
    std::unique_ptr<TH1F> hist_tracking_sf_phi_err_down;
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
