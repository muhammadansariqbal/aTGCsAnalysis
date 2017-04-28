#ifndef PU_H
#define PU_H
 std::vector<float> PU_data, PU_MC;

 /* pileup is extracted with command (72 mb):
 * pileupCalc.py -i /afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV/Cert_271036-275125_13TeV_PromptReco_Collisions16_JSON.txt --inputLumiJSON /afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV/PileUp/pileup_latest.txt --calcMode true --minBiasXsec 72000 --maxPileupBin 50 --numPileupBins 50 MyDataPileupHistogram.root
 */
float PU_data_f[50] = {
					4.07905981095e-07 ,
					6.46868965726e-06 ,
					4.31251234201e-05 ,
					0.000112740522978 ,
					0.000185805819086 ,
					0.000250628404513 ,
					0.000343808215205 ,
					0.00160514026836 ,
					0.0053559394764 ,
					0.00890485009822 ,
					0.0141589747463 ,
					0.0219824592363 ,
					0.033057355943 ,
					0.0491740477796 ,
					0.0684327429077 ,
					0.0852440311894 ,
					0.0960379347792 ,
					0.10080522323 ,
					0.0995956703247 ,
					0.0929668839536 ,
					0.0823493751899 ,
					0.0693206266628 ,
					0.0553835363731 ,
					0.0417962926189 ,
					0.0295445859 ,
					0.0193725335228 ,
					0.0116848371338 ,
					0.00644867241147 ,
					0.00325249536899 ,
					0.0015042913411 ,
					0.000643055617308 ,
					0.000257229440005 ,
					9.80085124829e-05 ,
					3.65829070001e-05 ,
					1.408444309e-05 ,
					6.12898387595e-06 ,
					3.34911040406e-06 ,
					2.34620048171e-06 ,
					1.94219664664e-06 ,
					1.74480849359e-06 ,
					1.62743535083e-06 ,
					1.54866700542e-06 ,
					1.49261819285e-06 ,
					1.45063170225e-06 ,
					1.41558003798e-06 ,
					1.38190417374e-06 ,
					1.34671499961e-06 ,
					1.30723064026e-06 ,
					1.26185954139e-06 ,
					1.21000205496e-06 ,

};

/* MC distribution is taken from: 
* SimGeneral/MixingModule/python/mix_2015_25ns_FallMC_matchData_PoissonOOTPU_cfi.py
*/
float PU_MC_f[50]  {
 					0.000829312873542,
			 		0.00124276120498,
			 		0.00339329181587,
			 		0.00408224735376,
			 		0.00383036590008,
					0.00659159288946,
			 		0.00816022734493,
			 		0.00943640833116,
			 		0.0137777376066,
			 		0.017059392038,
			 		0.0213193035468,
			 		0.0247343174676,
			 		0.0280848773878,
			 		0.0323308476564,
			 		0.0370394341409,
			 		0.0456917721191,
			 		0.0558762890594,
			 		0.0576956187107,
			 		0.0625325287017,
			 		0.0591603758776,
			 		0.0656650815128,
			 		0.0678329011676,
			 		0.0625142146389,
			 		0.0548068448797,
			 		0.0503893295063,
			 		0.040209818868,
			 		0.0374446988111,
			 		0.0299661572042,
			 		0.0272024759921,
			 		0.0219328403791,
			 		0.0179586571619,
			 		0.0142926728247,
			 		0.00839941654725,
			 		0.00522366397213,
			 		0.00224457976761,
			 		0.000779274977993,
			 		0.000197066585944,
			 		7.16031761328e-05,
			 		0.0,
					0.0,
					0.0,
					0.0,
					0.0,
					0.0,
					0.0,
					0.0,
					0.0,
			 		0.0,
			 		0.0,
					0.0 
};

std::vector<float> MC_dist(){ 
	for (unsigned int iPU = 0; iPU < 52; iPU++){
		PU_MC.push_back(PU_MC_f[iPU]);
	}
	return PU_MC;};
std::vector<float> data_dist(){ 
		for (unsigned int iPU = 0; iPU < 52; iPU++){
		PU_data.push_back(PU_data_f[iPU]);
	}
	return PU_data;};

#endif