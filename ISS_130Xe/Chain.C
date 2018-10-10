{
  TChain * chain = new TChain("gen_tree");

  //chain->Add("root_data/gen_run010.root");
  //chain->Add("root_data/gen_run011.root"); 
  //chain->Add("root_data/gen_run012.root");
  //chain->Add("root_data/gen_run013.root");
  chain->Add("root_data/gen_run01[4-9].root");
  chain->Add("root_data/gen_run02[0-9].root");
  //chain->Add("root_data/gen_run020.root");

  chain->GetListOfFiles()->Print();
 
  chain->Process("Monitors.C+");

}
