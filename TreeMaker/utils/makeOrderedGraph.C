{
  /** Little script to correctly order the points on a TGraphAsymmErrors.
   * This adds the new graph to the original ROOT file.
   **/

  TFile f("HEEP_SF.root", "UPDATE");
  TGraphAsymmErrors * tge = (TGraphAsymmErrors*) f.Get("SF_Eta");

  std::map<float, int> xvals;
  int N = tge->GetN();
  double * grX = tge->GetX();
  double * grY = tge->GetY();
  double * grEXHigh = tge->GetEXhigh();
  double * grEXLow = tge->GetEXlow();
  double * grEYHigh = tge->GetEYhigh();
  double * grEYLow = tge->GetEYlow();

  // std::map auto sorts by key, whoop, no work for me
  for (int i=0; i<tge->GetN(); i++) {
    xvals[grX[i]] = i;
  }

  std::vector<double> newX(N);
  std::vector<double> newY(N);
  std::vector<double> newEXHigh(N);
  std::vector<double> newEXLow(N);
  std::vector<double> newEYHigh(N);
  std::vector<double> newEYLow(N);
  int i = 0;
  for (auto & itr: xvals) {
    cout << itr.first << " = " << itr.second << endl;
    newX[i] = itr.first;
    newY[i] = grY[itr.second];
    newEXHigh[i] = grEXHigh[itr.second];
    newEXLow[i] = grEXLow[itr.second];
    newEYHigh[i] = grEYHigh[itr.second];
    newEYLow[i] = grEYLow[itr.second];
    i++;
  }

  TGraphAsymmErrors * tgeNew = new TGraphAsymmErrors(N, &newX[0], &newY[0], &newEXLow[0], &newEXHigh[0], &newEYLow[0], &newEYHigh[0]);
  tgeNew->SetName("SF_Eta_Ordered");
  tgeNew->Draw("ALP");
  tgeNew->Write();
}
