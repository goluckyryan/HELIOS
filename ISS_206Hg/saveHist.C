#include <fstream>
#include <stdlib.h>

void saveHist(){
  
  FILE * file_out;
  file_out = fopen ("206Hg_dp_Ex.txt", "w+");
  
  int a = 4;
  int b = 5;
  
  int len = a + b + 1;
  
  fprintf(file_out, "%*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s\n", len, "Ex", len, "raw", len, "sub", len, "fit", len, "f0", len, "f1", len, "f2", len, "f3", len, "f4", len, "f5", len, "f6");
  
  for( int i = 1 ; i <= hEx->GetNbinsX() ; i++){
    
    double x = hEx->GetBinCenter(i);
    
    double y = hEx->GetBinContent(i);
    
    double ys = specS->GetBinContent(i);
    
    double fa = fita->Eval(x);
    double f0 = gFit0->Eval(x);
    double f1 = gFit1->Eval(x);
    double f2 = gFit2->Eval(x);
    double f3 = gFit3->Eval(x);
    double f4 = gFit4->Eval(x);
    double f5 = gFit5->Eval(x);
    double f6 = gFit6->Eval(x);
  
    fprintf(file_out, "%*.*f %*.*f %*.*f %*.*f %*.*f %*.*f %*.*f %*.*f %*.*f %*.*f %*.*f\n", len, b, x, len, b, y, len, b, ys, len, b, fa, len, b, f0, len, b, f1, len, b, f2, len, b, f3, len, b, f4, len, b, f5, len, b, f6);
  
  }
  
  
  fclose(file_out);
  
  /*
  TFile * saveFile = new TFile("206Hg_dp.root", "RECREATE");
  
  TH1F * hist = NULL;
  TString name;
  for( int i = 0; i < 24; i++){
     name.Form("hExi%d", i);
     hist = (TH1F*) _file0->FindObjectAny(name);
     hist->Write();
  }
  
  for( int i = 0; i < 6; i++){
     name.Form("hExc%d", i);
     hist = (TH1F*) _file0->FindObjectAny(name);
     hist->Write();
  }
  
  for( int i = 0; i < 6; i++){
     name.Form("hExcA%d", i);
     hist = (TH1F*) _file0->FindObjectAny(name);
     hist->Write();
  }
  
  saveFile->Close();
  */ 
}
