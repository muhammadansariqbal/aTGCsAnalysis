#include "TH1.h"
#include "TH2.h"
#include "vector"

using namespace std;

void weight_hist_isolation(){

  //read in .root files to weight
  TFile * trigger_BCDEF = new TFile("Muon_IsoSF_RunBtoF.root");
  TFile * trigger_GH = new TFile("Muon_IsoSF_RunGtoH.root");

  double lumi_BCDEF = 19.695456271;
  double lumi_GH = 16.226452636;
  double lumi_tot = lumi_BCDEF + lumi_GH;
  TFile * trigger_allRuns = new TFile("Muon_IsoSF_average_RunBtoH.root", "RECREATE");

  std::vector<TString> isolation={"LooseISO_","tkLooseISO_","TightISO_"};
  std::vector<TString> ID={"LooseID","MediumID","TightID", "highptID"};
  std::vector<TString> var={"_eta/eta_ratio","_pt/pt_ratio","_vtx/tag_nVertices_ratio"};

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

  for(int k=0;k<isolation.size();k++){

    if(isolation[k]=="LooseISO_"){ID={"LooseID","MediumID","TightID"};}
    if(isolation[k]=="tkLooseISO_"){ID={"highptID"};var={"_eta/eta_ratio","_newpt/pair_ne_ratio","_vtx/tag_nVertices_ratio" };}
    if(isolation[k]=="TightISO_"){ID={"MediumID","TightID"};var={"_eta/eta_ratio","_pt/pt_ratio","_vtx/tag_nVertices_ratio"};}

    for(int i=0;i<ID.size();i++){
      for(int j=0;j<var.size();j++){

        //read in hists to weight
        hist= isolation[k];
        hist+=ID[i];
        hist+=var[j];

        // Run BCDEF & GH
        h_MU24_BCDEF_eta_ratio = (TH1F*)trigger_BCDEF->Get(hist);
        h_MU24_GH_eta_ratio = (TH1F*)trigger_GH->Get(hist);
        h_MU24_BCDEF_eta_ratio->Sumw2();
        h_MU24_GH_eta_ratio->Sumw2();

        //Scale
        h_MU24_BCDEF_eta_ratio->Scale(lumi_BCDEF);
        h_MU24_GH_eta_ratio->Scale(lumi_GH);

        // Add Histogramm Run BCDEF + GH
        h_MU24_BCDEF_eta_ratio->Add(h_MU24_GH_eta_ratio);
        // Save new histogramms as new histogramms
        new_MU24_eta_ratio = (TH1F*)h_MU24_BCDEF_eta_ratio;
        new_MU24_eta_ratio->Sumw2();
        // Save new histogramms as new histogramms
        new_MU24_eta_ratio  ->Scale(1/lumi_tot);


        dir1 =trigger_allRuns ->mkdir(isolation[k]+ID[i]+var[j]);
        dir1->cd();
        new_MU24_eta_ratio->Write();

      }//for-loop j

      if(isolation[k]=="tkLooseISO_"){
        // Run BCDEF
        h_MU24_BCDEF_abseta_pt_ratio = (TH2F*)trigger_BCDEF->Get(isolation[k]+ID[i]+"_newpt_eta/abseta_pair_ne_ratio");
        h_MU24_BCDEF_pt_abseta_ratio = (TH2F*)trigger_BCDEF->Get(isolation[k]+ID[i]+"_newpt_eta/pair_ne_ratio");

        // Run GH
        h_MU24_GH_abseta_pt_ratio = (TH2F*)trigger_GH->Get(isolation[k]+ID[i]+"_newpt_eta/abseta_pair_ne_ratio");
        h_MU24_GH_pt_abseta_ratio = (TH2F*)trigger_GH->Get(isolation[k]+ID[i]+"_newpt_eta/pair_ne_ratio");
      }
      else{
        // Run BCDEF
        h_MU24_BCDEF_abseta_pt_ratio = (TH2F*)trigger_BCDEF->Get(isolation[k]+ID[i]+"_pt_eta/abseta_pt_ratio");
        h_MU24_BCDEF_pt_abseta_ratio = (TH2F*)trigger_BCDEF->Get(isolation[k]+ID[i]+"_pt_eta/pt_abseta_ratio");

        // Run GH
        h_MU24_GH_abseta_pt_ratio = (TH2F*)trigger_GH->Get(isolation[k]+ID[i]+"_pt_eta/abseta_pt_ratio");
        h_MU24_GH_pt_abseta_ratio = (TH2F*)trigger_GH->Get(isolation[k]+ID[i]+"_pt_eta/pt_abseta_ratio");
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
      new_MU24_abseta_pt_ratio ->Sumw2();
      new_MU24_abseta_pt_ratio ->Scale(1/lumi_tot);
      new_MU24_pt_abseta_ratio->Sumw2();
      new_MU24_pt_abseta_ratio->Scale(1/lumi_tot);

      /////////////////                 Save new histogramms in new file                ////////////////////////////////////
      dir2 =trigger_allRuns ->mkdir(isolation[k]+ID[i]+"_pt_eta");
      dir2->cd();
      new_MU24_abseta_pt_ratio ->Write();
      new_MU24_pt_abseta_ratio->Write();
    }//for-loop i
  }//for-loop k

  trigger_allRuns->Close();
}
