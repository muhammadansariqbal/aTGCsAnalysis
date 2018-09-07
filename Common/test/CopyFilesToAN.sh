#!/bin/bash
# This is a script to copy plots to AN. Run from the plots directory in AN.

# DataMC Comparisons
# MWV and MJ
cp ~/private/aTGC/CMSSW_8_0_28/src/aTGCsAnalysis/Common/test/Plotting/plots_25ns_ele_WJets_CR/pdf/MWW_SD.pdf dataMC/dataMC_WJets_ele_MWV.pdf
cp ~/private/aTGC/CMSSW_8_0_28/src/aTGCsAnalysis/Common/test/Plotting/plots_25ns_mu_WJets_CR/pdf/MWW_SD.pdf dataMC/dataMC_WJets_mu_MWV.pdf
cp ~/private/aTGC/CMSSW_8_0_28/src/aTGCsAnalysis/Common/test/Plotting/plots_25ns_ele_WJets_CR/pdf/jet_mass_softdrop_PUPPI.pdf dataMC/dataMC_WJets_ele_MJ.pdf
cp ~/private/aTGC/CMSSW_8_0_28/src/aTGCsAnalysis/Common/test/Plotting/plots_25ns_mu_WJets_CR/pdf/jet_mass_softdrop_PUPPI.pdf ./dataMC_WJets_mu_MJ.pdf
cp ~/private/aTGC/CMSSW_8_0_28/src/aTGCsAnalysis/Common/test/Plotting/plots_25ns_ele_ttbar_CR/pdf/MWW_SD.pdf dataMC/dataMC_ttbar_ele_MWV.pdf
cp ~/private/aTGC/CMSSW_8_0_28/src/aTGCsAnalysis/Common/test/Plotting/plots_25ns_mu_ttbar_CR/pdf/MWW_SD.pdf dataMC/dataMC_ttbar_mu_MWV.pdf
cp ~/private/aTGC/CMSSW_8_0_28/src/aTGCsAnalysis/Common/test/Plotting/plots_25ns_ele_ttbar_CR/pdf/jet_mass_softdrop_PUPPI.pdf dataMC/dataMC_ttbar_ele_MJ.pdf
cp ~/private/aTGC/CMSSW_8_0_28/src/aTGCsAnalysis/Common/test/Plotting/plots_25ns_mu_ttbar_CR/pdf/jet_mass_softdrop_PUPPI.pdf dataMC/dataMC_ttbar_mu_MJ.pdf
# pTLep and MET
cp ~/private/aTGC/CMSSW_8_0_28/src/aTGCsAnalysis/Common/test/Plotting/plots_25ns_ele_WJets_CR/pdf/l_pt.pdf dataMC/dataMC_WJets_ele_pTLep.pdf
cp ~/private/aTGC/CMSSW_8_0_28/src/aTGCsAnalysis/Common/test/Plotting/plots_25ns_mu_WJets_CR/pdf/l_pt.pdf dataMC/dataMC_WJets_mu_pTLep.pdf
cp ~/private/aTGC/CMSSW_8_0_28/src/aTGCsAnalysis/Common/test/Plotting/plots_25ns_ele_WJets_CR/pdf/pfMET.pdf dataMC/dataMC_WJets_ele_pfMET.pdf
cp ~/private/aTGC/CMSSW_8_0_28/src/aTGCsAnalysis/Common/test/Plotting/plots_25ns_mu_WJets_CR/pdf/pfMET.pdf dataMC/dataMC_WJets_mu_pfMET.pdf
cp ~/private/aTGC/CMSSW_8_0_28/src/aTGCsAnalysis/Common/test/Plotting/plots_25ns_ele_ttbar_CR/pdf/l_pt.pdf dataMC/dataMC_ttbar_ele_pTLep.pdf
cp ~/private/aTGC/CMSSW_8_0_28/src/aTGCsAnalysis/Common/test/Plotting/plots_25ns_mu_ttbar_CR/pdf/l_pt.pdf dataMC/dataMC_ttbar_mu_pTLep.pdf
cp ~/private/aTGC/CMSSW_8_0_28/src/aTGCsAnalysis/Common/test/Plotting/plots_25ns_ele_ttbar_CR/pdf/pfMET.pdf dataMC/dataMC_ttbar_ele_pfMET.pdf
cp ~/private/aTGC/CMSSW_8_0_28/src/aTGCsAnalysis/Common/test/Plotting/plots_25ns_mu_ttbar_CR/pdf/pfMET.pdf dataMC/dataMC_ttbar_mu_pfMET.pdf
# pTLepW and pTHadW
cp ~/private/aTGC/CMSSW_8_0_28/src/aTGCsAnalysis/Common/test/Plotting/plots_25ns_ele_WJets_CR/pdf/W_pt.pdf dataMC/dataMC_WJets_ele_pTLepW.pdf
cp ~/private/aTGC/CMSSW_8_0_28/src/aTGCsAnalysis/Common/test/Plotting/plots_25ns_mu_WJets_CR/pdf/W_pt.pdf dataMC/dataMC_WJets_mu_pTLepW.pdf
cp ~/private/aTGC/CMSSW_8_0_28/src/aTGCsAnalysis/Common/test/Plotting/plots_25ns_ele_WJets_CR/pdf/jet_pt.pdf dataMC/dataMC_WJets_ele_pTJ.pdf
cp ~/private/aTGC/CMSSW_8_0_28/src/aTGCsAnalysis/Common/test/Plotting/plots_25ns_mu_WJets_CR/pdf/jet_pt.pdf dataMC/dataMC_WJets_mu_pTJ.pdf
cp ~/private/aTGC/CMSSW_8_0_28/src/aTGCsAnalysis/Common/test/Plotting/plots_25ns_ele_ttbar_CR/pdf/W_pt.pdf dataMC/dataMC_ttbar_ele_pTLepW.pdf
cp ~/private/aTGC/CMSSW_8_0_28/src/aTGCsAnalysis/Common/test/Plotting/plots_25ns_mu_ttbar_CR/pdf/W_pt.pdf dataMC/dataMC_ttbar_mu_pTLepW.pdf
cp ~/private/aTGC/CMSSW_8_0_28/src/aTGCsAnalysis/Common/test/Plotting/plots_25ns_ele_ttbar_CR/pdf/jet_pt.pdf dataMC/dataMC_ttbar_ele_pTJ.pdf
cp ~/private/aTGC/CMSSW_8_0_28/src/aTGCsAnalysis/Common/test/Plotting/plots_25ns_mu_ttbar_CR/pdf/jet_pt.pdf dataMC/dataMC_ttbar_mu_pTJ.pdf
# MTW and Tau21
cp ~/private/aTGC/CMSSW_8_0_28/src/aTGCsAnalysis/Common/test/Plotting/plots_25ns_ele_WJets_CR/pdf/W_mt.pdf dataMC/dataMC_WJets_ele_MTW.pdf
cp ~/private/aTGC/CMSSW_8_0_28/src/aTGCsAnalysis/Common/test/Plotting/plots_25ns_mu_WJets_CR/pdf/W_mt.pdf dataMC/dataMC_WJets_mu_MTW.pdf
cp ~/private/aTGC/CMSSW_8_0_28/src/aTGCsAnalysis/Common/test/Plotting/plots_25ns_ele_WJets_CR/pdf/jet_tau21_PUPPI.pdf dataMC/dataMC_WJets_ele_Tau21.pdf
cp ~/private/aTGC/CMSSW_8_0_28/src/aTGCsAnalysis/Common/test/Plotting/plots_25ns_mu_WJets_CR/pdf/jet_tau21_PUPPI.pdf dataMC/dataMC_WJets_mu_Tau21.pdf
cp ~/private/aTGC/CMSSW_8_0_28/src/aTGCsAnalysis/Common/test/Plotting/plots_25ns_ele_ttbar_CR/pdf/W_mt.pdf dataMC/dataMC_ttbar_ele_MTW.pdf
cp ~/private/aTGC/CMSSW_8_0_28/src/aTGCsAnalysis/Common/test/Plotting/plots_25ns_mu_ttbar_CR/pdf/W_mt.pdf dataMC/dataMC_ttbar_mu_MTW.pdf
cp ~/private/aTGC/CMSSW_8_0_28/src/aTGCsAnalysis/Common/test/Plotting/plots_25ns_ele_ttbar_CR/pdf/jet_tau21_PUPPI.pdf dataMC/dataMC_ttbar_ele_Tau21.pdf
cp ~/private/aTGC/CMSSW_8_0_28/src/aTGCsAnalysis/Common/test/Plotting/plots_25ns_mu_ttbar_CR/pdf/jet_tau21_PUPPI.pdf dataMC/dataMC_ttbar_mu_Tau21.pdf

