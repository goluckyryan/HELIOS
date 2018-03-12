//#include "TFile"

void Cali_alpha()
{
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
   
   //----------- 2, find the edge of the energy 
   double xHalf[numDet];
   double yMax[numDet];
   for( int i = 0; i < numDet; i++){
      yMax[i] = q[i]->GetMaximum();
      int iMax = q[i]->GetMaximumBin();
      //printf("=========== %d \n", i);
      //printf(" yMax : %f, iMax : %d \n", yMax[i], iMax);
      if( iMax < 100 ){
         yMax[i] = q[i]->GetMaximum(yMax[i]/2.);
         //printf(" ---> corrected yMax : %f \n", yMax[i]);
      }
      int iHalf = q[i]->FindLastBinAbove(yMax[i]/2);
      xHalf[i] = q[i]->GetBinCenter(iHalf);
   
      //printf("%d | iHalf : %d, xHalf : %f \n", i, iHalf, xHalf[i]);
   }
   
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
   printf("0 for stop, 1 for save e-correction & Canvas, 2 for continuous : ");
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
   
}

