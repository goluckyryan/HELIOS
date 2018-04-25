TGraph * g1, *g2;

Double func1(Double_t *x, Double_t *) {return g1->Eval(x[0]);};
Double func2(Double_t *x, Double_t *) {return g2->Eval(x[0]);};

double expAngle[7][6] = {
   { 24.2, 29.7, 34.3, 38.4},
   { 15.8, 24.4, 30.2, 34.9, 39.2, 43.0},
   { 11.5, 19.4, 26.7, 32.1, 36.8, 41.0},
   {       13.5, 22.7, 29.2, 34.4, 39.0},
   {       12.3, 20.9, 27.9, 33.4, 38.1},
   {       10.1, 17.7, 26.0, 31.9, 36.9},
   {             14.2, 24.0, 30.4, 35.7}
};

double expDangle[2][6] = {
   { 5.6, 4.6, 4.0, 3.6},
   { 10.6, 6.1, 4.9, 4.3, 3.8, 3.5}
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
   TCanvas * cAux = new TCanvas("cAux", "cAux", 0, 0, 500, 500);
   
   cAux->Divide(1,1);
   for( int i = 1; i <= 3 ; i++){
      cAux->cd(i)->SetGrid();
      cAux->cd(i)->SetLogy();
   }
   cAux->cd(1);
   TString expression;
   
   int fitCol[2] = {2,3};
   
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
      g1.SetPoint(i, angle[i], f1[i]);
      g2.SetPoint(i, angle[i], f2[i]);
   }
   
   g1.Draw("AC");
   g2.Draw("same");
   
}
