#!/bin/bash
# This is a script to add extended statistics
# NB it's OK to hadd TEfficiencies - it's the same as TEfficiency::Add()
# and we're not compensating for diff efficiencies etc

# Make sure it ends with a slash
sourceDir=/afs/cern.ch/work/m/maiqbal/private/aTGC/Samples_80X_BtagEffs/

cpCmd=cp

echo "Processing Muon Channel"
echo "ttbar";$cpCmd $sourceDir"ttbar_mu.root" "eff_ttbar_mu.root"
#echo "WJets_HT-100To200";hadd "eff_WJets_HT-100To200_mu.root" $sourceDir"WJets_HT-100To200_mu.root" $sourceDir"WJets_HT-100To200-ext1_mu.root" $sourceDir"WJets_HT-100To200-ext2_mu.root"
#echo "WJets_HT-200To400";hadd "eff_WJets_HT-200To400_mu.root" $sourceDir"WJets_HT-200To400_mu.root" $sourceDir"WJets_HT-200To400-ext1_mu.root" $sourceDir"WJets_HT-200To400-ext2_mu.root"
#echo "WJets_HT-400To600";hadd "eff_WJets_HT-400To600_mu.root" $sourceDir"WJets_HT-400To600_mu.root" $sourceDir"WJets_HT-400To600-ext_mu.root"
#echo "WJets_HT-600To800";hadd "eff_WJets_HT-600To800_mu.root" $sourceDir"WJets_HT-600To800_mu.root" $sourceDir"WJets_HT-600To800-ext_mu.root"
#echo "WJets_HT-800To1200";hadd "eff_WJets_HT-800To1200_mu.root" $sourceDir"WJets_HT-800To1200_mu.root" $sourceDir"WJets_HT-800To1200-ext_mu.root"
#echo "WJets_HT-1200To2500";hadd "eff_WJets_HT-1200To2500_mu.root" $sourceDir"WJets_HT-1200To2500_mu.root" $sourceDir"WJets_HT-1200To2500-ext_mu.root"
#echo "WJets_HT-2500ToInf";hadd "eff_WJets_HT-2500ToInf_mu.root" $sourceDir"WJets_HT-2500ToInf_mu.root" $sourceDir"WJets_HT-2500ToInf-ext_mu.root"
echo "WJets_Pt-100To250";hadd "eff_WJets_Pt-100To250_mu.root" $sourceDir"WJets_Pt-100To250_mu.root" $sourceDir"WJets_Pt-100To250-ext1_mu.root" $sourceDir"WJets_Pt-100To250-ext4_mu.root"
echo "WJets_Pt-250To400";hadd "eff_WJets_Pt-250To400_mu.root" $sourceDir"WJets_Pt-250To400_mu.root" $sourceDir"WJets_Pt-250To400-ext1_mu.root" $sourceDir"WJets_Pt-250To400-ext4_mu.root"
echo "WJets_Pt-400To600";hadd "eff_WJets_Pt-400To600_mu.root" $sourceDir"WJets_Pt-400To600_mu.root" $sourceDir"WJets_Pt-400To600-ext1_mu.root"
echo "WJets_Pt-600ToInf";hadd "eff_WJets_Pt-600ToInf_mu.root" $sourceDir"WJets_Pt-600ToInf_mu.root" $sourceDir"WJets_Pt-600ToInf-ext1_mu.root"
echo "WW";hadd "eff_WW_mu.root" $sourceDir"WW_mu.root" $sourceDir"WW-ext_mu.root"
echo "WZ";$cpCmd $sourceDir"WZ_mu.root" "eff_WZ_mu.root"
echo "t-top"; $cpCmd $sourceDir"SingleTop-t-channel-top_mu.root" "eff_SingleTop-t-channel-top_mu.root"
echo "t-antitop"; $cpCmd $sourceDir"SingleTop-t-channel-antitop_mu.root" "eff_SingleTop-t-channel-antitop_mu.root"
echo "s-channel";$cpCmd $sourceDir"SingleTop-s-channel_mu.root" "eff_SingleTop-s-channel_mu.root"
echo "tw-top";$cpCmd $sourceDir"SingleTop-tW-channel-top_mu.root" "eff_SingleTop-tW-channel-top_mu.root"
echo "tw-antitop";$cpCmd $sourceDir"SingleTop-tW-channel-antitop_mu.root" "eff_SingleTop-tW-channel-antitop_mu.root"
echo "WW-signal_MWW-600To800";$cpCmd $sourceDir"WW-signal_MWW-600To800_mu.root" "eff_WW-signal_MWW-600To800_mu.root"
echo "WW-signal_MWW-800ToInf";$cpCmd $sourceDir"WW-signal_MWW-800ToInf_mu.root" "eff_WW-signal_MWW-800ToInf_mu.root"
echo "WZ-signal_MWZ-600To800";$cpCmd $sourceDir"WZ-signal_MWZ-600To800_mu.root" "eff_WZ-signal_MWZ-600To800_mu.root"
echo "WZ-signal_MWZ-800ToInf";$cpCmd $sourceDir"WZ-signal_MWZ-800ToInf_mu.root" "eff_WZ-signal_MWZ-800ToInf_mu.root"

