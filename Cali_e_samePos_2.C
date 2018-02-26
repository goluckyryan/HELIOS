{   
/**///======================================================== initial input
   
   //const char* rootfile="psd_run38.root"; const char* treeName="psd_tree";
   //const char* rootfile="H052_Mg25.root"; const char* treeName="gen_tree";
   
   const char* rootfile="C_H052_Mg25.root"; const char* treeName="tree";
   
   const int numDet = 4;
   int detID = 1;
   double xCut = 95;
   
   
/**///========================================================  load tree

   TFile *f0 = new TFile (rootfile, "read"); 
   TTree *tree = (TTree*)f0->Get(treeName);
   printf("=====> /// %15s //// is loaded. Total #Entry: %10d \n", rootfile,  tree->GetEntries());
   
/**///======================================================== Browser or Canvas

   Int_t Div[2] = {2,2};  //x,y
   Int_t size[2] = {400,400}; //x,y
   TCanvas * cScript = new TCanvas("cScript", "cScript", 0, 0, size[0]*Div[0], size[1]*Div[1]);
   cScript->Divide(Div[0],Div[1]);
   for( int i = 1; i <= Div[0]+Div[1] ; i++){
      cScript->cd(i)->SetGrid();
   }
   cScript->cd(1);

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

   TH1F ** h = new TH1F[numDet];
   TString * gate = new TString[numDet];
   TSpectrum ** s = new TSpectrum[numDet];
   Float_t ** xpos = new Float_t[numDet];
   for( int i = 0; i < numDet ; i++){
      s[i] = new TSpectrum(10);
   }
   
   
   int numCommonPeaks = 10;
   
   for( int i = 0; i < numDet; i ++){
      TString name;
      name.Form("h%d", i);
      h[i] = new TH1F(name, name, 100, 0, 2000);
      name.Form("e[%d]", i); h[i]->SetXTitle(name);
      
      TString expression;
      expression.Form("e[%d] >> h%d" ,detID + 6*i ,i);
      gate[i].Form("x[%d] > %f", detID + 6*i, xCut);
      cScript->cd(i+1);
      tree->Draw(expression, gate[i] , "");
      
      int nPeaks = s[i]->Search(h[i], 1,"", 0.2);
      
      if( nPeaks < numCommonPeaks ) numCommonPeaks = nPeaks;
      
      xpos[i] = s[i]->GetPositionX();
      
      printf("------------------ %d \n", i);
      for(int j = 0; j < nPeaks ; j ++){
         printf("%d, x: %f \n", j, xpos[i][j]);
      }
      
   }
   
   // pause
   cScript->Update();
   printf("0 for stop, 1 for continous : ");
   int dummy;
   scanf("%d", &dummy);
   if( dummy == 0 ) return;

     
   //======== find the scaling paramter respect to d0;
   double c0[numDet], c1[numDet];
   
   // sorting
   double ** peak = new double[numDet];
   for ( int i = 0 ; i < numDet; i ++){
      int index[numCommonPeaks];
      peak[i] = new double[numCommonPeaks];
      TMath::Sort(numCommonPeaks, xpos[i], index, 0 );
      for( int j = 0; j < numCommonPeaks; j++){
         peak[i][j] = xpos[i][index[j]];
      }
   }
   
   for( int i = 0; i < numDet; i ++){
      printf("-------------------------- %d \n", i);
      for(int j = 0; j < numCommonPeaks ; j ++){
         printf("%d, x: %f \n", j, peak[i][j]);
      }
   }
   
   TGraph ** ga = new TGraph[numDet];
   
   for( int i = 0; i < numDet; i++){
      ga[i] = new TGraph(numCommonPeaks, peak[i], peak[0] );
      //ga[i] = new TGraph(numCommonPeaks, xpos[i], xpos[0] );
      ga[i]->Draw("*ap");
      ga[i]->Fit("pol1", "q");
      c0[i] = pol1->GetParameter(0);
      c1[i] = pol1->GetParameter(1);
      printf("==== %d | c0:%f, c1:%f \n", i, c0[i], c1[i]);
   }
   
   //======== scale the energy
   
   cScript->cd(2);

   TH1F ** d = new TH1F[numDet];
   for( int i = 0; i < numDet; i ++){
      TString name;
      name.Form("d%d", i);
      d[i] = new TH1F(name, name , 100, 0, 2000);
      d[i]->SetLineColor(i+1);
      d[i]->SetXTitle("Ex [a.u.]");
      
      TString expression;
      expression.Form("e[%d]*%f + %f>> d%d" , detID + 6*i , c1[i], c0[i], i);
      gate[i].Form("x[%d] > %f", detID + 6*i, xCut);
      if( i == 0) tree->Draw(expression, gate[i] , "");
      tree->Draw(expression, gate[i] , "same");
   }
   
   cScript->cd(1);
   
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
      expression.Form("e[%d] * %f + %f : x[%d] >> + kall" , detID + 6*i, c1[i], c0[i], detID + 6*i, i);
      tree->Draw(expression, "", "");
      
      
      expression.Form("e[%d] * %f + %f : x[%d] >> k%d" , detID + 6*i, c1[i], c0[i], detID + 6*i, i);
      if( i == 0) tree->Draw(expression, "" , "");
      tree->Draw(expression, "", "same");
      

   }
   
   cScript->cd(4);
   kall->Draw("colz");
   
   
   
   //===== save correction parameter   
   cScript->Update();
   printf("0 for stop, 1 for saving parameters : ");
   scanf("%d", &dummy);
   if( dummy == 0 ) {
      return;
   }else{
      FILE * paraOut;
      TString filename;
      filename.Form("e_correction_%d.dat", detID);
      paraOut = fopen (filename.Data(), "w+");
      
      printf("=========== save parameters to %s \n", filename.Data());
      for( int i = 0; i < numDet; i++){
         fprintf(paraOut, "%9.6f  %9.6f\n", c0[i], c1[i]);
      }
      
      fflush(paraOut);
      fclose(paraOut);
   }
}
