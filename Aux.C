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

double expAngle[12][6] = {
   { 24.2, 29.7, 34.3, 38.4},
   { 15.8, 24.4, 30.2, 35.0, 39.2, 43.0},
   {       19.6, 26.8, 32.3, 36.9, 41.1},
   {       11.1, 23.0, 29.3, 34.5, 39.1},
   {       10.1, 21.0, 28.0, 33.5, 38.2},
   {             17.1, 25.7, 31.7, 36.8},
   {             11.5, 24.0, 30.4, 35.7},
   {             11.1, 23.0, 29.7, 35.2},
   {             10.5, 22.0, 29.1, 34.6},
   {              9.5, 19.6, 27.5, 33.3},
   {                   11.1, 23.3, 30.2},
   {                    9.9, 20.9, 28.6}
};

double expDangle[12][6] = {
   {  5.5,  4.5,  4.0,  3.6},
   { 10.7,  6.2,  4.9,  4.3,  3.9,  3.5},
   {        8.2,  5.8,  4.9,  4.2,  3.8},
   {       16.3,  6.9,  5.4,  4.6,  4.1},
   {       13.4,  7.8,  5.8,  4.9,  4.2},
   {             10.6,  6.4,  5.2,  4.5},
   {             17.6,  7.0,  5.4,  4.7},
   {             16.3,  7.4,  5.6,  4.9},
   {             14.8,  8.0,  5.9,  4.9},
   {             10.9,  9.2,  6.3,  5.2},
   {                   16.4,  7.9,  5.9},
   {                   12.8,  9.1,  6.4}
};

double expEx[12] = {0.0, 1.8, 1.9, 2.9, 4.3, 4.9, 5.3, 5.5, 5.7, 6.1, 7.0, 7.4};

double expData[12][6] = {
   { 84,  52,  73,  51},                  // 0.0
   { 71, 120, 140,  78,   45,  24},       // 1.8
   {     147, 309, 296,  241,  70},       // 2.9
   {     313, 391, 370,  331, 103},       // 3.9
   {     191, 237, 347,  270, 115},       // 4.3
   {          130, 170,  155,  56},       // 4.9
   {          129, 266,  198,  53},       // 5.3
   {           78,  93,   96,  68},       // 5.5
   {           26,  67,   75,  48},       // 5.7
   {          124, 328,  370, 210},       // 6.1
   {               494,  310, 113},       // 7.0
   {              1071, 1032, 494}        // 7.4
};

void Aux(int expCol, int k1, int k2){
   
   int fitCol[2] = {k1,k2}; // start from 0, 0 = angle, 1 = ground state
   //int expCol = 3;
   
   printf("============== Ex : %f MeV\n", expEx[expCol]);
   
   TCanvas * cAux = new TCanvas("cAux", "cAux", 0, 0, 1000, 500);
   
   cAux->Divide(2,1);
   for( int i = 1; i <= 2 ; i++){
      cAux->cd(i)->SetGrid();
      cAux->cd(i)->SetLogy();
   }
   cAux->cd(1);
   TString expression;
   
   //======== load Xsec data
   ifstream file;
   file.open("25Mg_dp.out.Xsec.txt");
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
            
            g1Name = line.substr(17 + 18*(fitCol[0]-1), 18);
            g2Name = line.substr(17 + 18*(fitCol[1]-1), 18);
            continue;
         }
         // get angle
         int len = line.length();
         if( (17 + 18*fitCol[0] > len) || (17 + 18*fitCol[1] > len)) {
            printf("accessing location longer then line.length(). \n");
            break;
         }
         angle.push_back(atof(line.substr(0, 17).c_str()));
         f1.push_back(atof(line.substr(17 + 18*(fitCol[0]-1), 18).c_str()));
         f2.push_back(atof(line.substr(17 + 18*(fitCol[1]-1), 18).c_str()));
         
         if( f1.back() > range[0] ) range[0] = f1.back();
         if( f2.back() > range[0] ) range[0] = f2.back();
         
         if( f1.back() < range[1] ) range[1] = f1.back();
         if( f2.back() < range[1] ) range[1] = f2.back();
         
         
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
   
   
   //============================= fitting
   
   double dphi = 4 * 2 * TMath::ATan(4.5/11.);
   double count2Xsec = 1e+27/ 2.36e+30;
   
   gData = new TGraph();
   gData->SetTitle("Exp");
   int i = expCol;
   for(int j = 0; j < 6 ; j++){
      if( expData[i][j] == 0.0 ) {
         continue;
      }else{
         double dOmega = TMath::Sin(expAngle[i][j]*TMath::DegToRad())* expDangle[i][j]*TMath::DegToRad()* dphi;
         dOmega = 0.11;
         //printf("%d, %4.1f  %5.0f %5.3f  %6.3f\n", j, expAngle[i][j], expData[i][j], expDangle[i][j], dOmega);
         
         double data = expData[i][j] / dOmega * count2Xsec;
         
         gData->SetPoint(j, expAngle[i][j], data);
         
         if( data > range[0] ) range[0] = data;
         if( data < range[1] ) range[1] = data;
      }
   }
   
   double up = TMath::Power(10,TMath::Ceil(TMath::Log10(range[0])));
   double down = TMath::Power(10,TMath::Floor(TMath::Log10(range[1])));
   //printf(" %f %f \n", down, up);
   
   auto legend = new TLegend(0.6, 0.7, 0.9, 0.9);
   legend->AddEntry(g1, g1Name , "l");
   legend->AddEntry(g2, g2Name , "l");
   legend->AddEntry(gData, "Exp" , "l");
   
   //====== drawing
   cAux->cd(1);
   g1->GetYaxis()->SetRangeUser(down, up);
   g1->Draw("AC");
   g2->Draw("same");
   gData->Draw("* same");
   legend->Draw();

   cAux->cd(2);
   gData->GetYaxis()->SetRangeUser(down, up);   
   gData->GetXaxis()->SetLimits(0,50);
   gData->Draw("A*");
   
   TF1 * fit = new TF1("fit", func, -5 , 45, 2);
   double para[2] = { 1 , 1};
   fit->SetParameters(para);
   fit->SetParLimits(0, 0, 1000);
   fit->SetParLimits(1, 0, 1000);
   fit->SetLineColor(1);
   gData->Fit("fit");
   
   TF1 * fit1 = new TF1("fit1", func, -5, 45, 2);
   double a = fit->GetParameter(0);
   fit1->SetParameter(0, a);
   fit1->SetParameter(1, 0);
   fit1->SetLineColor(4);
   
   TF1 * fit2 = new TF1("fit2", func, -5, 45, 2);
   double b = fit->GetParameter(1);
   fit2->SetParameter(0, 0);
   fit2->SetParameter(1, b);
   fit2->SetLineColor(2);
   
   fit1->Draw("same");
   fit2->Draw("same");
   
}
