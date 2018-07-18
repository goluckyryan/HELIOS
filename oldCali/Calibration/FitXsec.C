TGraph * g1, *g2, *gData;

Double_t func1(Double_t *x, Double_t *) {return g1->Eval(x[0]);}
Double_t func2(Double_t *x, Double_t *) {return g2->Eval(x[0]);}
Double_t func(Double_t *x, Double_t *para) {
   double f1 = g1->Eval(x[0]);
   double f2 = g2->Eval(x[0]);
   double val = para[0] * f1 + para[1] * f2;
   
   return val;
}

Double_t Average(TGraph * g, double x1, double x2, int n){
   double dx = (x2-x1)/n;
   
   double val = 0;
   for(int i = 0; i <=n ;i ++){
      val += g->Eval(x1 + dx*i);
      //printf("%d, %f, %f\n", i, x1+dx*i, val); 
   }
   
   return val /(n+1);
}

double expAngle[4][4] = {
   { 22.35, 27.85, 32.40, 36.40},
   { 13.65, 21.90, 27.75, 32.50},
   { 12.20, 19.40, 26.05, 31.15},
   {      , 15.90, 24.10, 29.65}
};

double expEx[4] = {0.0, 1.58, 2.0, 2.5};

double expData[4][4] = {
   { 1132, 1135, 1376, 1104},          
   { 2566, 2981, 3785, 3627},     
   {        584, 1098, 1117},
   {       2234, 3796, 4471} 
};

