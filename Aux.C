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

double expAngle[7][6] = {
   { 24.2, 29.7, 34.3, 38.4},
   { 15.8, 24.4, 30.2, 34.9, 39.2, 43.0},
   { 11.5, 19.4, 26.7, 32.1, 36.8, 41.0},
   {       13.5, 22.7, 29.2, 34.4, 39.0},
   {       12.3, 20.9, 27.9, 33.4, 38.1},
   {       10.1, 17.7, 26.0, 31.9, 36.9},
   {             14.2, 24.0, 30.4, 35.7}
};

double expDangle[4][6] = {
   {  5.6,  4.6,  4.0,  3.6},
   { 10.6,  6.1,  4.9,  4.3,  3.8,  3.5},
   { 11.7, 19.6, 26.8, 32.3, 36.9, 41.1},
   {       13.6, 22.9, 29.3, 34.5, 39.1}
};


double expData[7][6] = {
   { 84,  66,  73,  51},
   { 71, 140, 140,  78,  45,  24},
   {167, 175, 311, 296, 241,  70},
   {     334, 391, 370, 331, 103},
   {     230, 238, 347, 270, 114},
   {      29, 130, 170, 155,  56},
   {          129, 266, 198,  53}
};

void Aux(){
   TCanvas * cAux = new TCanvas("cAux", "cAux", 0, 0, 1000, 500);
   
   cAux->Divide(2,1);
   for( int i = 1; i <= 2 ; i++){
      cAux->cd(i)->SetGrid();
      cAux->cd(i)->SetLogy();
   }
   cAux->cd(1);
   TString expression;
   
   int fitCol[2] = {6,7}; // start from 0, 0 = angle, 1 = ground state
   int expCol = 3;
   
   //======== load Xsec data
   ifstream file;
   file.open("25Mg_dp.out.Xsec.txt");
   string line;
   vector<double> angle;
   vector<double> f1;
   vector<double> f2;
   if( file.is_open() ){
      int lineNum = 0;
      while( ! file.eof() ){
         getline(file, line);
         lineNum += 1;
         if( lineNum == 1 ) continue;
         
         // get angle
         int len = line.length();
         if( (17 + 18*fitCol[0] > len) || (17 + 18*fitCol[1] > len)) {
            printf("accessing location longer then line.length(). \n");
            break;
         }
         angle.push_back(atof(line.substr(0, 17).c_str()));
         f1.push_back(atof(line.substr(17 + 18*(fitCol[0]-1), 18).c_str()));
         f2.push_back(atof(line.substr(17 + 18*(fitCol[1]-1), 18).c_str()));
         
         //printf("%d| %f, %f, %f \n", lineNum,  angle.back(), f1.back(), f2.back());
      }
   }else{
      printf("... fail\n");
   }

   g1 = new TGraph(); g1->SetLineColor(4);
   g2 = new TGraph(); g2->SetLineColor(2);
   
   for( int i =0 ; i < angle.size() ; i++){
      g1->SetPoint(i, angle[i], f1[i]);
      g2->SetPoint(i, angle[i], f2[i]);
   }
   
   g1->Draw("AC");
   g2->Draw("same");

   
   //============================= fitting
   
   double dphi = 4 * 2 * TMath::ATan(4.5/11.);
   double count2Xsec = 1e+27/ 2.36e+30;
   
   gData = new TGraph();
   int i = expCol;
   for(int j = 0; j < 6 ; j++){
      if( expData[i] == 0 ) continue;
      double dOmega = TMath::Sin(expAngle[i][j]*TMath::DegToRad())* expDangle[i][j]*TMath::DegToRad()* dphi;
      dOmega = 0.01;
      printf("%d, %4.1f  %5.0f %5.3f  %6.3f\n", j, expAngle[i][j], expData[i][j], expDangle[i][j], dOmega);
      gData->SetPoint(j, expAngle[i][j], expData[i][j] / dOmega * count2Xsec);
   }
   
   
   
   gData->Draw("* same");

   cAux->cd(2);   
   gData->GetXaxis()->SetLimits(0,50);
   gData->Draw("A*");
   
   TF1 * fit = new TF1("fit", func, -5 , 45, 2);
   double para[2] = { 0.1 , 0.1};
   fit->SetParameters(para);
   fit->SetLineColor(6);
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
