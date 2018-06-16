//void Script(){   
{

/**///======================================================== initial input
   
   //const char* rootfile="psd_run38.root"; const char* treeName="psd_tree";
   //const char* rootfile="H052_Mg25.root"; const char* treeName="gen_tree";
   //const char* rootfile="X_H052_Mg25.root"; const char* treeName="tree";
   const char* rootfile="test_3.root"; const char* treeName="tree";
   
   const string detGeoFileName = "detectorGeo_upstream.txt";
   bool isLoadDetGeo = false;
   
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
   double Bfield, bore;
   double a, w, l, support, firstPos;
   double posRecoil, rhoRecoil;
   vector<double> pos;
   int nDet;
   
   if( isLoadDetGeo ){
      printf("----- loading detector geometery : %s.", detGeoFileName.c_str());
      ifstream file;
      file.open(detGeoFileName.c_str());
      string line;
      int i = 0;
      if( file.is_open() ){
         string x;
         while( file >> x){
            //printf("%d, %s \n", i,  x.c_str());
            if( x.substr(0,2) == "//" )  continue;
            
            if( i == 0 ) Bfield = atof(x.c_str());
            if( i == 1 ) bore = atof(x.c_str());
            if( i == 2 ) a    = atof(x.c_str());
            if( i == 3 ) w    = atof(x.c_str());
            if( i == 4 ) posRecoil = atof(x.c_str());
            if( i == 5 ) rhoRecoil   = atof(x.c_str());
            if( i == 6 ) l    = atof(x.c_str());
            if( i == 7 ) support = atof(x.c_str());
            if( i == 8 ) firstPos = atof(x.c_str());
            if( i >= 9 ) {
               pos.push_back(atof(x.c_str()));
            }
            i = i + 1;
         }
         
         nDet = pos.size();
         file.close();
         printf("... done.\n");
         
         for(int id = 0; id < nDet; id++){
            pos[id] = firstPos + pos[id];
         }
         
         printf("========== B-field: %6.2f T \n", Bfield);
         printf("========== Recoil detector: %6.2f mm, radius: %6.2f mm \n", posRecoil, rhoRecoil);
         printf("========== gap of multi-loop: %6.2f mm \n", firstPos > 0 ? firstPos - support : firstPos + support );
         for(int i = 0; i < nDet ; i++){
            if( firstPos > 0 ){
               printf("%d, %6.2f mm - %6.2f mm \n", i, pos[i], pos[i] + l);
            }else{
               printf("%d, %6.2f mm - %6.2f mm \n", i, pos[i] - l , pos[i]);
            }
         }
         printf("=======================\n");
         
      }else{
          printf("... fail\n");
          
      }
   }
   
/**///========================================================= Analysis
   
   
}

