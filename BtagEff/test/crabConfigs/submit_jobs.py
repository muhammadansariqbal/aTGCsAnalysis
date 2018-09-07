#!/usr/bin/env python

"""
Run this script to create crab configs for btagging efficiency files,
and optionally submit them.
"""

import os
from subprocess import check_call
from itertools import product

FEATURE_NAME = "btag_eff_withLeptonSF_withCuts"

# set True to actually submit jobs to crab
SUBMIT_JOBS = True


def create_crab_config(request_name, cmssw_config_file, dataset, units_per_job, output_filename):
    with open("crab_template.py") as in_f:
        crab_template = in_f.read()

    fmt_dict = {
        "REQUEST_NAME": request_name,
        "PSET_NAME": cmssw_config_file,
        "DATASET": dataset,
        "UNITS_PER_JOB": str(units_per_job)
    }
    crab_template = crab_template.format(**fmt_dict)

    if "{" in crab_template or "}" in crab_template:
        raise RuntimeError("Failed to create crab config contents:\n %s" % crab_template)

    output_dir = os.path.dirname(os.path.abspath(output_filename))
    if not os.path.isdir(output_dir):
        os.makedirs(output_dir)

    with open(output_filename, "w") as out_f:
        out_f.write(crab_template)


def submit_job(crab_config):
    cmd = "crab submit -c %s" % crab_config
    check_call(cmd, shell=True)


