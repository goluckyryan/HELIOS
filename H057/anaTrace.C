{

   TFile * f1 = new TFile ("trace.root", "read");
   TTree * tree = (TTree *) f1->Get("tree");
   
	int totnumEntry = tree->GetEntries();
   printf( "========== total Entry : %d \n", totnumEntry);

   int Div[2] = {1,1};
   int size[2] = {600,400}; //x,y
   TCanvas * cAnaTrace = new TCanvas("cAnaTrace", "cAnaTrace", 0, 0, size[0]*Div[0], size[1]*Div[1]);
   if(!cAnaTrace->GetShowEditor() )cAnaTrace->ToggleEditor();
   if(!cAnaTrace->GetShowToolBar() )cAnaTrace->ToggleToolBar();
   cAnaTrace->Divide(Div[0],Div[1]);
   for( int i = 1; i <= Div[0]*Div[1] ; i++){
      cAnaTrace->cd(i)->SetGrid();
   }

   cAnaTrace->cd(1);
   


}