echo "Processing Electron Channel"
echo "ttbar";$cpCmd $sourceDir"ttbar_ele.root" "eff_ttbar_ele.root"
#echo "WJets_HT-100To200";hadd "eff_WJets_HT-100To200_ele.root" $sourceDir"WJets_HT-100To200_ele.root" $sourceDir"WJets_HT-100To200-ext1_ele.root" $sourceDir"WJets_HT-100To200-ext2_ele.root"
#echo "WJets_HT-200To400";hadd "eff_WJets_HT-200To400_ele.root" $sourceDir"WJets_HT-200To400_ele.root" $sourceDir"WJets_HT-200To400-ext1_ele.root" $sourceDir"WJets_HT-200To400-ext2_ele.root"
#echo "WJets_HT-400To600";hadd "eff_WJets_HT-400To600_ele.root" $sourceDir"WJets_HT-400To600_ele.root" $sourceDir"WJets_HT-400To600-ext_ele.root"
#echo "WJets_HT-600To800";hadd "eff_WJets_HT-600To800_ele.root" $sourceDir"WJets_HT-600To800_ele.root" $sourceDir"WJets_HT-600To800-ext_ele.root"
#echo "WJets_HT-800To1200";hadd "eff_WJets_HT-800To1200_ele.root" $sourceDir"WJets_HT-800To1200_ele.root" $sourceDir"WJets_HT-800To1200-ext_ele.root"
#echo "WJets_HT-1200To2500";hadd "eff_WJets_HT-1200To2500_ele.root" $sourceDir"WJets_HT-1200To2500_ele.root" $sourceDir"WJets_HT-1200To2500-ext_ele.root"
#echo "WJets_HT-2500ToInf";hadd "eff_WJets_HT-2500ToInf_ele.root" $sourceDir"WJets_HT-2500ToInf_ele.root" $sourceDir"WJets_HT-2500ToInf-ext_ele.root"
echo "WJets_Pt-100To250";hadd "eff_WJets_Pt-100To250_ele.root" $sourceDir"WJets_Pt-100To250_ele.root" $sourceDir"WJets_Pt-100To250-ext1_ele.root" $sourceDir"WJets_Pt-100To250-ext4_ele.root"
echo "WJets_Pt-250To400";hadd "eff_WJets_Pt-250To400_ele.root" $sourceDir"WJets_Pt-250To400_ele.root" $sourceDir"WJets_Pt-250To400-ext1_ele.root" $sourceDir"WJets_Pt-250To400-ext4_ele.root"
echo "WJets_Pt-400To600";hadd "eff_WJets_Pt-400To600_ele.root" $sourceDir"WJets_Pt-400To600_ele.root" $sourceDir"WJets_Pt-400To600-ext1_ele.root"
echo "WJets_Pt-600ToInf";hadd "eff_WJets_Pt-600ToInf_ele.root" $sourceDir"WJets_Pt-600ToInf_ele.root" $sourceDir"WJets_Pt-600ToInf-ext1_ele.root"
echo "WW";hadd "eff_WW_ele.root" $sourceDir"WW_ele.root" $sourceDir"WW-ext_ele.root"
echo "WZ";$cpCmd $sourceDir"WZ_ele.root" "eff_WZ_ele.root"
echo "t-top";$cpCmd $sourceDir"SingleTop-t-channel-top_ele.root" "eff_SingleTop-t-channel-top_ele.root"
echo "t-antitop";$cpCmd $sourceDir"SingleTop-t-channel-antitop_ele.root" "eff_SingleTop-t-channel-antitop_ele.root"
echo "s-channel";$cpCmd $sourceDir"SingleTop-s-channel_ele.root" "eff_SingleTop-s-channel_ele.root"
echo "tw-top";$cpCmd $sourceDir"SingleTop-tW-channel-top_ele.root" "eff_SingleTop-tW-channel-top_ele.root"
echo "tw-antitop";$cpCmd $sourceDir"SingleTop-tW-channel-antitop_ele.root" "eff_SingleTop-tW-channel-antitop_ele.root"
echo "WW-signal_MWW-600To800";$cpCmd $sourceDir"WW-signal_MWW-600To800_ele.root" "eff_WW-signal_MWW-600To800_ele.root"
echo "WW-signal_MWW-800ToInf";$cpCmd $sourceDir"WW-signal_MWW-800ToInf_ele.root" "eff_WW-signal_MWW-800ToInf_ele.root"
echo "WZ-signal_MWZ-600To800";$cpCmd $sourceDir"WZ-signal_MWZ-600To800_ele.root" "eff_WZ-signal_MWZ-600To800_ele.root"
echo "WZ-signal_MWZ-800ToInf";$cpCmd $sourceDir"WZ-signal_MWZ-800ToInf_ele.root" "eff_WZ-signal_MWZ-800ToInf_ele.root"
