{
   gStyle->SetOptStat(1111111111111111);
   
   const char* rootfile="H052_Mg25.root";
   
	//###########################  load tree
   TFile *f0 = new TFile (rootfile, "read"); 
   TTree *tree = (TTree*)f0->Get("gen_tree");
   printf("=====> /// %15s //// is loaded. Total #Entry: %10d \n", rootfile,  tree->GetEntries());

   

/**///======================================================== Browser or Canvas

   //TBrowser B ;
   
   Int_t Div[2] = {1,1};  //x,y
   Int_t size[2] = {500,500}; //x,y
   TCanvas * cPostAna = new TCanvas("cScript", "cScript", 0, 0, size[0]*Div[0], size[1]*Div[1]);
   cPostAna->Divide(Div[0],Div[1]);
   
/**///========================================================= Analysis

   //======== create histogram for each detector
   const int index = 24;
   TH2F ** d = new TH2F[index];
   for( int i = 0; i < index; i ++){
      TString name;
      name.Form("d%d", i);
      d[i] = new TH2F(name, name , 300, -100 , 3500 , 300, -100 , 3500);
      d[i]->SetXTitle("xf+xn");
      d[i]->SetYTitle("e");
      
      TString expression;
      expression.Form("e[%d]:(xf[%d]+xn[%d])>> d%d" , i,i,i,i);
      TString gate;
      gate.Form("e[%d]!=0 && xf[%d]!=0 && xn[%d]!=0",i,i,i);
      //gate.Form("xf[%d]!=0 && xn[%d]!=0",i,i);
      gen_tree->Draw(expression, gate , "");
      
      //printf("========  i = %d \n", i); 
   }
   
   //======== profileX
   Double_t* slope = new Double_t[index];
   Double_t* intep = new Double_t[index];
   
   TF1 *fit = new TF1("fit", "pol1"); 
      
   for( int i = 0; i < index; i ++){
      d[i]->ProfileX("d_prx");
      
      printf("===================== d%i \n", i);
      
      //fit
      d_prx->Fit("fit");
      
      slope[i] = fit->GetParameter(1);
      intep[i] = fit->GetParameter(0);
      
   }
   
   //===== save correction parameter
   FILE * paraOut;
   paraOut = fopen ("xf_xn_correction_para.dat", "w+");
   
   for( int i = 0; i < index; i++){
      fprintf(paraOut, "%9.6f  %9.6f\n", intep[i], slope[i]);
   }
   
   fflush(paraOut);
   fclose(paraOut);
   
   /*
   // make correction
   TH2F ** dc = new TH2F[index];
   for( int i = 0; i < index; i ++){
      TString name;
      name.Form("dc%d", i);
      dc[i] = new TH2F(name, name , 300, -100 , 3500 , 300, -100 , 3500);
      dc[i]->SetXTitle("xf+xn");
      dc[i]->SetYTitle("e");
      
      TString expression;
      expression.Form("e[%d]:%f*(xf[%d]+xn[%d])+%f>> dc%d" , i, slope[i] , i,i, intep[i], i);
      TString gate;
      gate.Form("e[%d]!=0 && xf[%d]!=0 && xn[%d]!=0",i,i,i);
      //gate.Form("xf[%d]!=0 && xn[%d]!=0",i,i);
      gen_tree->Draw(expression, gate , "");
      
      //printf("========  i = %d \n", i); 
   }
   */

}