# Background
# MJ
cp ~/private/FittingForATGC/Background/CMSSW_5_3_32/src/FittingForATGCBackground/plots_el_HPV_900_4500/m_j_fitting/_WJets0tree_WJets_el_ChiSqBern_with_pull.pdf fitBG/fitBG_MJ_WJets_ele.pdf
cp ~/private/FittingForATGC/Background/CMSSW_5_3_32/src/FittingForATGCBackground/plots_mu_HPV_900_4500/m_j_fitting/_WJets0tree_WJets_mu_ChiSqBern_with_pull.pdf fitBG/fitBG_MJ_WJets_mu.pdf
cp ~/private/FittingForATGC/Background/CMSSW_5_3_32/src/FittingForATGCBackground/plots_el_HPV_900_4500/m_j_fitting/_TTbartree_TTbar_el_2Gaus_ErfExp_with_pull.pdf fitBG/fitBG_MJ_ttbar_ele.pdf
cp ~/private/FittingForATGC/Background/CMSSW_5_3_32/src/FittingForATGCBackground/plots_mu_HPV_900_4500/m_j_fitting/_TTbartree_TTbar_mu_2Gaus_ErfExp_with_pull.pdf fitBG/fitBG_MJ_ttbar_mu.pdf
cp ~/private/FittingForATGC/Background/CMSSW_5_3_32/src/FittingForATGCBackground/plots_el_HPV_900_4500/m_j_fitting/_SToptree_STop_el_ExpGaus_with_pull.pdf fitBG/fitBG_MJ_STop_ele.pdf
cp ~/private/FittingForATGC/Background/CMSSW_5_3_32/src/FittingForATGCBackground/plots_mu_HPV_900_4500/m_j_fitting/_SToptree_STop_mu_ExpGaus_with_pull.pdf fitBG/fitBG_MJ_Stop_mu.pdf
cp ~/private/FittingForATGC/Background/CMSSW_5_3_32/src/FittingForATGCBackground/plots_el_HPV_900_4500/m_j_fitting/_WWtree_WW_el_2GausWW_with_pull.pdf fitBG/fitBG_MJ_Diboson_ele.pdf
cp ~/private/FittingForATGC/Background/CMSSW_5_3_32/src/FittingForATGCBackground/plots_mu_HPV_900_4500/m_j_fitting/_WWtree_WW_mu_2GausWW_with_pull.pdf fitBG/fitBG_MJ_Diboson_mu.pdf
# MJ pre-fit
cp ~/private/FittingForATGC/Background/CMSSW_5_3_32/src/FittingForATGCBackground/plots_el_HPV_900_4500/m_j_fitting/m_j_prefit__with_pull.pdf fitBG/fitBG_MJ_Prefit_el.pdf
cp ~/private/FittingForATGC/Background/CMSSW_5_3_32/src/FittingForATGCBackground/plots_mu_HPV_900_4500/m_j_fitting/m_j_prefit__with_pull.pdf fitBG/fitBG_MJ_Prefit_mu.pdf
# MWV
# Signal region
cp ~/private/FittingForATGC/Background/CMSSW_5_3_32/src/FittingForATGCBackground/plots_el_HPV_900_4500/m_lvj_fitting/tree_TTbar_el_m_lvj_sigExpTail_with_pull_log.pdf fitBG/fitBG_MWV_sig_ttbar_ele.pdf
cp ~/private/FittingForATGC/Background/CMSSW_5_3_32/src/FittingForATGCBackground/plots_mu_HPV_900_4500/m_lvj_fitting/tree_TTbar_mu_m_lvj_sigExpTail_with_pull_log.pdf fitBG/fitBG_MWV_sig_ttbar_mu.pdf
cp ~/private/FittingForATGC/Background/CMSSW_5_3_32/src/FittingForATGCBackground/plots_el_HPV_900_4500/m_lvj_fitting/tree_STop_el_m_lvj_sigExp_with_pull_log.pdf fitBG/fitBG_MWV_sig_STop_ele.pdf
cp ~/private/FittingForATGC/Background/CMSSW_5_3_32/src/FittingForATGCBackground/plots_mu_HPV_900_4500/m_lvj_fitting/tree_STop_mu_m_lvj_sigExp_with_pull_log.pdf fitBG/fitBG_MWV_sig_STop_mu.pdf
cp ~/private/FittingForATGC/Background/CMSSW_5_3_32/src/FittingForATGCBackground/plots_el_HPV_900_4500/m_lvj_fitting/tree_WW_el_m_lvj_sigExpN_with_pull_log.pdf fitBG/fitBG_MWV_sig_Diboson_ele.pdf
cp ~/private/FittingForATGC/Background/CMSSW_5_3_32/src/FittingForATGCBackground/plots_mu_HPV_900_4500/m_lvj_fitting/tree_WW_mu_m_lvj_sigExpN_with_pull_log.pdf fitBG/fitBG_MWV_sig_Diboson_mu.pdf
# SB region
cp ~/private/FittingForATGC/Background/CMSSW_5_3_32/src/FittingForATGCBackground/plots_el_HPV_900_4500/m_lvj_fitting/tree_TTbar_el_m_lvj_sbExpTail_with_pull_log.pdf fitBG/fitBG_MWV_SB_ttbar_ele.pdf
cp ~/private/FittingForATGC/Background/CMSSW_5_3_32/src/FittingForATGCBackground/plots_mu_HPV_900_4500/m_lvj_fitting/tree_TTbar_mu_m_lvj_sbExpTail_with_pull_log.pdf fitBG/fitBG_MWV_SB_ttbar_mu.pdf
cp ~/private/FittingForATGC/Background/CMSSW_5_3_32/src/FittingForATGCBackground/plots_el_HPV_900_4500/m_lvj_fitting/tree_STop_el_m_lvj_sbExp_with_pull_log.pdf fitBG/fitBG_MWV_SB_STop_ele.pdf
cp ~/private/FittingForATGC/Background/CMSSW_5_3_32/src/FittingForATGCBackground/plots_mu_HPV_900_4500/m_lvj_fitting/tree_STop_mu_m_lvj_sbExp_with_pull_log.pdf fitBG/fitBG_MWV_SB_STop_mu.pdf
cp ~/private/FittingForATGC/Background/CMSSW_5_3_32/src/FittingForATGCBackground/plots_el_HPV_900_4500/m_lvj_fitting/tree_WW_el_m_lvj_sbExp_with_pull_log.pdf fitBG/fitBG_MWV_SB_Diboson_ele.pdf
cp ~/private/FittingForATGC/Background/CMSSW_5_3_32/src/FittingForATGCBackground/plots_mu_HPV_900_4500/m_lvj_fitting/tree_WW_mu_m_lvj_sbExp_with_pull_log.pdf fitBG/fitBG_MWV_SB_Diboson_mu.pdf
# WJets
cp ~/private/FittingForATGC/Background/CMSSW_5_3_32/src/FittingForATGCBackground/plots_el_HPV_900_4500/m_lvj_fitting/tree_WJets_el_m_lvj_sigExpN_with_pull_log.pdf fitBG/fitBG_MWV_sig_WJets_ele.pdf
cp ~/private/FittingForATGC/Background/CMSSW_5_3_32/src/FittingForATGCBackground/plots_mu_HPV_900_4500/m_lvj_fitting/tree_WJets_mu_m_lvj_sigExpN_with_pull_log.pdf fitBG/fitBG_MWV_sig_WJets_mu.pdf
cp ~/private/FittingForATGC/Background/CMSSW_5_3_32/src/FittingForATGCBackground/plots_el_HPV_900_4500/m_lvj_fitting/tree_WJets_el_m_lvj_sbExpN_with_pull_log.pdf fitBG/fitBG_MWV_SB_WJets_ele.pdf
cp ~/private/FittingForATGC/Background/CMSSW_5_3_32/src/FittingForATGCBackground/plots_mu_HPV_900_4500/m_lvj_fitting/tree_WJets_mu_m_lvj_sbExpN_with_pull_log.pdf fitBG/fitBG_MWV_SB_WJets_mu.pdf
# Alpha functions
cp ~/private/FittingForATGC/Background/CMSSW_5_3_32/src/FittingForATGCBackground/plots_el_HPV_900_4500/other/correction_pdf_WJets0_ExpN_M_lvj_sig_to_sideband.pdf fitBG/fitBG_AlphaFunc_ele.pdf
cp ~/private/FittingForATGC/Background/CMSSW_5_3_32/src/FittingForATGCBackground/plots_mu_HPV_900_4500/other/correction_pdf_WJets0_ExpN_M_lvj_sig_to_sideband.pdf fitBG/fitBG_AlphaFunc_mu.pdf
# MWV pre-fit SB
cp ~/private/FittingForATGC/Background/CMSSW_5_3_32/src/FittingForATGCBackground/plots_el_HPV_900_4500/m_lvj_fitting/m_lvj_sb_WJets0__with_pull_log.pdf fitBG/fitBG_MWV_PrefitSB_ele.pdf
cp ~/private/FittingForATGC/Background/CMSSW_5_3_32/src/FittingForATGCBackground/plots_mu_HPV_900_4500/m_lvj_fitting/m_lvj_sb_WJets0__with_pull_log.pdf fitBG/fitBG_MWV_PrefitSB_mu.pdf

