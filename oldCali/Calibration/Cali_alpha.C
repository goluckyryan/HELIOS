//void Cali_alpha(TString rootfile)
{
/**///======================================================== initial input
   
   const char* rootfile="~/ANALYSIS/RUNs/H060_208Pb/gen_run09.root"; 
   
   const char* treeName="gen_tree";
   
   const int numDet = 24;
   
/**///========================================================  load tree

   printf("============ calibration for PSD detectors using alpha souce. \n");
   printf("1, calibration energy using charateristic energy of alpha souce. \n");
   printf("2, calibration xf-xn with energy-gate. \n");
   //printf("3, calibration e vs xf+xn. \n");
   printf("------------------------------------------------- \n");

   TFile *f0 = new TFile (rootfile, "read"); 
   TTree *tree = (TTree*)f0->Get(treeName);
   printf("=====> /// %15s //// is loaded. Total #Entry: %10d \n", rootfile,  tree->GetEntries());
   
/**///======================================================== Browser or Canvas

   //TBrowser B ;   
   Int_t Div[2] = {6,4};  //x,y
   Int_t size[2] = {230,230}; //x,y
   TCanvas * cAlpha = new TCanvas("cAlpha", "cAlpha", 0, 0, size[0]*Div[0], size[1]*Div[1]);
   cAlpha->Divide(Div[0],Div[1]);
   
   for( int i = 1; i <= Div[0]*Div[1] ; i++){
      cAlpha->cd(i)->SetGrid();
   }

   gStyle->SetOptStat(1111111);
   gStyle->SetStatY(1.0);
   gStyle->SetStatX(0.99);
   gStyle->SetStatW(0.2);
   gStyle->SetStatH(0.1);
   
/**///========================================================= Analysis

   //############################################################ energy correction
   printf("############## e correction \n");
   TH1F ** q = new TH1F[numDet];
   TString * gate = new TString[numDet];
   for( int i = 0; i < numDet; i ++){
      TString name;
      name.Form("q%d", i);
      q[i] = new TH1F(name, name, 200, 500, 1800);
      q[i]->SetXTitle(name);
      
      TString expression;
      expression.Form("e[%d]>> q%d" ,i ,i);
      gate[i].Form("e[%d]!=0", i);
      
      cAlpha->cd(i+1);
      tree->Draw(expression, gate[i] , "");
   }
   
   //----------- 1, pause for save Canvas
   int dummy = 0;
   cAlpha->Update();
   printf("0 for stop, 1 for save Canvas, 2 for continuous : ");
   scanf("%d", &dummy);
   if( dummy == 0 ) {
      return;
   }else if(dummy == 1){
      cAlpha->SaveAs("alpha_e.pdf");
   }
   
   printf("----- finding the edge of the energy spectrum...\n");
   //----------- 2, find the edge of the energy 
   double xHalf[numDet];
   double yMax[numDet];
   for( int i = 0; i < numDet; i++){
      yMax[i] = q[i]->GetMaximum();
      int iMax = q[i]->GetMaximumBin();
      //printf("=========== %d \n", i);
      //printf(" yMax : %f, iMax : %d \n", yMax[i], iMax);
      
      // for the higher energy peak
      //if( iMax < 100 ){
      //   yMax[i] = q[i]->GetMaximum(yMax[i]/2.);
         //printf(" ---> corrected yMax : %f \n", yMax[i]);
      //}
      int iHalf = q[i]->FindLastBinAbove(yMax[i]/2);
      xHalf[i] = q[i]->GetBinCenter(iHalf);
   
      printf("%2d | iHalf : %3d, xHalf : %6f \n", i, iHalf, xHalf[i]);
   }
   
   printf("----- adjusting the energy to det-0......\n");
   //------------ 3, correction
   TH1F ** p = new TH1F[numDet];
   for( int i = 0; i < numDet; i ++){
      TString name;
      name.Form("p%d", i);
      p[i] = new TH1F(name, name, 200, 500, 1800);
      p[i]->SetXTitle(name);
      
      TString expression;
      expression.Form("e[%d] * %f >> p%d", i, xHalf[0]/xHalf[i], i);
      gate[i].Form("e[%d]!=0", i);
      cAlpha->cd(i+1);
      tree->Draw(expression, gate[i] , "");
   }
   
   
   //----------- 4, pause for saving correction parameters
   cAlpha->Update();
   printf("0 for stop, 1 for save e-correction & Canvas, 2 for xf - xn correction: ");
   scanf("%d", &dummy);
   if( dummy == 0 ) return;
   if( dummy == 1 ){
      FILE * paraOut;
      TString filename;
      filename.Form("correction_e.dat");
      paraOut = fopen (filename.Data(), "w+");
      printf("=========== save e-correction parameters to %s \n", filename.Data());
      for( int i = 0; i < numDet; i++){
         fprintf(paraOut, "%9.6f\n", xHalf[i]);
      }
      fflush(paraOut);
      fclose(paraOut);
      
      cAlpha->SaveAs("alpha_e_corrected.pdf");
   }  
   
   //############################################################  for xf-xn correction
   printf("############## xf - xn correction \n");
   TLine line(0,0,0,0);
   line.SetLineColor(4);
   
   printf("----- plotting xf vs xn with energy gate near the peak...\n");
   TH2F ** h = new TH2F[numDet];
   for( int i = 0; i < numDet; i ++){
      TString name;
      name.Form("h%d", i);
      h[i] = new TH2F(name, name, 100, -100, 1800, 100, -100, 1800);
      name.Form("xf[%d]", i); h[i]->SetYTitle(name);
      name.Form("xn[%d]", i); h[i]->SetXTitle(name);
      
      TString expression;
      expression.Form("xf[%d]:xn[%d]>> h%d" ,i ,i, i);
      gate[i].Form("xf[%d]!=0 && xn[%d]!=0 && xf[%d] + xn[%d] > 10 && TMath::Abs(e[%d]-%f)<50", i, i, i, i, i, xHalf[i]-25);
      
      cAlpha->cd(i+1);
      
      tree->Draw(expression, gate[i] , "");
      
      line.SetX2(1600);
      line.SetY1(1600);
      line.Draw("same");
   }
   
   //-------- 1, pause for saving Canvas
   cAlpha->Update();
   printf("0 for stop, 1 for save Canvas, 2 for continuous : ");
   scanf("%d", &dummy);
   if( dummy == 0 ) {
      return;
   }else if(dummy == 1){
      cAlpha->SaveAs("alpha_xf_xn.pdf");
   }
   
   printf("----- profile and obtain the fit function...\n");
   //-------- 2, profileX and get the fit function
   double para[numDet][2];
   double xnScale[numDet];
   for( int i = 0; i < numDet; i++){
      cAlpha->cd(i+1);
      h[i]->ProfileX()->Fit("pol1", "Q");
      pol1->GetParameters(para[i]);
      xnScale[i] = -para[i][1]; 
   }
   
   printf("----- correcting...\n");
   //--------- 3, correction
   TH2F ** k = new TH2F[numDet];
   for( int i = 0; i < numDet; i ++){
      TString name;
      name.Form("k%d", i);
      k[i] = new TH2F(name, name, 100, -100, 1800, 100, -100, 1800);
      name.Form("xf[%d]", i); k[i]->SetYTitle(name);
      name.Form("xn[%d]", i); k[i]->SetXTitle(name);
      
      TString expression;
      expression.Form("xf[%d]:xn[%d]*%f>> k%d" ,i ,i, xnScale[i], i);
      gate[i].Form("xf[%d]!=0 && xn[%d]!=0", i, i);
      
      cAlpha->cd(i+1);
      
      tree->Draw(expression, gate[i] , "");
      line.SetX2(para[i][0]);
      line.SetY1(para[i][0]);
      line.Draw("same");
   }
   
   //--------- 4, pause for saving correction parameter
   cAlpha->Update();
   printf("0 for stop, 1 for save xf-xn-correction & Canvas, 2 for e - xf+xn correction: ");
   //printf("0 for stop, 1 for save xf-xn-correction & Canvas: ");
   scanf("%d", &dummy);
   if( dummy == 0 ) return;
   if( dummy == 1 ){   
      FILE * paraOut;
      TString filename;
      filename.Form("correction_xf_xn.dat");
      paraOut = fopen (filename.Data(), "w+");
      printf("=========== save xf_xn-correction parameters to %s \n", filename.Data());
      for( int i = 0; i < numDet; i++){
         fprintf(paraOut, "%9.6f\n", -para[i][1]);
      }
      fflush(paraOut);
      fclose(paraOut);
      
      cAlpha->SaveAs("alpha_xf_xn_corrected.pdf");
   }
   
   return;
   
   //############################################################ for e vs xf+xn correction
   //#### alpha calibration does not fit for physical run
   printf("############## e - xf+xn correction \n");
   line.SetX1(-200);
   line.SetY1(-200);
   line.SetX2(2000);
   line.SetY2(2000);
   
   TH2F ** ex = new TH2F[numDet];
   for( int i = 0; i < numDet; i ++){
      TString name;
      name.Form("ex%d", i);
      ex[i] = new TH2F(name, name, 100, -100, 1800, 100, -100, 1800);
      name.Form("e[%d]", i); ex[i]->SetYTitle(name);
      name.Form("xf[%d]+xn[%d]", i,i); ex[i]->SetXTitle(name);
      
      TString expression;
      expression.Form("e[%d]*%f : xf[%d]+xn[%d]*%f>> ex%d" ,i ,xHalf[0]/xHalf[i] ,i, i, xnScale[i], i);
      gate[i].Form("xf[%d]!=0 && xn[%d]!=0 && e[%d]!=0", i, i, i);
      
      cAlpha->cd(i+1);
      
      tree->Draw(expression, gate[i] , "");
      
      line.Draw("same");
   }
   
   //-------- 1, pause for saving Canvas
   cAlpha->Update();
   printf("0 for stop, 1 for save Canvas, 2 for continuous : ");
   scanf("%d", &dummy);
   if( dummy == 0 ) {
      return;
   }else if(dummy == 1){
      cAlpha->SaveAs("alpha_e_xf+xn.pdf");
   }
   
   //-------- 2, profileX and get the fit function
   double exfxn[numDet][2];
   for( int i = 0; i < numDet; i++){
      cAlpha->cd(i+1);
      ex[i]->ProfileX()->Fit("pol1", "Q");
      pol1->GetParameters(exfxn[i]); 
   }
   
   //--------- 3, correction
   TH2F ** exC = new TH2F[numDet];
   for( int i = 0; i < numDet; i ++){
      TString name;
      name.Form("exC%d", i);
      exC[i] = new TH2F(name, name, 100, -100, 1800, 100, -100, 1800);
      name.Form("e[%d]", i); exC[i]->SetYTitle(name);
      name.Form("xf[%d]+xn[%d]", i, i); exC[i]->SetXTitle(name);
      
      TString expression;
      expression.Form("e[%d]*%f : (xf[%d]+xn[%d]*%f)*%f + %f>> exC%d" ,i ,xHalf[0]/xHalf[i] ,i, i, xnScale[i], exfxn[i][1], exfxn[i][0], i);
      gate[i].Form("xf[%d]!=0 && xn[%d]!=0 && e[%d]!=0", i, i, i);
      
      cAlpha->cd(i+1);
      
      tree->Draw(expression, gate[i] , "");
      line.Draw("same");
   }
   
   //--------- 4, pause for saving correction parameter
   cAlpha->Update();
   printf("0 for end, 1 for save e-xf+xn correction & Canvas: ");
   scanf("%d", &dummy);
   if( dummy == 0 ) return;
   if( dummy == 1 ){
      FILE * paraOut;
      TString filename;
      filename.Form("correction_xfxn_e.dat");
      paraOut = fopen (filename.Data(), "w+");
      printf("=========== save xfxn-e-correction parameters to %s \n", filename.Data());
      for( int i = 0; i < numDet; i++){
         fprintf(paraOut, "%9.6f\t%9.6f\n", exfxn[i][0], exfxn[i][1]);
      }
      fflush(paraOut);
      fclose(paraOut);

      cAlpha->SaveAs("alpha_e_xf+xn_correction.pdf");

   }
   
}

