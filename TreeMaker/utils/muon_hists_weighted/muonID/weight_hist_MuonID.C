#include "TH1.h"
#include "TH2.h"
#include "vector"

using namespace std;

/**
 * Create lumi-weighted ID scale factors.
 * Original script by Anna Benecke
 */

void weight_hist_MuonID(){

  //read in .root files to weight
  TFile * trigger_BCDEF = new TFile("Muon_IDSF_RunBtoF.root");
  TFile * trigger_GH = new TFile("Muon_IDSF_RunGtoH.root");
  double lumi_BCDEF = 19.695456271;
  double lumi_GH = 16.226452636;
  double lumi_tot = lumi_BCDEF + lumi_GH;
  TFile * trigger_allRuns = new TFile("Muon_IDSF_average_RunBtoH.root", "RECREATE");
  std::vector<TString> ID={"LooseID","MediumID2016","MediumID","TightID", "HighPtID"};
  std::vector<TString> var={"eta/eta_ratio","pt/pt_ratio","vtx/tag_nVertices_ratio"};

  //Define all variables
  TH1F* h_MU24_BCDEF_eta_ratio ;
  TH2F* h_MU24_BCDEF_abseta_pt_ratio;
  TH2F* h_MU24_BCDEF_pt_abseta_ratio ;
  TH1F* h_MU24_BCDEF_pt_ratio ;
  TH1F* h_MU24_BCDEF_tag_nVertices_ratio;
  TH1F* h_MU24_GH_eta_ratio ;
  TH2F* h_MU24_GH_abseta_pt_ratio;
  TH2F* h_MU24_GH_pt_abseta_ratio;
  TH1F* h_MU24_GH_pt_ratio ;
  TH1F* h_MU24_GH_tag_nVertices_ratio;
  TH1F* new_MU24_eta_ratio ;
  TH2F* new_MU24_abseta_pt_ratio;
  TH2F* new_MU24_pt_abseta_ratio;
  TH1F* new_MU24_pt_ratio;
  TH1F* new_MU24_tag_nVertices_ratio;
  TDirectory *dir1,*dir2,*dir3,*dir4;
  TString hist;

  for(int i=0;i<ID.size();i++){

    if(ID[i]=="HighPtID" ){
      var={"eta/eta_ratio","newpt/pair_ne_ratio", "vtx/tag_nVertices_ratio"};
    }

    for(int j=0;j<var.size();j++){
      //read in hists to weight
      hist= "MC_NUM_";
      hist+=ID[i];
      hist+="_DEN_genTracks_PAR_";
      hist+=var[j];

      // Run BCDEF & GH
      h_MU24_BCDEF_eta_ratio = (TH1F*)trigger_BCDEF->Get(hist);
      h_MU24_GH_eta_ratio = (TH1F*)trigger_GH->Get(hist);

      //Scale
      h_MU24_BCDEF_eta_ratio->Sumw2();
      h_MU24_BCDEF_eta_ratio->Scale(lumi_BCDEF);
      h_MU24_GH_eta_ratio->Sumw2();
      h_MU24_GH_eta_ratio->Scale(lumi_GH);

      // Add Histogramm Run BCDEF + GH
      h_MU24_BCDEF_eta_ratio->Add(h_MU24_GH_eta_ratio);
      // Save new histogramms as new histogramms
      new_MU24_eta_ratio = (TH1F*)h_MU24_BCDEF_eta_ratio;
      // Save new histogramms as new histogramms
      new_MU24_eta_ratio->Sumw2();
      new_MU24_eta_ratio->Scale(1/lumi_tot);

      dir1 =trigger_allRuns->mkdir("MC_NUM_"+ID[i]+"_DEN_genTracks_PAR_"+var[j]);
      dir1->cd();
      new_MU24_eta_ratio->Write();

    }//for-loop j

    if(ID[i]=="HighPtID" ){
      // Run BCDEF
      h_MU24_BCDEF_abseta_pt_ratio = (TH2F*)trigger_BCDEF->Get("MC_NUM_"+ID[i]+"_DEN_genTracks_PAR_newpt_eta/abseta_pair_ne_ratio");
      h_MU24_BCDEF_pt_abseta_ratio = (TH2F*)trigger_BCDEF->Get("MC_NUM_"+ID[i]+"_DEN_genTracks_PAR_newpt_eta/pair_ne_ratio");

      // Run GH
      h_MU24_GH_abseta_pt_ratio = (TH2F*)trigger_GH->Get("MC_NUM_"+ID[i]+"_DEN_genTracks_PAR_newpt_eta/abseta_pair_ne_ratio");
      h_MU24_GH_pt_abseta_ratio = (TH2F*)trigger_GH->Get("MC_NUM_"+ID[i]+"_DEN_genTracks_PAR_newpt_eta/pair_ne_ratio");

    }else{
    // Run BCDEF
      h_MU24_BCDEF_abseta_pt_ratio = (TH2F*)trigger_BCDEF->Get("MC_NUM_"+ID[i]+"_DEN_genTracks_PAR_pt_eta/abseta_pt_ratio");
      h_MU24_BCDEF_pt_abseta_ratio = (TH2F*)trigger_BCDEF->Get("MC_NUM_"+ID[i]+"_DEN_genTracks_PAR_pt_eta/pt_abseta_ratio");

      // Run GH
      h_MU24_GH_abseta_pt_ratio = (TH2F*)trigger_GH->Get("MC_NUM_"+ID[i]+"_DEN_genTracks_PAR_pt_eta/abseta_pt_ratio");
      h_MU24_GH_pt_abseta_ratio = (TH2F*)trigger_GH->Get("MC_NUM_"+ID[i]+"_DEN_genTracks_PAR_pt_eta/pt_abseta_ratio");
    }
      ////////////                     Scale                     /////////////////////////////////////
      h_MU24_BCDEF_abseta_pt_ratio->Sumw2();
      h_MU24_BCDEF_abseta_pt_ratio->Scale(lumi_BCDEF);
      h_MU24_BCDEF_pt_abseta_ratio->Sumw2();
      h_MU24_BCDEF_pt_abseta_ratio->Scale(lumi_BCDEF);

      h_MU24_GH_abseta_pt_ratio->Sumw2();
      h_MU24_GH_abseta_pt_ratio->Scale(lumi_GH);
      h_MU24_GH_pt_abseta_ratio->Sumw2();
      h_MU24_GH_pt_abseta_ratio->Scale(lumi_GH);

      /////////////////                Add Histogramm Run BCDEF + GH                   /////////////////////////////////////
      h_MU24_BCDEF_abseta_pt_ratio->Add(h_MU24_GH_abseta_pt_ratio);
      h_MU24_BCDEF_pt_abseta_ratio->Add(h_MU24_GH_pt_abseta_ratio);

      /////////////////                Save new histogramms as new histogramms                  /////////////////////////////////////
      new_MU24_abseta_pt_ratio = (TH2F*)h_MU24_BCDEF_abseta_pt_ratio;
      new_MU24_pt_abseta_ratio = (TH2F*)h_MU24_BCDEF_pt_abseta_ratio;

      /////////////////                Scale new Histogramms to total lumi                 /////////////////////////////////////
      new_MU24_abseta_pt_ratio->Scale(1/lumi_tot);
      new_MU24_pt_abseta_ratio->Scale(1/lumi_tot);

      /////////////////                 Save new histogramms in new file                ////////////////////////////////////
      dir2 = trigger_allRuns->mkdir("MC_NUM_"+ID[i]+"_DEN_genTracks_PAR_pt_eta/abseta_pt_ratio");
      dir2->cd();
      new_MU24_abseta_pt_ratio ->Write();
      new_MU24_pt_abseta_ratio->Write();
  }//for-loop i

  trigger_allRuns->Close();
}