# Signal
# Signal aTGC positive values
# WW
cp ~/private/FittingForATGC/Signal/CMSSW_7_1_5/src/FittingForATGCSignal/Plots/cwww_pos_el_WW.pdf fitSignal/fitSignal_WW_ele_cwwwPos.pdf
cp ~/private/FittingForATGC/Signal/CMSSW_7_1_5/src/FittingForATGCSignal/Plots/cwww_pos_mu_WW.pdf fitSignal/fitSignal_WW_mu_cwwwPos.pdf
cp ~/private/FittingForATGC/Signal/CMSSW_7_1_5/src/FittingForATGCSignal/Plots/ccw_pos_mu_WW.pdf fitSignal/fitSignal_WW_mu_ccwPos.pdf
cp ~/private/FittingForATGC/Signal/CMSSW_7_1_5/src/FittingForATGCSignal/Plots/ccw_pos_el_WW.pdf fitSignal/fitSignal_WW_ele_ccwPos.pdf
cp ~/private/FittingForATGC/Signal/CMSSW_7_1_5/src/FittingForATGCSignal/Plots/cb_pos_el_WW.pdf fitSignal/fitSignal_WW_ele_cbPos.pdf
cp ~/private/FittingForATGC/Signal/CMSSW_7_1_5/src/FittingForATGCSignal/Plots/cb_pos_mu_WW.pdf fitSignal/fitSignal_WW_mu_cbPos.pdf
# WZ
cp ~/private/FittingForATGC/Signal/CMSSW_7_1_5/src/FittingForATGCSignal/Plots/cwww_pos_el_WZ.pdf fitSignal/fitSignal_WZ_ele_cwwwPos.pdf
cp ~/private/FittingForATGC/Signal/CMSSW_7_1_5/src/FittingForATGCSignal/Plots/cwww_pos_mu_WZ.pdf fitSignal/fitSignal_WZ_mu_cwwwPos.pdf
cp ~/private/FittingForATGC/Signal/CMSSW_7_1_5/src/FittingForATGCSignal/Plots/ccw_pos_mu_WZ.pdf fitSignal/fitSignal_WZ_mu_ccwPos.pdf
cp ~/private/FittingForATGC/Signal/CMSSW_7_1_5/src/FittingForATGCSignal/Plots/ccw_pos_el_WZ.pdf fitSignal/fitSignal_WZ_ele_ccwPos.pdf
cp ~/private/FittingForATGC/Signal/CMSSW_7_1_5/src/FittingForATGCSignal/Plots/cb_pos_el_WZ.pdf fitSignal/fitSignal_WZ_ele_cbPos.pdf
cp ~/private/FittingForATGC/Signal/CMSSW_7_1_5/src/FittingForATGCSignal/Plots/cb_pos_mu_WZ.pdf fitSignal/fitSignal_WZ_mu_cbPos.pdf

