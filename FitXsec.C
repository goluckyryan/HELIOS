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

double expAngle[13][12] = {
   { 24.4, 29.8, 34.5, 38.5, 42.3},
   { 16.2, 24.4, 30.2, 35.0, 39.2, 43.0},
   {       20.0, 26.8, 32.3, 36.9, 41.1},
   {       17.9, 21.6, 25.5, 28.1, 31.2, 33.3, 34.0, 37.9, 40.2, 42},
   {       13.9, 23.0, 29.4, 34.5, 39.1},
   {       12.6, 21.0, 28.0, 33.5, 38.2},
   {             17.4, 25.7, 31.7, 36.8},
   {             14.4, 24.0, 30.4, 35.7},
   {             13.8, 23.0, 29.7, 35.2},
   {             13.2, 22.0, 29.1, 34.6},
   {             11.5, 19.6, 27.5, 33.3},
   {                   13.9, 23.3, 30.2},
   {                   12.3, 20.9, 28.6}
};

double expDangle[13][12] = {
   {  5.5,  4.5,  4.0,  3.6,  3.2},
   { 10.7,  6.2,  4.9,  4.3,  3.9,  3.5},
   {        5.7,  5.8,  4.9,  4.2,  3.8},
   {        3.7,  3.7,  2.6,  2.6, 2.2 , 2.2, 1.9, 1.9, 1.8, 1.8 },
   {              6.9,  5.4,  4.6,  4.1},
   {              7.8,  5.8,  4.9,  4.2},
   {             10.6,  6.4,  5.2,  4.5},
   {             17.6,  7.0,  5.4,  4.7},
   {             16.3,  7.4,  5.6,  4.9},
   {             14.8,  8.0,  5.9,  4.9},
   {             10.9,  9.2,  6.3,  5.2},
   {                   16.4,  7.9,  5.9},
   {                   12.8,  9.1,  6.4}
};

double expEx[13] = {0.0, 1.8, 2.9, 2.9, 3.9, 4.3, 4.9, 5.3, 5.5, 5.7, 6.1, 7.0, 7.4};

double expData[13][12] = {
   {105,  66,  73,  51,   44},            // 0.0 -- OK d5/2
   { 85, 140, 140,  78,   38,  32},       // 1.8 -- OK d5/2, no dip ~ 20 deg of s1/2
   {     170, 311, 297,  241,  93},       // 2.9 -- a dip ~ 20 deg, should be s1/2
   {     200, 150, 298,  324, 302, 276, 212, 254, 130, 98 }, // spliting half of 2.9 MeV
   {     275, 391, 370,  331, 137},       // 3.9 -- a dip ~ 20 deg, should be s1/2
   {     142, 237, 347,  270, 153},       // 4.3 -- a dip ~ 20 deg
   {          130, 170,  155,  75},       // 4.9 -- a dip ~ 20 deg
   {          126, 266,  198,  71},       // 5.3 -- OK d5/2
   {           67,  93,   96,  88},       // 5.5 -- no dip, d-shell
   {           17,  68,   75,  64},       // 5.7 -- no dip, d-shell
   {           40, 328,  370, 280},       // 6.1 -- no dip
   {               433,  310, 151},       // 7.0 -- no dip
   {               569, 1032, 659}        // 7.4 -- no dip
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
   TString fileName = "25Mg_dp_3.out.Xsec.txt";
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
         double dOmega = TMath::Sin(expAngle[i][j]*TMath::DegToRad())* expDangle[i][j]*TMath::DegToRad()* dphi;
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
