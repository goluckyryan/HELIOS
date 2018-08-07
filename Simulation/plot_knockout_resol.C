{

	TFile * f1 = new TFile("knockout_resol.root", "read");
	TTree * tree = (TTree *) f1->Get("tree");
	
	TCanvas * c1 = new TCanvas("c1" , "c1", 0, 0, 500, 500 );
	
	




}
