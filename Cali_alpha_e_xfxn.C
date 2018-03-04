//#include "TFile"

//void Cali_alpha_e_xfxn()
{  

   Int_t Div[2] = {6,4};  //x,y
   Int_t size[2] = {230,230}; //x,y
   TCanvas * cAlpha = new TCanvas("cAlpha", "cAlpha", 0, 0, size[0]*Div[0], size[1]*Div[1]);
   cAlpha->Divide(Div[0],Div[1]);
   cAlpha->cd(1);

   gStyle->SetOptStat(1111111);
   gStyle->SetStatY(1.0);
   gStyle->SetStatX(0.99);
   gStyle->SetStatW(0.2);
   gStyle->SetStatH(0.1);
   
   int numDet = 24;
 
   //############################################################ for e vs xf+xn correction 
   printf("############## e - xf+xn correction \n");
   TLine line;
   line.SetX1(-200);
   line.SetY1(-200);
   line.SetX2(2000);
   line.SetY2(2000);
   
   TH2F ** ex = new TH2F[numDet];
   TString * gate = new TString[numDet];
   for( int i = 0; i < numDet; i ++){
      TString name;
      name.Form("ex%d", i);
      ex[i] = new TH2F(name, name, 100, -100, 1800, 100, -100, 1800);
      name.Form("e[%d]", i); ex[i]->SetYTitle(name);
      name.Form("xf[%d]+xn[%d]", i,i); ex[i]->SetXTitle(name);
      
      TString expression;
      expression.Form("e[%d] : xf[%d]+xn[%d]>> ex%d" ,i, i, i, i);
      //gate[i].Form("xf[%d]!=0 && xn[%d]!=0 && e[%d]!=0", i, i, i);
      
      cAlpha->cd(i+1);
      
      tree->Draw(expression, gate[i] , "");
      
      line.Draw("same");
   }
   
   //-------- 1, pause for saving Canvas
   cAlpha->Update();
   printf("0 for stop, 1 for save Canvas, 2 for continuous : ");
   int dummy;
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
      expression.Form("e[%d] : (xf[%d]+xn[%d])*%f + %f>> exC%d" ,i ,i, i, exfxn[i][1], exfxn[i][0], i);
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

