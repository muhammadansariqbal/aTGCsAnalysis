#!/usr/bin/env python

"""Add together TEfficiencies according to their cross-sections"""

import ROOT
ROOT.PyConfig.IgnoreCommandLineOptions = True


wjets = [
    ("eff_WJets_HT-100To200", lambda chan: 1345.0*1.21),
    ("eff_WJets_HT-200To400", lambda chan: 359.7*1.21),
    ("eff_WJets_HT-400To600", lambda chan: 48.91*1.21),
    ("eff_WJets_HT-600To800", lambda chan: 12.05*1.21),
    ("eff_WJets_HT-800To1200", lambda chan: 5.501*1.21),
    ("eff_WJets_HT-1200To2500", lambda chan: 1.329*1.21),
    ("eff_WJets_HT-2500ToInf", lambda chan: 0.03216*1.21),
]

single_top = [
    ("eff_SingleTop-tW-channel-top", lambda chan: 35.6),
    ("eff_SingleTop-tW-channel-antitop", lambda chan: 35.6),
    ("eff_SingleTop-t-channel-top", lambda chan: 136.02),
    ("eff_SingleTop-t-channel-antitop", lambda chan: 80.95),
    ("eff_SingleTop-s-channel", lambda chan: 10.32*0.33),
]

signal_ww = [
    ("eff_WW-signal_MWW-600To800", lambda chan: 0.1833 * 2.544 if chan == "ele" else 0.1833 * 2.759),
    ("eff_WW-signal_MWW-800ToInf", lambda chan: 0.2366 * 1.375 if chan == "ele" else 0.2366 * 1.535),
]

signal_wz = [
    ("eff_WZ-signal_MWZ-600To800", lambda chan: 0.06493 * 7.160 if chan == "ele" else 0.06493 * 8.732),
    ("eff_WZ-signal_MWZ-800ToInf", lambda chan: 0.1012 * 3.819 if chan == "ele" else 0.1012 * 4.341),
]

channels = ("ele", "mu")

all_procs = [
    (wjets, "WJets"),
    (single_top, "SingleTop"),
    (signal_ww, "WW-signal"),
    (signal_wz, "WZ-signal"),
]

for proc_list, proc_name in all_procs:

    for chan in channels:
        total_xsec = sum([x[1](chan) for x in proc_list])

        sum_eff_b = None
        sum_eff_c = None
        sum_eff_udsg = None

        for fstem, xsec in proc_list:
            f = ROOT.TFile(fstem+"_"+chan+".root")
            eff_b = f.Get("BtagAnalyzer/BTaggingEff_b")
            eff_c = f.Get("BtagAnalyzer/BTaggingEff_c")
            eff_udsg = f.Get("BtagAnalyzer/BTaggingEff_udsg")

            eff_b.SetWeight(xsec(chan)/total_xsec)
            eff_c.SetWeight(xsec(chan)/total_xsec)
            eff_udsg.SetWeight(xsec(chan)/total_xsec)

            if sum_eff_b is None:
                sum_eff_b = eff_b.Clone()
            else:
                sum_eff_b += eff_b

            if sum_eff_c is None:
                sum_eff_c = eff_c.Clone()
            else:
                sum_eff_c += eff_c

            if sum_eff_udsg is None:
                sum_eff_udsg = eff_udsg.Clone()
            else:
                sum_eff_udsg += eff_udsg

        outf = ROOT.TFile("sum_"+proc_name+"_"+chan+".root", "RECREATE")
        bdir = outf.mkdir("BtagAnalyzer")
        bdir.cd()
        sum_eff_b.Write()
        sum_eff_c.Write()
        sum_eff_udsg.Write()

