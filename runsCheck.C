{
   TString rootFile;
   
   TFile * f = NULL;
   TTree * tree = NULL;
   
   const char* treeName="gen_tree";
   
   ULong64_t e_t[100];
   TBranch  *b_EnergyTimestamp;
   
   double time1, time2, dTime;
   ULong64_t firstTime, lastTime;
   
   FILE * paraOut;
   TString filename;
   filename.Form("run_Summary.dat");
   paraOut = fopen (filename.Data(), "w+");
   
   for( int i = 1; i <= 46 ; i++){
      rootFile.Form("~/ANALYSIS/RUNs/H060_208Pb/gen_run%02d.root", i);
      
      f = new TFile (rootFile, "read"); 
      tree = (TTree*)f->Get(treeName);

      tree->SetBranchAddress("e_t", e_t, &b_EnergyTimestamp);
      
      int totalEvent = tree->GetEntries();
      
      
      time1 = 0;
      time2 = 0;
      dTime = 0;
      firstTime = 0;
      lastTime = 0;
      
      //find the first event has time_recored
      
      bool breakFlag = false;
      for(int event = 1; event < totalEvent; event++){
         tree->GetEntry(event);
         for(int j = 0; j < 24; j++){
            if( e_t[j] > 0 ) {
               firstTime = e_t[j];
               //printf("%d ", event);
               breakFlag = true;
               break;
            }
         }
         if( breakFlag ) break;      
      }
      
      //find the last event time_recored
      breakFlag = false;
      for(int event = totalEvent-1; event > 0; event--){
         tree->GetEntry(event);
         for(int j = 0; j < 24; j++){
            if( e_t[j] > 0 ) {
               lastTime = e_t[j];
               //printf(", %d \n", event);
               breakFlag = true;
               break;
            }
         }
         if( breakFlag ) break;      
      }
      
      
      time1 = firstTime/1e8;
      time2 = lastTime/1e8;
      dTime = (lastTime - firstTime)/1e8;
      //printf("%11f sec, %11f sec, %11f sec \n", time1, time2, dTime);
      
      double size = f->GetSize(); // in byte
      printf("=====> /// %15s //// is loaded. Total #Entry: %10d, size: %6.1f MB, duration: %10.2f sec = %7.2f min = %7.2f hr\n", 
               rootFile.Data(),  totalEvent, size/1024/1024, dTime, dTime/60., dTime/60./60.);
      
      
      if( i == 1 ) {
         fprintf(paraOut, "%50s, %10s, %8s, %11s\n", "file",  "#event", "size[MB]", "duration[s]");   
      
      }   
      fprintf(paraOut, "%50s, %10d, %8.2f, %11.3f\n",rootFile.Data(),  totalEvent, size/1024/1024, dTime);   
      fflush(paraOut);   
   }
      
   fclose(paraOut);
   
   printf("=========== saved summery to %s.\n", filename.Data());
   
}