void FitXsec(int expCol, int k1, int k2){
   
   int fitCol[2] = {k1,k2}; // start from 0, 0 = angle, 1 = ground state
   //int expCol = 3;
   
   printf("============== Ex : %f MeV\n", expEx[expCol]);
   
   TCanvas * cFitXsec = new TCanvas("cFitXsec", "cFitXsec", 0, 0, 1000, 500);
   
   cFitXsec->Divide(2,1);
   for( int i = 1; i <= 2 ; i++){
      cFitXsec->cd(i)->SetGrid();
      cFitXsec->cd(i)->SetLogy();
   }
   cFitXsec->cd(1);
   TString expression;
   
   //======== load Xsec data
   ifstream file;
   TString fileName = "~/Ptolemy/208Pb_dp.out.Xsec.tx";
   file.open(fileName.Data());
   printf("--- using theoretical Xsec : %s \n", fileName.Data());
   string line;
   vector<double> angle;
   vector<double> f1;
   vector<double> f2;
   TString g1Name, g2Name;
   double range[2] = {1,1};
   
   if( file.is_open() ){
      int lineNum = 0;
      while( ! file.eof() ){
         getline(file, line);
         lineNum += 1;
         if( lineNum == 1 ) {
            
            g1Name = line.substr(18*fitCol[0], 18);
            g2Name = line.substr(18*fitCol[1], 18);
            continue;
         }
         // get angle
         int len = line.length();
         if( (18 + 18*fitCol[0] > len) || (19 + 18*fitCol[1] > len)) {
            printf("accessing location longer then line.length(). \n");
            break;
         }
         angle.push_back(atof(line.substr(0, 18).c_str()));
         f1.push_back(atof(line.substr(18*fitCol[0], 18).c_str()));
         f2.push_back(atof(line.substr(18*fitCol[1], 18).c_str()));
         
         if( f1.back() > range[0] ) range[0] = f1.back();
         if( f1.back() < range[1] ) range[1] = f1.back();
         
         if( k2 > 0 ){
            if( f2.back() > range[0] ) range[0] = f2.back();
            if( f2.back() < range[1] ) range[1] = f2.back();
         }
         
         //printf("%d| %f, %f, %f \n", lineNum,  angle.back(), f1.back(), f2.back());
      }
   }else{
      printf("... fail\n");
   }
   
   g1 = new TGraph(); g1->SetLineColor(4); g1->SetTitle(g1Name);
   g2 = new TGraph(); g2->SetLineColor(2); g2->SetTitle(g2Name);
   
   for( int i =0 ; i < angle.size() ; i++){
      g1->SetPoint(i, angle[i], f1[i]);
      g2->SetPoint(i, angle[i], f2[i]);
   }

   //============================= filling gData and fitting
   
   double dphi = 4 * 2 * TMath::ATan(4.5/11.);
   double count2Xsec = 1e+27/ 2.36e+30;
   
   gData = new TGraph();
   gData->SetTitle("Exp");
   int i = expCol;
   double startT = 50; 
   double endT = 0;
   for(int j = 0; j < 12 ; j++){
      if( expData[i][j] == 0.0 ) {
         continue;
      }else{
         dOmega = 0.11 / 0.5605;
         
         double data = expData[i][j] / dOmega * count2Xsec;
         
         printf("%d, %4.1f %6.3f\n", j, expAngle[i][j], data);
         
         gData->SetPoint(j, expAngle[i][j], data);
         
         if( data > range[0] ) range[0] = data;
         if( data < range[1] ) range[1] = data;
         
         if( expAngle[i][j] < startT) startT = expAngle[i][j];
         if( expAngle[i][j] > endT  ) endT   = expAngle[i][j] ;
         
         //printf("%f, %f, %f \n", expAngle[i][j], startT, endT);
         
      }
   }
   
   startT -= 5;
   endT += 5;
   
   double up = TMath::Power(10,TMath::Ceil(TMath::Log10(range[0])));
   double down = TMath::Power(10,TMath::Floor(TMath::Log10(range[1])));
   //printf(" %f %f \n", down, up);
   
   auto legend = new TLegend(0.6, 0.7, 0.9, 0.9);
   legend->AddEntry(g1, g1Name , "l");
   if( k2 > 0 ) legend->AddEntry(g2, g2Name , "l");
   TString gDataName;
   gDataName.Form("  Exp %4.1f MeV", expEx[expCol]);
   legend->AddEntry(gData, gDataName , "l");
   
   //====== drawing
   cFitXsec->cd(1);
   g1->GetYaxis()->SetRangeUser(down, up);
   g1->Draw("AC");
   if( k2 > 0 ) g2->Draw("same");
   gData->Draw("* same");
   legend->Draw();

   cFitXsec->cd(2);
   if( k2 == 0 ){
      up = 1;
      down = 0.01;
   }
   gData->GetYaxis()->SetRangeUser(down, up);   
   gData->GetXaxis()->SetLimits(0,50);
   gData->GetXaxis()->SetTitle("theta_CM [deg]");
   gData->GetYaxis()->SetTitle("dsigma/dOmega [mb]");
   gData->Draw("A*");
   
   
   if( k2 > 0){
      TF1 * fit = new TF1("fit", func, startT , endT, 2);
      double para[2] = { 1 , 1};
      fit->SetParameters(para);
      fit->SetParLimits(0, 0, 1000);
      fit->SetParLimits(1, 0, 1000);
      fit->SetLineColor(1);
      gData->Fit("fit");
      
      TF1 * fit1 = new TF1("fit1", func, startT , endT, 2);
      double a  = fit->GetParameter(0);
      double ae = fit->GetParError(0);
      fit1->SetParameter(0, a);
      fit1->SetParameter(1, 0);
      fit1->SetLineColor(4);
      
      TF1 * fit2 = new TF1("fit2", func, startT , endT, 2);
      double b  = fit->GetParameter(1);
      double be = fit->GetParError(1);
      fit2->SetParameter(0, 0);
      fit2->SetParameter(1, b);
      fit2->SetLineColor(2);
      
      fit1->Draw("same");
      fit2->Draw("same");
      
      TLatex text;
      TString textStr;
      
      text.SetNDC(true);
      text.SetTextAlign(13); // align at Top
      text.SetTextSize(0.05);
      
      textStr.Form("1s_{1/2} : %4.2f (%4.2f)", a, ae);   
      text.DrawLatex( 0.2, 0.85, textStr);
      
      textStr.Form("0d_{5/2} : %4.2f (%4.2f)", b, be);   
      text.DrawLatex( 0.2, 0.8, textStr);
   }else{
      TF1 * fit = new TF1("fit", func, startT , endT, 2);
      fit->SetParameter(0, 1.);
      fit->SetParameter(1, 0.);
      fit->SetParLimits(0, 0, 1000);
      fit->SetParLimits(1, 0, 0.001);
      fit->SetLineColor(1);
      gData->Fit("fit", "n");
      
      TF1 * fit1 = new TF1("fit1", func, startT , endT, 2);
      double a  = fit->GetParameter(0);
      double ae = fit->GetParError(0);
      fit1->SetParameter(0, a);
      fit1->SetParameter(1, 0);
      fit1->SetLineColor(4);
      fit1->Draw("same");
      
      TLatex text;
      TString textStr;
      
      text.SetNDC(true);
      text.SetTextAlign(13); // align at Top
      text.SetTextSize(0.05);
      
      textStr.Form("0d_{5/2} : %4.2f (%4.2f)", a, ae);   
      text.DrawLatex( 0.2, 0.85, textStr);
         
   }
   
   legend->Draw();
   
}
