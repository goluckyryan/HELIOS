{
/**///======================================================== initial input
   
   //const char* rootfile="psd_run38.root"; const char* treeName="psd_tree";
   //const char* rootfile="H052_Mg25.root"; const char* treeName="gen_tree";
   
   const char* rootfile="C_H052_Mg25.root"; const char* treeName="tree";
   
   const int numDet = 6;
   int eRange[3] = {150, -1000, 1000};

   double cutSlope = -2.35;
   double cutIntep[numDet] = {900, 880, 1015, 890, 830, 800}; 
   double slope = 4.75;
   double yIntep = 700;
   int numPeak = 3; 
   
   double ** peak = new double[numDet];
   for(int i = 0; i < 4; i++){
      peak[i] = new double[numPeak];
   }
   
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
   
/**///========================================================= Analysis

   TString name;
   name.Form("b");
   TH2F * b = new TH2F(name, name , 200, nearPos[0]-2 , nearPos[5]+length+2 , 200, -100 , 2000);
   b->SetXTitle("pos(xf,xn)");
   b->SetYTitle("e");
   
   TF1 * line = new TF1("cutLine", "[0] + x* [1]", nearPos[0]-2 , nearPos[5]+length+2);
   line->SetParameter(1, cutSlope);
   line->SetParameter(0, cutIntep[0]);
   line->SetLineColor(2);

   TF1 * eline = new TF1("eline", "[0] + x* [1] + x*x*[2]", nearPos[0]-2 , nearPos[5]+length+2);
   eline->SetParameter(2, -0.0025);
   eline->SetParameter(1, 4.88107);
   eline->SetParameter(0, -100);
   eline->SetLineColor(4);

   int dummy = 0;
   do{
      b->Reset();
      printf("==== drawing e vs x plot, wait\n");
      for( int i = 0; i < numDet; i ++){  
         printf(" %d : %8.3f\n", i, cutIntep[i]);
         TString expression;
         expression.Form("e[%d] *%f:x[%d]>> + b" , i, cutIntep[0]/cutIntep[i] , i );
         TString gate;
         gate.Form("");
         
         tree->Draw(expression, gate , "");
      }
      line->Draw("same");
      eline->Draw("same");
      
      cScript->Update();
      
      int detID;
      float newPos;
      printf(" which detector to adjust and new position? " );
      scanf("%d, %f", &detID, &newPos);
      printf(" %d, newPos : %f \n", detID, newPos);
      if( detID > 5) break;
      cutIntep[detID] = newPos;
      
   }while(detID <= 5);
   

   // pause
   cScript->Update();
   //printf("0 for stop, 1 for continous : ");
   float a2 , a1, a0;
   do{
      printf("new a2 (%f), a1 (%f), a0 (%f) : ", eline->GetParameter(2) , eline->GetParameter(1) , eline->GetParameter(0) );
      std::cin >> a2;
      if( a2 > 0. ) {
         printf(" exit do loop. \n");
         break;
      }
      std::cin >> a1;
      std::cin >> a0;
      //scanf("%1f", &a2);
      //scanf("%1f", &a0);

      printf("your input : %f, %f, %f\n", a2, a1, a0);
      eline->SetParameter(2, a2);
      eline->SetParameter(1, a1);
      eline->SetParameter(0, a0);
      eline->Draw("same");
      cScript->Update();

   }while(a2 < 0.);
   
   //======== create 2-D histogram of projection
   TH2F * p = new TH2F("p", "p" ,200, nearPos[0]-2 , nearPos[5]+length+2, eRange[0], eRange[1], eRange[2]);
      
   for( int i = 0; i < numDet-1; i ++){
      TString expression;
      expression.Form("e[%d] *%f - %f*x[%d]*x[%d] - %f*x[%d] : x[%d] >> + p" ,i ,cutIntep[0]/cutIntep[i], eline->GetParameter(2), i, i ,eline->GetParameter(1) ,i ,i);
      TString gate;
      //gate.Form("e[%d] > %f + %f * x[%d] ", i, cutIntep[i], cutSlope, i);
      //gate.Form("e[%d]*%f > %f + %f * x[%d] ", i, cutIntep[0]/cutIntep[i], line->GetParameter(0), line->GetParameter(1), i);
      gate.Form("");
      tree->Draw(expression, gate , "");
   }
   
      /*
   //pause
   cScript->Update();
   printf("0 for stop, 1 for projection : ");
   scanf("%d", &dummy);
   if( dummy == 0 ) return;
   
   //======== create 1-D histogram of projection
   TH1F * g = new TH1F("g", "g" , eRange[0], eRange[1], eRange[2]);
   for( int i = 0; i < numDet; i ++){
      
      TString expression;
      expression.Form("e[%d] *%f - %f*x[%d]*x[%d] - %f*x[%d] >> + g" ,i ,cutIntep[0]/cutIntep[i], eline->GetParameter(2), i, i ,eline->GetParameter(1) ,i);
      
      tree->Draw(expression, "" , "");
   }
   
   /*
   //pause
   cScript->Update();
   printf("0 for stop, 1 for fit, 2 for not-fit : ");
   scanf("%d", &dummy);
   if( dummy == 0 ) return;
   
   
   if( dummy == 1){
      //======= find peaks using guassian fit  
      for( int i = 0; i < numDet; i ++){
         for(int j = 0; j < numPeak; j++){
            
         
            g[i]->Fit("gaus", "", "", fitRange[2*j], fitRange[2*j+1]);
            peak[i][j] = gaus->GetParameter(1);
         }
      }
   }
   
   //======== find the scaling paramter respect to d0;
   double c0[numDet], c1[numDet];
   
   TGraph ** ga = new TGraph[numDet];
   
   for( int i = 0; i < numDet; i++){
      ga[i] = new TGraph(numPeak, peak[i], peak[0] );
      ga[i]->Draw("*ap");
      ga[i]->Fit("pol1");
      c0[i] = pol1->GetParameter(0);
      c1[i] = pol1->GetParameter(1);
      printf("==== %d | c0:%f, c1:%f \n", i, c0[i], c1[i]);
   }
   
   //======== scale the energy

   TH1F ** h = new TH1F[numDet];
   for( int i = 0; i < numDet; i ++){
      TString name;
      name.Form("h%d", i);
      h[i] = new TH1F(name, name , eRange[0], eRange[1], eRange[2]);
      h[i]->SetXTitle("Ex [a.u.]");
      
      TString expression;
      expression.Form("(e[%d] - %f * x[%d])*%f + %f>> h%d" , detID + 6*i , slope ,detID + 6*i , c1[i], c0[i], i);
      TString gate;
      gate.Form("e[%d] > %f + %f * x[%d] ", detID + 6*i ,cutIntep[i], cutSlope, detID + 6*i);
      
      cScript->cd(1);
      
      if( i == 0) tree->Draw(expression, gate , "");
      tree->Draw(expression, gate , "same");
   }
   
   // ======== d with correction  
   TH2F ** k = new TH2F[numDet];
   for( int i = 0; i < numDet; i ++){
      TString name;
      name.Form("k%d", i);
      k[i] = new TH2F(name, name , 200, nearPos[detID]-2 , nearPos[detID]+length+2 , 200, 100 , 2000);
      k[i]->SetXTitle("pos(xf,xn)");
      k[i]->SetYTitle("e");
      
      TString expression;
      //expression.Form("(e%d - %f * x%d) * %f + %f:x%d>> k%d" , i, slope, i,  c1[i], c0[i], i, i);
      //expression.Form("e%d * %f + %f  : x%d>> k%d" , i, c1[i], c0[i], i, i);
      expression.Form("e[%d] * %f + %f + %f * x[%d]  : x[%d] >> k%d" , detID + 6*i, c1[i], c0[i], (1.0 - c1[i])*slope, detID + 6*i, detID + 6*i, i);
      TString gate;
      gate.Form("");
      
      cScript->cd(2);
      if( i == 0) tree->Draw(expression, gate , "");
      tree->Draw(expression, gate , "same");
   }
  
   
   //===== save correction parameter   
   FILE * paraOut;
   TString filename;
   filename.Form("e_correction_%d.dat", detID);
   paraOut = fopen (filename.Data(), "w+");
   
   printf("=========== save parameters to %s \n", filename.Data());
   for( int i = 0; i < numDet; i++){
      fprintf(paraOut, "%9.6f  %9.6f\n", c0[i], c1[i]);
   }
   fprintf(paraOut, "%9.6f\n", slope);
   
   fflush(paraOut);
   fclose(paraOut);
   /**/
}

