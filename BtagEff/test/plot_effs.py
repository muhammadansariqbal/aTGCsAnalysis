#!/usr/bin/env python


"""Quickly draw TEfficiencies from all ROOT files in given dirs.

Makes b/c/udsg plots in same location as ROOT files.

Usage:

    ./plot_effs.py <dirname1> <dirname2> ...

"""

from __future__ import print_function
import ROOT
import sys
import os

ROOT.PyConfig.IgnoreCommandLineOptions = True
ROOT.gROOT.SetBatch(1)
# ROOT.TH1.SetDefaultSumw2()  # ! DO NOT USE THIS, REMOVES ERRORS ON PLOTS
ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetPaintTextFormat(".3g")


def make_eff_plots(filename):
    print("Doing", filename)
    f = ROOT.TFile(filename)
    eff_b = f.Get("BtagAnalyzer/BTaggingEff_b")
    eff_c = f.Get("BtagAnalyzer/BTaggingEff_c")
    eff_udsg = f.Get("BtagAnalyzer/BTaggingEff_udsg")

    stem = os.path.splitext(os.path.abspath(filename))[0]
    name = os.path.basename(filename)
    name = name.replace("eff_", "").replace("sum_", "")
    name = name.replace("ttbar", "t#bar{t}").replace("WJets", "W+jets").replace("SingleTop", "t")
    name = name.replace(".root", "")
    name = name.replace("_udsg", "").replace("_c", "").replace("_b", "")
    name = name.replace("_mu", " (#mu channel)").replace("_ele", " (e channel)")
    name = name.replace("-", " ")

    c = ROOT.TCanvas("c", "", 800, 600)
    eff_b.SetTitle(name +", b efficiency;p_{T} (GeV);#eta")
    eff_b.Draw("COLZ TEXT89E")
    c.SetLogx()
    c.SaveAs(stem+"_eff_b.pdf")

    eff_c.SetTitle(name +", c efficiency;p_{T} (GeV);#eta")
    eff_c.Draw("COLZ TEXT89E")
    c.SaveAs(stem+"_eff_c.pdf")

    eff_udsg.SetTitle(name + ", udsg efficiency;p_{T} (GeV);#eta")
    eff_udsg.Draw("COLZ TEXT89E")
    c.SaveAs(stem+"_eff_udsg.pdf")
    f.Close()


if __name__ == "__main__":
    for dirname in sys.argv[1:]:
        root_files = [x for x in os.listdir(dirname) if x.endswith(".root") and
                        os.path.basename(x).startswith(("sum", "eff"))]
        for fname in root_files:
            make_eff_plots(os.path.join(dirname, fname))
