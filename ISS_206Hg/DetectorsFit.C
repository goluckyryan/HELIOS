//Detector analysis 
#include <vector>
#include <TGraphErrors.h>

void DetectorsFit(){
  
  vector<int>    count0, dcount0;
  vector<double> mean0,  dmean0;
  vector<double> sigma0, dsigma0;
  
  vector<int>    count1, dcount1;
  vector<double> mean1,  dmean1;
  vector<double> sigma1, dsigma1;
  
  for( int i = 0 ; i < 24 ; i ++ ){
    
    
    vector<double> ans = fit2GaussP1(hExi[i], 0.1, 0.07, 1.2, 0.07, -2, 1.5, 1);
    
    count0.push_back(ans[0]);
    dcount0.push_back(ans[1]);
    mean0.push_back(ans[2]);
    dmean0.push_back(ans[3]);
    sigma0.push_back(ans[4]);
    dsigma0.push_back(ans[5]);
    
    count1.push_back(ans[6]);
    dcount1.push_back(ans[7]);
    mean1.push_back(ans[8]);
    dmean1.push_back(ans[9]);
    sigma1.push_back(ans[10]);
    dsigma1.push_back(ans[11]);
    
  }
  
  //print result
  printf("============================ result\n");
  for( int i = 0 ; i < 24 ; i ++){
    
    printf("%2d |%7d %7d %7.3f %7.3f %3.0f %3.0f  %7d %7d %7.3f %7.3f %3.0f %3.0f\n", 
       i, count0[i], dcount0[i], mean0[i], dmean0[i], sigma0[i]*1000, dsigma0[i]*1000,
       count1[i], dcount1[i], mean1[i], dmean1[i], sigma1[i]*1000, dsigma1[i]*1000);
  }
  printf("============================\n");
  
  
  //Plot graph

  TCanvas * cDetFit = new TCanvas("cDetFit", "Detectors Fit", 600, 600);
  
  cDetFit->Divide(1,1);
  
  cDetFit->cd(1);
  
  TGraphErrors * gr0 = new TGraphErrors();
  gr0->SetMarkerColor(4);
  gr0->SetMarkerSize(1.5);
  gr0->SetMarkerStyle(4);
  gr0->GetXaxis()->SetTitle("#sigma_{0} [keV]");
  gr0->GetYaxis()->SetTitle("#sigma_{1} [keV]");
  
  
  
  for( int i = 0; i < 24; i ++){
    if( i == 11 || i == 0 ) continue;
    if( dsigma0[i] * 1000 > 200 ) continue;
    if( dsigma1[i] * 1000 > 200 ) continue;
    gr0->SetPoint(i, sigma0[i]*1000, sigma1[i]*1000);
    gr0->SetPointError(i, dsigma0[i]*1000, dsigma1[i]*1000);
  }
  
  
  gr0->Draw("AP");
  gr0->GetXaxis()->SetLimits(-50, 200);
  gr0->GetYaxis()->SetRangeUser(-50, 200);
  gr0->Draw("AP");
  
  
  printf("============================ better check again. sometimes it is very sensitive to initial guess\n");
}
