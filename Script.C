{   
/**///======================================================== initial input
   
   //const char* rootfile="psd_run38.root"; const char* treeName="psd_tree";
   //const char* rootfile="H052_Mg25.root"; const char* treeName="gen_tree";
   const char* rootfile="X_H052_Mg25.root"; const char* treeName="tree";
   
   const int numDet = 4;
   
/**///========================================================  load tree

   TFile *f0 = new TFile (rootfile, "read"); 
   TTree *tree = (TTree*)f0->Get(treeName);
   printf("=====> /// %15s //// is loaded. Total #Entry: %10d \n", rootfile,  tree->GetEntries());
   
/**///======================================================== Browser or Canvas

   //TBrowser B ;   
   Int_t Div[2] = {6,4};  //x,y
   Int_t size[2] = {230,230}; //x,y
   
   TCanvas * cScript = new TCanvas("cScript", "cScript", 0, 0, size[0]*Div[0], size[1]*Div[1]);
   cScript->Divide(Div[0],Div[1]);
   for( int i = 1; i <= Div[0]*Div[1] ; i++){
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
   if( file.is_open() ){
      double a;
      int i = 0;
      while( file >> a ){
         if( i >= 7) break;
         if( i == 6) length = a;
         nearPos[i] = a;
         i = i + 1;
      }
      file.close();
      printf("... done.\n");
      for(int i = 0; i < 5 ; i++){
         printf("%6.2f mm, ", nearPos[i]);
      }
      printf("%6.2f mm || length : %6.2f mm \n", nearPos[5], length);
   }else{
       printf("... fail\n");
   }
   
   double xnCorr[24];
   printf("----- loading xf-xn correction.");
   file.open("correction_xf_xn.dat");
   if( file.is_open() ){
      double a;
      int i = 0;
      while( file >> a ){
         if( i >= numDet) break;
         xnCorr[i] = a;
         //xnCorr[i] = 1;
         i = i + 1;
      }
      
      printf("... done.\n");
   }else{
      printf("... fail.\n");
   }
   file.close();
   
   double c1[6][4];
   double c0[6][4];
   double m[6]  ;
   printf("----- loading energy calibration for same position. \n");
   for( int i = 0; i < 6; i++){
      TString filename;
      filename.Form("e_correction_%d.dat", i);
      printf("        %s", filename.Data());
      file.open(filename.Data());
      if( file.is_open() ){
         double a, b;
         int j = 0;
         while( file >> a >> b ){
            c0[i][j] = a;
            c1[i][j] = b;
            j = j + 1;
            if( j >= 4) break;
         }
         file >> a;
         m[i] = a;
         
         printf("... done.\n");
         //printf("                 c0 : %f, m : %f \n", c0[i][2], m[i]);
      }else{
         printf("... fail.\n");
      }
      file.close();
   }
   
   double p0[6];
   double p1[6];
   printf("----- loading energy calibration for different position.");
   file.open("e_correction_diff.dat");
   if( file.is_open() ){
      double a, b;
      int i = 0;
      while( file >> a >> b ){
         p0[i] = a;
         p1[i] = b;
         i = i+ 1;
      }
      file.close();
      printf("... ok.\n");
      //for(int i = 0; i < 6 ; i++){
      //   printf("                    p0: %f, p1: %f \n", p0[i], p1[i]);
      //}
   }else{
      printf("... fail.\n");
   }
   file.close();
   
   double mean[24];
   printf("----- loading tac (mean) calibration.");
   file.open("tac_correction_mean.dat");
   if( file.is_open() ){
      double a;
      int i = 0;
      while( file >> a ){
         if( i > 24) break;
         mean[i] = a;
         i = i + 1;
      }
      printf("... done.\n");
   }else{
      printf("... fail.\n");
   }
   file.close();
   
/**///========================================================= Analysis
   TString gate_et = "&& TMath::Abs(energy_t)<20";
   TString gate_e = "&& ( TMath::Abs(energy+1059)<50 || TMath::Abs(energy + 744) < 50 || TMath::Abs(energy + 464)<50 || TMath::Abs(energy + 351)<50)"; 

  
   double tcut[24][2];
   tcut[0][0] = -200 ; tcut[0][1] = 400 ;
   tcut[1][0] = -200 ; tcut[1][1] = 400 ;
   tcut[2][0] = -200 ; tcut[2][1] = 400 ;
   //tcut[3][0] = -100 ; tcut[3][1] = 400 ;
   tcut[3][0] = -600 ; tcut[3][1] = 400 ;
   tcut[4][0] =  100 ; tcut[4][1] = 500 ;
   tcut[5][0] = -400 ; tcut[5][1] = 400 ;
   
   tcut[6][0] = -200 ; tcut[6][1] = 400 ;
   tcut[7][0] = -200 ; tcut[7][1] = 400 ;
   tcut[8][0] = -200 ; tcut[8][1] = 400 ;
   //tcut[9][0] =    0 ; tcut[9][1] = 500 ;
   tcut[9][0] =  -500 ; tcut[9][1] = 500 ;
   tcut[10][0] =  100 ; tcut[10][1] = 500 ;
   tcut[11][0] = -200 ; tcut[11][1] = -200 ;
   
   tcut[12][0] = -300 ; tcut[12][1] = 400 ;
   tcut[13][0] = -300 ; tcut[13][1] = 400 ;
   tcut[14][0] = -200 ; tcut[14][1] = 400 ;
   tcut[15][0] = -200 ; tcut[15][1] = 400 ;
   tcut[16][0] =  100 ; tcut[16][1] = 500 ;
   tcut[17][0] =  100 ; tcut[17][1] = 800 ;
   
   tcut[18][0] = -200 ; tcut[18][1] = 400 ;
   tcut[19][0] = -300 ; tcut[19][1] = 400 ;
   tcut[20][0] = -200 ; tcut[20][1] = 600 ;
   //tcut[21][0] =   50 ; tcut[21][1] = 600 ;
   tcut[21][0] = -300 ; tcut[21][1] = 600 ;
   tcut[22][0] =   50 ; tcut[22][1] = 500 ;
   tcut[23][0] =  200 ; tcut[23][1] = 700 ;
   
   TString name, expression, gate;
   
   
   TH1F ** h = new TH1F*[24];
   
   for( int i = 0; i < 24; i++){
      name.Form("h%d", i);
      h[i] = new TH1F(name, name, 150, -2000, 1500); 
      h[i]->SetXTitle("Ex [a.u]");
      //h[i]->SetYTitle("t4 [15 ch/ns]");
      expression.Form("energy >> h%d", i);
      
      //expression.Form("ttt + 0.152535 * energy + 0.000 172029 * energy * energy + 7.045e-8 * energy * energy * energy:energy >> h%d", i);
      
      //expression.Form("tt + 0.155161 * energy + 0.00014525 * energy * energy + 4.99e-8 * energy * energy * energy:energy >> h%d", i);
      
      
      gate.Form("det == %d && !(%f > t4 && t4 > %f ) && TMath::Abs(t4) < 1000 && TMath::Abs(energy_t)<20", i, tcut[i][1], tcut[i][0] );
      
      cScript->cd(i+1);
      tree->Draw(expression, gate, "colz");
   
   }
   
   /**/
   
   TCanvas * cAux = new TCanvas("cAux", "cAux", 0, 0, 500, 800);   
   cAux->Divide(1,3);
   for( int i = 1; i <= 3 ; i++){
      cAux->cd(i)->SetGrid();
   }
   
   cAux->cd(1);
   
   
   TH1F * spec = new TH1F("spec", "spec" , 400, -2000, 1500);
   spec->SetXTitle("Ex [a.u.]");
   expression.Form("energy >> spec");        
   tree->Draw(expression, "TMath::Abs(energy_t)<20 && det%6 != 5 && TMath::Abs(t4) < 1000" , "");
   
   cAux->cd(2);
   TH1F * specG = new TH1F("specG", "specG" , 400, -2000, 1500);
   specG->SetXTitle("Ex [a.u.]");
   for( int i = 0 ; i < 6 ; i++){
      for(int j = 0; j < 4 ; j++){
         
         if( i == 5 ) continue;
         expression.Form("energy >> + specG");
         
         int id = i + 6*j ;
         gate.Form("det == %d && (%f > t4 && t4 > %f ) && TMath::Abs(t4) < 1000", id, tcut[id][1], tcut[id][0] );
                
         tree->Draw(expression, gate , "");
      }
   }

   
   cAux->cd(3);
   /*
   TH1F * specGb = new TH1F("specGb", "specGb" , 400, -2000, 1500);
   specGb->SetXTitle("Ex [a.u.]");
   for( int i = 0 ; i < 6 ; i++){
      for(int j = 0; j < 4 ; j++){
         
         if( i == 5 ) continue;
         expression.Form("energy >> + specGb");
         
         int id = i + 6*j ;
         gate.Form("det == %d && !(%f > t4 && t4 > %f ) && TMath::Abs(t4) < 1000", id, tcut[id][1], tcut[id][0] );
                
         tree->Draw(expression, gate , "");
      }
   }
   */
   
   TH1F * specG2 = new TH1F("specG2", "specG2" , 400, -2000, 1500);
   specG2->SetXTitle("Ex [a.u.]");
   for( int i = 0 ; i < 6 ; i++){
      for(int j = 0; j < 4 ; j++){
         
         if( i == 5 ) continue;
         expression.Form("energy >> + specG2");
         if( i >= 3){
            
            gate.Form("-200 > x[%d] && x[%d] > -600 && TMath::Abs(energy_t)<20 && det == %d && ttt > 0", i + 6*j, i + 6*j, i + 6*j);
            
            if( i == 3 && (j == 1 || j == 3) ) {
               gate.Form("-200 > x[%d] && x[%d] > -600 && TMath::Abs(energy_t)<20 && det == %d && ttt > -100", i + 6*j, i + 6*j, i + 6*j);
            }
            
         }else{
            gate.Form("-200 > x[%d] && x[%d] > -600 && TMath::Abs(energy_t)<20", i + 6*j, i + 6*j);
         }        
         tree->Draw(expression, gate , "");
      }
   }
   
   /**/
   
   /*
   TH2F * spec = new TH2F("spec", "spec" , 400, -2000, 1500, 300, -600, -200);
   spec->SetXTitle("Ex [a.u.]");
   spec->SetYTitle("z [mm]");
   for(int i = 0; i < 24; i++){
      if( i%6 == 5) continue;
      expression.Form("x[%d]: energy >> + spec", i);        
      tree->Draw(expression, "TMath::Abs(energy_t)<20 && TMath::Abs(t4) < 1000" , "");
   }
   
   spec->Draw("colz");
   
   cAux->cd(2);
   TH2F * specG = new TH2F("specG", "specG" , 400, -2000, 1500, 300, -600, -200);
   specG->SetXTitle("Ex [a.u.]");
   specG->SetYTitle("z [mm]");
   for( int i = 0 ; i < 6 ; i++){
      for(int j = 0; j < 4 ; j++){
         
         if( i == 5 ) continue;
         
         int id = i + 6*j ;
         expression.Form("x[%d]:energy >> + specG", id);
         
         gate.Form("det == %d && !(%f > t4 && t4 > %f ) && TMath::Abs(t4) < 1000 && TMath::Abs(energy_t)<20", id, tcut[id][1], tcut[id][0] );
                
         tree->Draw(expression, gate , "");
      }
   }

   specG->Draw("colz");
   
   cAux->cd(3);
   TH2F * specG2 = new TH2F("specG2", "specG2" , 400, -2000, 1500, 300, -600, -200);
   specG2->SetXTitle("Ex [a.u.]");
   specG2->SetYTitle("z [mm]");
   for( int i = 0 ; i < 6 ; i++){
      for(int j = 0; j < 4 ; j++){
         
         if( i == 5 ) continue;
         int id = i + 6*j;
         expression.Form("x[%d]:energy >> + specG2", id);
         if( i >= 3){
            
            gate.Form("-200 > x[%d] && x[%d] > -600 && TMath::Abs(energy_t)<20 && det == %d && !(ttt > 0)", i + 6*j, i + 6*j, i + 6*j);
            
            if( i == 3 && (j == 1 || j == 3) ) {
               gate.Form("-200 > x[%d] && x[%d] > -600 && TMath::Abs(energy_t)<20 && det == %d && !(ttt > -100)", i + 6*j, i + 6*j, i + 6*j);
            }
            
         }else{
            gate.Form("0 && -200 > x[%d] && x[%d] > -600 && TMath::Abs(energy_t)<20", i + 6*j, i + 6*j);
         }        
         tree->Draw(expression, gate , "");
      }
   }
   
   specG2->Draw("colz");
   /**/
}