# Datasets you want to run over. Can optionally specify units_per_job for each
samples = [
    {"name": "ttbar", "dataset": "/TT_TuneCUETP8M2T4_13TeV-powheg-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/MINIAODSIM"},

    {"name": "WW", "dataset": "/WWToLNuQQ_13TeV-powheg/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/MINIAODSIM"},
    {"name": "WW-ext", "dataset": "/WWToLNuQQ_13TeV-powheg/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1/MINIAODSIM"},

    {"name": "WZ", "dataset": "/WZTo1L1Nu2Q_13TeV_amcatnloFXFX_madspin_pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v3/MINIAODSIM"},

    {"name": 'SingleTop-s-channel', "dataset": '/ST_s-channel_4f_leptonDecays_13TeV-amcatnlo-pythia8_TuneCUETP8M1/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/MINIAODSIM'},
    {"name": 'SingleTop-tW-channel-antitop', "dataset": '/ST_tW_antitop_5f_inclusiveDecays_13TeV-powheg-pythia8_TuneCUETP8M1/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1/MINIAODSIM'},
    {"name": 'SingleTop-tW-channel-top', "dataset": '/ST_tW_top_5f_inclusiveDecays_13TeV-powheg-pythia8_TuneCUETP8M1/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1/MINIAODSIM'},
    {"name": 'SingleTop-t-channel-top', "dataset": '/ST_t-channel_top_4f_inclusiveDecays_13TeV-powhegV2-madspin-pythia8_TuneCUETP8M1/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/MINIAODSIM'},
    {"name": 'SingleTop-t-channel-antitop', "dataset": '/ST_t-channel_antitop_4f_inclusiveDecays_13TeV-powhegV2-madspin-pythia8_TuneCUETP8M1/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/MINIAODSIM'},

    {"name": 'WW-signal_MWW-600To800', "dataset": '/WWToLNuQQ_MWW-600To800_PtW-180_aTGC_ShowerReconfig_13TeV-amcatnloFXFX-madspin-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v2/MINIAODSIM'},
    {"name": 'WW-signal_MWW-800ToInf', "dataset": '/WWToLNuQQ_MWW-800_PtW-180_aTGC_ShowerReconfig_13TeV-amcatnloFXFX-madspin-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/MINIAODSIM'},
    {"name": 'WZ-signal_MWZ-600To800', "dataset": '/WZToLNuQQ_MWZ-600To800_PtZ-180_aTGC_ShowerReconfig_13TeV-amcatnloFXFX-madspin-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/MINIAODSIM'},
    {"name": 'WZ-signal_MWZ-800ToInf', "dataset": '/WZToLNuQQ_MWZ-800_PtZ-180_aTGC_ShowerReconfig_13TeV-amcatnloFXFX-madspin-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/MINIAODSIM'},

#    {"name": 'WJets_HT-100To200', "dataset": '/WJetsToLNu_HT-100To200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/MINIAODSIM'},
#    {"name": 'WJets_HT-100To200-ext1', "dataset": '/WJetsToLNu_HT-100To200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1/MINIAODSIM'},
#    {"name": 'WJets_HT-100To200-ext2', "dataset": '/WJetsToLNu_HT-100To200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext2-v1/MINIAODSIM'},

#    {"name": 'WJets_HT-200To400', "dataset": '/WJetsToLNu_HT-200To400_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/MINIAODSIM'},
#    {"name": 'WJets_HT-200To400-ext1', "dataset": '/WJetsToLNu_HT-200To400_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1/MINIAODSIM'},
#    {"name": 'WJets_HT-200To400-ext2', "dataset": '/WJetsToLNu_HT-200To400_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext2-v1/MINIAODSIM'},

#    {"name": 'WJets_HT-400To600', "dataset": '/WJetsToLNu_HT-400To600_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/MINIAODSIM'},
#    {"name": 'WJets_HT-400To600-ext', "dataset": '/WJetsToLNu_HT-400To600_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1/MINIAODSIM'},

#    {"name": 'WJets_HT-600To800', "dataset": '/WJetsToLNu_HT-600To800_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/MINIAODSIM'},
#    {"name": 'WJets_HT-600To800-ext', "dataset": '/WJetsToLNu_HT-600To800_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1/MINIAODSIM'},

#    {"name": 'WJets_HT-800To1200', "dataset": '/WJetsToLNu_HT-800To1200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/MINIAODSIM'},
#    {"name": 'WJets_HT-800To1200-ext', "dataset": '/WJetsToLNu_HT-800To1200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1/MINIAODSIM'},

#    {"name": 'WJets_HT-1200To2500', "dataset": '/WJetsToLNu_HT-1200To2500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/MINIAODSIM'},
#    {"name": 'WJets_HT-1200To2500-ext', "dataset": '/WJetsToLNu_HT-1200To2500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1/MINIAODSIM'},

#    {"name": 'WJets_HT-2500ToInf', "dataset": '/WJetsToLNu_HT-2500ToInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/MINIAODSIM'},
#    {"name": 'WJets_HT-2500ToInf-ext', "dataset": '/WJetsToLNu_HT-2500ToInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1/MINIAODSIM'},

    {"name": 'WJets_Pt-100To250', "dataset": '/WJetsToLNu_Pt-100To250_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/MINIAODSIM'},
    {"name": 'WJets_Pt-100To250-ext1', "dataset": '/WJetsToLNu_Pt-100To250_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1/MINIAODSIM'},
    {"name": 'WJets_Pt-100To250-ext4', "dataset": '/WJetsToLNu_Pt-100To250_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext4-v1/MINIAODSIM'},

    {"name": 'WJets_Pt-250To400', "dataset": '/WJetsToLNu_Pt-250To400_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/MINIAODSIM'},
    {"name": 'WJets_Pt-250To400-ext1', "dataset": '/WJetsToLNu_Pt-250To400_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1/MINIAODSIM'},
    {"name": 'WJets_Pt-250To400-ext4', "dataset": '/WJetsToLNu_Pt-250To400_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext4-v1/MINIAODSIM'},

    {"name": 'WJets_Pt-400To600', "dataset": '/WJetsToLNu_Pt-400To600_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/MINIAODSIM'},
    {"name": 'WJets_Pt-400To600-ext1', "dataset": '/WJetsToLNu_Pt-400To600_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1/MINIAODSIM'},

    {"name": 'WJets_Pt-600ToInf', "dataset": '/WJetsToLNu_Pt-600ToInf_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/MINIAODSIM'},
    {"name": 'WJets_Pt-600ToInf-ext1', "dataset": '/WJetsToLNu_Pt-600ToInf_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1/MINIAODSIM'},

]


if __name__ == "__main__":
    # Channels to run over, and corresponding config file
    channels = [
        {"name": "mu", "config": "../btageff_mu.py"},
        {"name": "ele", "config": "../btageff_ele.py"}
    ]

    default_files_per_job = 2

    for chan_dict, sample_dict in product(channels, samples):
        output_filename = FEATURE_NAME + "/crab-%s-%s.py" % (sample_dict['name'], chan_dict['name'])
        create_crab_config(request_name="%s_%s_%s" % (sample_dict['name'], chan_dict['name'], FEATURE_NAME),
                           cmssw_config_file=chan_dict['config'],
                           dataset=sample_dict['dataset'],
                           units_per_job=sample_dict.get("units_per_job", default_files_per_job),
                           output_filename=output_filename)
        if SUBMIT_JOBS:
            submit_job(output_filename)
            print "\033[0;40;32mtask:", sample_dict['name'] + "-" + chan_dict['name'], "was submitted!\033[0m"

