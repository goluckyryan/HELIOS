{

   TFile * f1 = new TFile ("trace.root", "read");
   TTree * tree = (TTree *) f1->Get("tree");
   
	int totnumEntry = tree->GetEntries();
   printf( "========== total Entry : %d \n", totnumEntry);

   int Div[2] = {2,2};
   int size[2] = {600,400}; //x,y
   TCanvas * cAnaTrace = new TCanvas("cAnaTrace", "cAnaTrace", 0, 0, size[0]*Div[0], size[1]*Div[1]);
   if(!cAnaTrace->GetShowEditor() )cAnaTrace->ToggleEditor();
   if(!cAnaTrace->GetShowToolBar() )cAnaTrace->ToggleToolBar();
   cAnaTrace->Divide(Div[0],Div[1]);
   for( int i = 1; i <= Div[0]*Div[1] ; i++){
      cAnaTrace->cd(i)->SetGrid();
   }

   cAnaTrace->cd(1);
   tree->Draw("te:te_t>>h(400, 0, 100, 400, -1000, 10000)");   

   cAnaTrace->cd(2);
   tree->Draw("te_r:te_t-te_r/2>>h(400, 0, 100, 400, -1, 20)");   
   
   cAnaTrace->cd(3);
   tree->Draw("ttac_t:te_t-te_r/2>>h(400, 0, 100, 400, 54, 57)"); 


   cAnaTrace->cd(4);
   tree->Draw("xf_t:xn_t>>h(400, 0, 100, 400, 54, 57)"); 


}
