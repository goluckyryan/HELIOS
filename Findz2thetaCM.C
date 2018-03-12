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
   
/**///========================================================= Analysis
   int numEx = 8;
   
   tree->Draw("Ex >> hEx");
   
   // get Ex energy
   TSpectrum * spec = new TSpectrum(numEx+2);
   spec->Search(hEx);
   float * xpos = spec->GetPositionX();
   //Sort
   
   int * inX = new int[numEx];
   TMath::Sort(numEx, xpos, inX, 0 );  
   vector<double> energy;   
   for( int j = 0; j < numEx; j++){
      //printf(" %d , x: %8.3f \n", j, xpos[inX[j]]);
      energy.push_back(xpos[inX[j]]);
   }


   // plotting cos(thetaCM) vs z
   TH2F * k = new TH2F("k", "k", 400, -600, -200, 400, 0.65, 1.0); 
   
   double c0[numEx], c1[numEx];
   
   for(int ExID = 0; ExID < numEx ; ExID ++){
      
      TString gate;
      gate.Form("tag == 2 && rho > 100 &&  ExID == %d", ExID);
      
      tree->Draw("TMath::Cos(thetaCM):z >> k", gate, "")   ;
      
      // fit pol1
      k->ProfileX("kpx");
      kpx->Fit("pol1", "q");
      
      c0[ExID] = pol1->GetParameter(0);
      c1[ExID] = pol1->GetParameter(1);
      
      printf("%d, Ex:%f -- c[0]: %9.6f, c[1]: %20.15f \n", ExID, energy[ExID], c0[ExID], c1[ExID]); 
      
      //cScript->Update();
      //printf("press 1 for continous.");
      //int dummy;
      //scanf("%d", &dummy);
      
   }
   
   // Find the energy vs c0, energy vs c1;
   TGraph * g0 = new TGraph(numEx, &energy[0], &c0[0] );
   g0->Draw("*ap");
   g0->Fit("pol2", "q");
   double e00 = pol2->GetParameter(0);
   double e01 = pol2->GetParameter(1);
   double e02 = pol2->GetParameter(2);
   printf("====  e00:%12.6f, e01:%12.9f, e02:%12.9f \n", e00, e01, e02);
   
   TF1 * e2c0 = new TF1("e2c0", "[0] + [1]*x + [2]*x*x", -10, 20);
   e2c0->SetParameter(0, e00);
   e2c0->SetParameter(1, e01);
   e2c0->SetParameter(2, e02);

   TGraph * g1 = new TGraph(numEx, &energy[0], &c1[0] );
   g1->Draw("*ap");
   g1->Fit("pol2", "q");
   double e10 = pol2->GetParameter(0);
   double e11 = pol2->GetParameter(1);
   double e12 = pol2->GetParameter(2);
   printf("====  e10:%12.6f, e11:%12.9f, e12:%12.9f \n", e10, e11, e12);
   
   TF1 * e2c1 = new TF1("e2c1", "[0] + [1]*x + [2]*x*x", -10, 20);
   e2c1->SetParameter(0, e10);
   e2c1->SetParameter(1, e11);
   e2c1->SetParameter(2, e12);
   
   tree->Draw("TMath::Cos(thetaCM):z >> ezt", "tag==2 && rho > 100");
   
   // for Ex = 0;
   double ex = energy[3];
   TF1 * line = new TF1("line", "[0] + [1]*x", -600, -200);
   double p0 = e2c0->Eval(ex);
   line->SetParameter(0, p0);
   double p1 = e2c1->Eval(ex);
   line->SetParameter(1, p1);
   
   line->Draw("same");
}