# Signal aTGC negative values
# WW
cp ~/private/FittingForATGC/Signal/CMSSW_7_1_5/src/FittingForATGCSignal/Plots/cwww_neg_el_WW.pdf fitSignal/fitSignal_WW_ele_cwwwNeg.pdf
cp ~/private/FittingForATGC/Signal/CMSSW_7_1_5/src/FittingForATGCSignal/Plots/cwww_neg_mu_WW.pdf fitSignal/fitSignal_WW_mu_cwwwNeg.pdf
cp ~/private/FittingForATGC/Signal/CMSSW_7_1_5/src/FittingForATGCSignal/Plots/ccw_neg_mu_WW.pdf fitSignal/fitSignal_WW_mu_ccwNeg.pdf
cp ~/private/FittingForATGC/Signal/CMSSW_7_1_5/src/FittingForATGCSignal/Plots/ccw_neg_el_WW.pdf fitSignal/fitSignal_WW_ele_ccwNeg.pdf
cp ~/private/FittingForATGC/Signal/CMSSW_7_1_5/src/FittingForATGCSignal/Plots/cb_neg_el_WW.pdf fitSignal/fitSignal_WW_ele_cbNeg.pdf
cp ~/private/FittingForATGC/Signal/CMSSW_7_1_5/src/FittingForATGCSignal/Plots/cb_neg_mu_WW.pdf fitSignal/fitSignal_WW_mu_cbNeg.pdf
# WZ
cp ~/private/FittingForATGC/Signal/CMSSW_7_1_5/src/FittingForATGCSignal/Plots/cwww_neg_el_WZ.pdf fitSignal/fitSignal_WZ_ele_cwwwNeg.pdf
cp ~/private/FittingForATGC/Signal/CMSSW_7_1_5/src/FittingForATGCSignal/Plots/cwww_neg_mu_WZ.pdf fitSignal/fitSignal_WZ_mu_cwwwNeg.pdf
cp ~/private/FittingForATGC/Signal/CMSSW_7_1_5/src/FittingForATGCSignal/Plots/ccw_neg_mu_WZ.pdf fitSignal/fitSignal_WZ_mu_ccwNeg.pdf
cp ~/private/FittingForATGC/Signal/CMSSW_7_1_5/src/FittingForATGCSignal/Plots/ccw_neg_el_WZ.pdf fitSignal/fitSignal_WZ_ele_ccwNeg.pdf
cp ~/private/FittingForATGC/Signal/CMSSW_7_1_5/src/FittingForATGCSignal/Plots/cb_neg_el_WZ.pdf fitSignal/fitSignal_WZ_ele_cbNeg.pdf
cp ~/private/FittingForATGC/Signal/CMSSW_7_1_5/src/FittingForATGCSignal/Plots/cb_neg_mu_WZ.pdf fitSignal/fitSignal_WZ_mu_cbNeg.pdf


