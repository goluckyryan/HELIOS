{

   for( int i = 0; i < 24; i++){
      if( i == 11 ) continue;
   
      TH1F * hist = (TH1F *)gROOT->FindObjectAny(Form("hei%d", i));
      
      fitGauss(hist, 5.16, 0.03, 5.13, 5.3);
   
   }
   
}

