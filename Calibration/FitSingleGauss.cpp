double * FitSingleGauss(TH1F * h0, int xMin = 1000){

   //find maximum;
   double yMax = h0->GetMaximum();
   int iMax = h0->GetMaximumBin();
   double xMax = h0->GetBinCenter(iMax);
   if( xMax < xMin ){
      yMax = h0->GetMaximum(yMax - 10);
   }
   
   double i1 = h0->FindFirstBinAbove(yMax/2.);
   double i2 = h0->FindLastBinAbove(yMax/2.);

   double x1 = h0->GetBinCenter(i1);
   double x2 = h0->GetBinCenter(i2);
   
   double FWHM = (x2 - x1)/2;
   
   h0->Fit("gaus", "", "", xMax - 3* FWHM, xMax + 3* FWHM);
   
   double * para = new double[3];
   gaus->GetParameters(para);
   
   return para;   

}
