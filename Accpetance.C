{   
/**///======================================================== initial input
   
   const char* rootfile="test.root"; const char* treeName="tree";
   
/**///========================================================  load tree

   TFile *f0 = new TFile (rootfile, "read"); 
   TTree *tree = (TTree*)f0->Get(treeName);
   printf("=====> /// %15s //// is loaded. Total #Entry: %10d \n", rootfile,  tree->GetEntries());
   
/**///======================================================== Browser or Canvas

   //TBrowser B ;   
   Int_t Div[2] = {1,1};  //x,y
   Int_t size[2] = {800,600}; //x,y
   
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
   double l = 50.5; // length
   double support = 43.5;
   double firstPos = 218.5; // m 
   double pos[6] = {0. , 59., 118., 176.9, 236.2, 294.8}; // near position in m
/**///========================================================= Analysis
   
   
   const int polN = 6;
   
   TString expression, gate, gate_aux, gate_e;
   
   gate = "tag == 2 && TMath::Cos(thetaCM) < 0.985";
   
   expression.Form("thetaCM*TMath::RadToDeg() : Ex >> p");
   
   TH2F * p = new TH2F("p","p", 220, -1, 10, 200, 0, 50);
   tree->Draw(expression, gate, "colz");
     
   /*
   
   double posZ[12];
   for(int i = 0; i < 6 ; i++){
      posZ[2*i]   = firstPos + pos[i];
      posZ[2*i+1] = firstPos + pos[i] + l ;
   }
   
   TH2F * p = new TH2F("p","p", 200, 0, 8, 200, 5, 50);
   
   double c[12][polN+1];
   
   for(int i = 0; i < 12 ; i++){
      
      printf("========= %2d | ", i);
      
      p->Reset();
      
      gate_aux.Form("&& TMath::Abs(z + %f) < 1", posZ[i]);
      
      tree->Draw(expression, gate + gate_aux);
      
      p->ProfileX("ppx");
      TString polname;
      polname.Form("pol%d", polN);
      ppx->Fit("pol6", "q");
      
      for(int j = 0; j <= polN ; j++){
         c[i][j] = pol6->GetParameter(j);
         printf("%10.7f, ", c[i][j]);
      } 
      
      printf("\n");
      
      //printf("thetaCM = %10.7f + %10.7f Ex + %10.7f Ex^2 + %10.7f Ex^3 + %10.7f Ex^4 + %10.7f Ex^5 + %10.7f Ex^5\n", c[i][0], c[i][1], c[i][2], c[i][3], c[i][4], c[i][5], c[i][6]);
   
   } 
   /*
   //===== save correction parameter
   FILE * paraOut;
   paraOut = fopen ("acceptance.dat", "w+");
   
   for( int i = 0; i < 12; i++){
      for( int j = 0; j <= polN; j++){
         fprintf(paraOut, "%10.7f\t", c[i][j]);
      }
      fprintf(paraOut, "\n");
   }
   
   fflush(paraOut);
   fclose(paraOut);
   
   */
   
   // for histogram for each Ex
   TH1F** h = new TH1F*[14];
   for(int i = 0 ; i < 1 ; i++){
      TString name;
      name.Form("h%d", i);
      h[i] = new TH1F(name, name, 450, 5, 50);
      
      expression.Form("thetaCM*TMath::RadToDeg() >> h%d", i);
      
      gate_e.Form("&& TMath::Abs(Ex-%d) < 0.1", i);
      
      tree->Draw(expression, gate + gate_e);
   }
   
   //find the acceptance, the angle the count drop
   vector<double> angle;
   for( int i = 0; i < 1; i++){
      printf(" ============================ h%d\n", i);
      angle.clear();
      for(int j = 1 ; j < 4500; j++){
         int a = h[i]->GetBinContent(j);
         int b = h[i]->GetBinContent(j+1);
         
         if( a == 0 && b > 0) {
            angle.push_back(h[i]->GetBinLowEdge(j+1));
            //printf(" boundary : %10.5f\n", h[i]->GetBinLowEdge(j+1) );
         }
         if( a > 0 && b == 0) {
            angle.push_back(h[i]->GetBinLowEdge(j+1));
            //printf(" boundary : %10.5f\n", h[i]->GetBinLowEdge(j+1) );
         }
      }
      
      for( int j = 1; j < angle.size()/2; j++){
         double delta = angle[2*j+1] - angle[2*j];
         double mean = (angle[2*j+1] + angle[2*j])/2;
         printf("%2d | %10.5f - %10.5f = %10.5f | %10.5f, %10.5f \n", 
                    j, 
                    angle[2*j], 
                    angle[2*j+1], 
                    delta,
                    mean,
                    1./TMath::Sin(mean*TMath::DegToRad())/(delta*TMath::DegToRad()));
      }
   }
   
   /**/
}

