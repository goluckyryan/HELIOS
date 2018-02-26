{
/**///======================================================== initial input
   
   //const char* rootfile="psd_run38.root"; const char* treeName="psd_tree";
   //const char* rootfile="H052_Mg25.root"; const char* treeName="gen_tree";
   
   const char* rootfile="C_H052_Mg25.root"; const char* treeName="tree";
   
      // pause
   int detID;
   printf("which detector ? ");
   scanf("%d", &detID);

   const int numDet = 4;
   int eRange[3] = {150, -300, 1000};

   double cutSlope = -2.35;
   double cutIntep[4] = {920, 900, 880, 920}; 
   double slope = 4.75;
   double yIntep = 700;
   int numPeak = 10;
   eRange[0]=  80;   
   
   if( detID == 0){
      cutIntep[0] = 950;
      cutIntep[1] = 980;
      cutIntep[2] = 800;
      cutIntep[3] = 900;
      slope = 4.75;

   
      eRange[1]=  400;
      eRange[2]= 1800;
   }
   
   if( detID == 1 ){
      cutIntep[0] = 930;
      cutIntep[1] = 930;
      cutIntep[2] = 930;
      cutIntep[3] = 940; 
      slope = 4.75;

      eRange[1]=  200;
      eRange[2]= 1600;
   }
   
   if( detID == 2) {
      cutIntep[0] = 1000;
      cutIntep[1] = 880;
      cutIntep[2] = 820;
      cutIntep[3] = 950;
      slope = 4.45;
      
      eRange[0]=  60;   
      eRange[1]= -300;
      eRange[2]= 1200;
   }
   
   if( detID == 3){
      cutIntep[0] = 930;
      cutIntep[1] = 930;
      cutIntep[2] = 930;
      cutIntep[3] = 920;
      slope = 4.3;
      
      eRange[0]=  100;
      eRange[1]= -500;
      eRange[2]= 1000;
   }
   
   if( detID == 4){
      cutIntep[0] = 930;
      cutIntep[1] = 930;
      cutIntep[2] = 1000;
      cutIntep[3] = 1050;
      slope = 4.0; // the slope can be difference.
      yIntep = 500;
      
      eRange[1]= -800;
      eRange[2]=  800;
   }
   
   if( detID == 5){
      cutIntep[0] = 930;
      cutIntep[1] = 930;
      cutIntep[2] = 960;
      cutIntep[3] = 940;
      slope = 3.7;
      
      eRange[1]= -1000;
      eRange[2]=  800;
   }
   
/**///========================================================  load tree

   TFile *f0 = new TFile (rootfile, "read"); 
   TTree *tree = (TTree*)f0->Get(treeName);
   printf("=====> /// %15s //// is loaded. Total #Entry: %10d \n", rootfile,  tree->GetEntries());
   
/**///======================================================== Browser or Canvas

   //TBrowser B ;   
   Int_t Div[2] = {2,2};  //x,y
   Int_t size[2] = {400,400}; //x,y
   
   TCanvas * cScript = new TCanvas("cScript", "cScript", 0, 0, size[0]*Div[0], size[1]*Div[1]);
   cScript->Divide(Div[0],Div[1]);
   cScript->cd(1);
   for( int i = 1; i <= Div[0]+Div[1] ; i++){
      cScript->cd(i)->SetGrid();
   }

   gStyle->SetOptStat(1111111);
   gStyle->SetStatY(1.0);
   gStyle->SetStatX(0.99);
   gStyle->SetStatW(0.2);
   gStyle->SetStatH(0.1);
   
/**///========================================================= load files
   double nearPos[6];
   double length;
   printf("----- loading sensor position.");
   ifstream file;
   file.open("nearPos.dat");
   double a;
   int i = 0;
   while( file >> a ){
      if( i >= 7) break;
      if( i == 6) length = a;
      nearPos[i] = a;
      i = i + 1;
   }
   file.close();
   printf("... done.\n      ");
   for(int i = 0; i < 5 ; i++){
      printf("%6.2f mm, ", nearPos[i]);
   }
   printf("%6.2f mm || length : %6.2f mm \n", nearPos[5], length);
   
/**///========================================================= Analysis
   
   
   
   printf("======= showing cut line(red) and slope (blue)\n");
   
   TH2F ** b = new TH2F[numDet];
   for( int i = 0; i < numDet; i ++){
      TString name;
      name.Form("b%d", i);
      b[i] = new TH2F(name, name , 200, nearPos[detID]-2 , nearPos[detID]+length+2 , 200, -100 , 2000);
      b[i]->SetXTitle("pos(xf,xn)");
      b[i]->SetYTitle("e");
      
      TString expression;
      expression.Form("e[%d]:x[%d]>> b%d" , detID + 6*i, detID + 6*i ,i);
      TString gate;
      gate.Form("");
      
      cScript->cd(i+1);
      
      tree->Draw(expression, gate , "");
   }
   
   TF1 ** line = new TF1[numDet];
   
   for( int i = 0; i < numDet; i++){
      TString name;
      name.Form("line%d", i);
      line[i] = new TF1(name, "[0] + x* [1]", nearPos[detID]-2 , nearPos[detID]+length+2);
      line[i]->SetParameter(1, cutSlope);
      line[i]->SetLineColor(2);
      line[i]->SetParameter(0, cutIntep[i]);
      cScript->cd(i+1); 
      line[i]->Draw("same");
   }
   
   
   TF1 * traj = new TF1("traj", "[0] + x * [1]", nearPos[detID]-2 , nearPos[detID]+length+2);
   traj->SetLineColor(4);
   traj->SetParameter(1, slope);
   traj->SetParameter(0, yIntep);
   
   for( int i = 0; i < numDet; i++){
      cScript->cd(i+1); 
      traj->Draw("same");
   }
  
   // pause
   cScript->Update();
   printf("0 for stop, 1 for continous : ");
   int dummy;
   scanf("%d", &dummy);
   if( dummy == 0 ) return;
   
   printf("======= showing tilted energy vs pos \n");
   
   //======== create 2-D histogram of projection
   TH2F ** p = new TH2F[numDet];
   for( int i = 0; i < numDet; i ++){
      TString name;
      name.Form("p%d", i);
      p[i] = new TH2F(name, name ,200, nearPos[detID]-2 , nearPos[detID]+length+2, eRange[0], eRange[1], eRange[2]);
      p[i]->SetYTitle("Ex [a.u.]");
      p[i]->SetXTitle("pos(xf,xn)");
      
      TString expression;
      expression.Form("e[%d] - %f * x[%d] : x[%d] >> p%d" , detID + 6*i, slope ,detID + 6*i ,detID + 6*i,  i);
      TString gate;
      gate.Form("e[%d] > %f + %f * x[%d] ", detID + 6*i, cutIntep[i], cutSlope, detID + 6*i);
      
      cScript->cd(i + 1);
      tree->Draw(expression, gate , "");
   }
   
      
   //pause
   cScript->Update();
   printf("0 for stop, 1 for projection and peak-finding, 2 for manual : ");
   scanf("%d", &dummy);
   if( dummy == 0 ) return;
   
   
   printf("======= showing energy spectrum and find peak\n");
   
   //======== create 1-D histogram of projection
   TH1F ** g = new TH1F[numDet];
   for( int i = 0; i < numDet; i ++){
      TString name;
      name.Form("g%d", i);
      g[i] = new TH1F(name, name , eRange[0], eRange[1], eRange[2]);
      g[i]->SetXTitle("Ex [a.u.]");
      
      TString expression;
      expression.Form("e[%d] - %f * x[%d]>> g%d" , detID + 6*i, slope ,detID + 6*i , i);
      TString gate;
      gate.Form("e[%d] > %f + %f * x[%d] ", detID + 6*i,cutIntep[i], cutSlope, detID + 6*i);
      
      cScript->cd(i + 1);
      
      tree->Draw(expression, gate , "");
   }
   
   vector<double> * peak = new vector<double>[numDet];
   int numCommonPeaks = 10;
   
   int ** index = new int[numDet];
   
   if( dummy == 1){
      //======== find peak using TSpectrum
      TSpectrum * spec = new TSpectrum(20);
   
      for( int i = 0; i < numDet; i ++){
         cScript->cd(i+1);
         int nPeaks = spec->Search(g[i], 1 ,"", 0.10);
         //float * xpos = new float[nPeaks];
         if( nPeaks < numCommonPeaks ) numCommonPeaks = nPeaks;

         float * xpos = spec->GetPositionX();
         // sorting and save 
         index[i] = new int[nPeaks];
         TMath::Sort(nPeaks, xpos, index[i], 0 );  
         for( int j = 0; j < nPeaks; j++){
            peak[i].push_back(xpos[index[i][j]]);
         }
         
      }
   
   }else{
      
      if( detID == 0){
         numCommonPeaks = 3;
         peak[0].push_back(746); peak[0].push_back(1057); peak[0].push_back(1514);
         peak[1].push_back(804); peak[1].push_back(1128); peak[1].push_back(1607);
         peak[2].push_back(604); peak[2].push_back( 860); peak[2].push_back(1240);
         peak[3].push_back(717); peak[3].push_back( 994); peak[3].push_back(1428);
      }
      
      if( detID == 2){
         numCommonPeaks = 4;
         peak[0].push_back(  -5); peak[0].push_back(553); peak[0].push_back(676); peak[0].push_back(968);
         peak[1].push_back(-112); peak[1].push_back(340); peak[1].push_back(437); peak[1].push_back(683);
         peak[2].push_back(-193); peak[2].push_back(197); peak[2].push_back(286); peak[2].push_back(496);
         peak[3].push_back( -56); peak[3].push_back(452); peak[3].push_back(560); peak[3].push_back(827);
      }
   }
  
   cScript->Update();
   // select peaks
   vector<double> * Upeak = new vector<double>[numDet];
   for( int i = 0; i < numDet; i ++){
   
   }
   
   
   
   numPeak = numCommonPeaks;   
   for( int i = 0; i < numDet; i ++){
      printf("-------------------------- g%d \n", i);
      for(int j = 0; j < numCommonPeaks ; j ++){
         printf("%d, x: %f \n", j, peak[i][j]);
      }
   }
   
   printf("======= scaling respect to g0.\n");
   
   cScript->Update();
   printf("0 for stop, 1 for continues: ");
   scanf("%d", &dummy);
   if( dummy == 0 ) return;
   
   //======== find the scaling paramter respect to d0;
   double c0[numDet], c1[numDet];
   
   TGraph ** ga = new TGraph[numDet];
   
   for( int i = 0; i < numDet; i++){
      ga[i] = new TGraph(numPeak, peak[i], peak[0] );
      ga[i]->Draw("*ap");
      ga[i]->Fit("pol1", "q");
      c0[i] = pol1->GetParameter(0);
      c1[i] = pol1->GetParameter(1);
      printf("==== %d | c0:%f, c1:%f \n", i, c0[i], c1[i]);
   }
   
   //======== scale the energy

   TH1F ** h = new TH1F[numDet];
   for( int i = 0; i < numDet; i ++){
      TString name;
      name.Form("h%d", i);
      h[i] = new TH1F(name, name , eRange[0], eRange[1], eRange[2]);
      h[i]->SetXTitle("Ex [a.u.]");
      
      TString expression;
      expression.Form("(e[%d] - %f * x[%d])*%f + %f>> h%d" , detID + 6*i , slope ,detID + 6*i , c1[i], c0[i], i);
      TString gate;
      gate.Form("e[%d] > %f + %f * x[%d] ", detID + 6*i ,cutIntep[i], cutSlope, detID + 6*i);
      
      cScript->cd(1);
      
      if( i == 0) tree->Draw(expression, gate , "");
      tree->Draw(expression, gate , "same");
   }
   
   // ======== d with correction  
   TH2F ** k = new TH2F[numDet];
   TH2F * kall = new TH2F("kall", "kall",   200, nearPos[detID]-2 , nearPos[detID]+length+2 , 200, 100 , 2000);
   for( int i = 0; i < numDet; i ++){
      TString name;
      name.Form("k%d", i);
      k[i] = new TH2F(name, name , 200, nearPos[detID]-2 , nearPos[detID]+length+2 , 200, 100 , 2000);
      k[i]->SetXTitle("pos(xf,xn)");
      k[i]->SetYTitle("e");
      
      TString expression;
      cScript->cd(2);
      expression.Form("e[%d] * %f + %f + %f * x[%d]  : x[%d] >> + kall" , detID + 6*i, c1[i], c0[i], (1.0 - c1[i])*slope, detID + 6*i, detID + 6*i);      
      tree->Draw(expression, "" , "");
      
      
      expression.Form("e[%d] * %f + %f + %f * x[%d]  : x[%d] >> k%d" , detID + 6*i, c1[i], c0[i], (1.0 - c1[i])*slope, detID + 6*i, detID + 6*i, i);
      TString gate;
      gate.Form("");
      
      cScript->cd(3);
      if( i == 0) tree->Draw(expression, gate , "");
      tree->Draw(expression, gate , "same");
   }
  
   
   //===== save correction parameter   
   cScript->Update();
   printf("0 for end, 1 for saving parameters: ");
   scanf("%d", &dummy);
   if( dummy == 0 ) return;
   if( dummy == 1 ){
      FILE * paraOut;
      TString filename;
      filename.Form("e_correction_%d.dat", detID);
      paraOut = fopen (filename.Data(), "w+");
      
      printf("=========== save parameters to %s \n", filename.Data());
      for( int i = 0; i < numDet; i++){
         fprintf(paraOut, "%9.6f  %9.6f\n", c0[i], c1[i]);
      }
      fprintf(paraOut, "%9.6f\n", slope);
      
      fflush(paraOut);
      fclose(paraOut);
   }
   /**/
}

