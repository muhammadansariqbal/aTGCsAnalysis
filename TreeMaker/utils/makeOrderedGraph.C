{
  /** Little script to correctly order the points on a TGraphAsymmErrors.
   * This adds the new graph to the original ROOT file.
   * Depending on what sort of object is stored (TGraphAsymmErrors or TGraphErrors)
   * you'll need to (un)comment various type-bits below.
   **/

  TFile f("egammaEffi.txt_EGM2D.root", "UPDATE");
  TString graphName = "grSF1D_0";
  // TGraphAsymmErrors * tge = (TGraphAsymmErrors*) f.Get(graphName);
  TGraphErrors * tge = (TGraphErrors*) f.Get(graphName);

  if (tge == nullptr) {
    throw runtime_error("Couldn't get graph from file");
  }

  std::map<float, int> xvals;
  int N = tge->GetN();
  double * grX = tge->GetX();
  double * grY = tge->GetY();

  // If using TGraphAsymmErrors
  // double * grEXHigh = tge->GetEXhigh();
  // double * grEXLow = tge->GetEXlow();
  // double * grEYHigh = tge->GetEYhigh();
  // double * grEYLow = tge->GetEYlow();

  // If using TGraphErrors
  // In TGraphError, ErrorXhigh = ErrorXlow, same for Y.
  double * grEXHigh = tge->GetEX();
  double * grEXLow = tge->GetEX();
  double * grEYHigh = tge->GetEY();
  double * grEYLow = tge->GetEY();

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

  // Save as TGraphAsymmErrors as more flexible/uniform wiht other TGraph sources
  TGraphAsymmErrors * tgeNew = new TGraphAsymmErrors(N, &newX[0], &newY[0], &newEXLow[0], &newEXHigh[0], &newEYLow[0], &newEYHigh[0]);
  tgeNew->SetName(graphName + "_Ordered");
  tgeNew->Draw("ALP");
  tgeNew->Write();
}
