//#include <TROOT.h>
//#include <TFile.h>
//#include <TTree.h>
//#include <TH1.h>
//#include <TH2.h>
//#include <TCut.h>
//#include <TCanvas.h>
//#include <TStyle.h>

//void analysis()
{
   
   gErrorIgnoreLevel = kInfo;

   TFile * f1 = new TFile("A_gen_run064.root");      // 206Hg
   TTree * t1 = (TTree *) f1->FindObjectAny("tree");

   //TFile * f2 = new TFile("A_gen_run021.root");      // 130Xe
   //TTree * t2 = (TTree *) f2->FindObjectAny("tree");

   TFile * ft1 = new TFile("transfer.root");      // 206Hg simulation
   TTree * tt1 = (TTree *) ft1->FindObjectAny("tree");
   TFile * ft2 = new TFile("../ISS_130Xe/transfer.root");      // 130Xe simulation
   TTree * tt2 = (TTree *) ft2->FindObjectAny("tree");

	TCut cutDet = "detID != 6 || detID != 7 || detID != 8 || detID != 11 || detID !=13 || detID != 17 || detID != 19 ";
   TCut cutEBIS = "100 < (e_t-EBIS_t)/1e2 && (e_t-EBIS_t)/1e2 < 900";
   TCut cutEBIS2 = "11000 < (e_t-EBIS_t)/1e2 && (e_t-EBIS_t)/1e2 < 19000";
   //TCut cutE   = "9 > e && e > 1.2";
   TCut cutE   = "e > 1.2";
   TCut cutX   = "-0.9 < x && x < 0.9";
   TCut cutEx0 = "-0.14 < Ex && Ex < 0.14";
   TCut cutEx0s = "(-0.28 < Ex && Ex < -0.14) || (0.14 < Ex && Ex < 0.20)";
   TCut cutEx1 = " 1.1 < Ex && Ex < 1.3";
   TCut cutEx2 = " 1.6 < Ex && Ex < 2.0";
   TCut cutEx3 = " 2.2 < Ex && Ex < 2.6";
   
   TCut cutEx0Xe = " -3   < Ex && Ex < -2.7";
   TCut cutEx1Xe = " -1.5 < Ex && Ex < -1.2";
   TCut cutEx2Xe = " -0.2 < Ex && Ex <  0.4";
   
   //================= canvas 
   TCanvas * c1 = new TCanvas("c1", "Analysis", 600, 400);
   c1->Divide(1,1);
   if(! c1->GetShowEventStatus() ) c1->ToggleEventStatus();
   if(! c1->GetShowEditor()      ) c1->ToggleEditor();
   
   c1->SetGrid();
   
   gStyle->SetOptStat("neiou");
   
   
   
   
   //t1->Draw("0.014797621 * z - e + 8.68>> h(200, -1, 4)", cutEBIS && cutE );
   
   
   //t1->Draw("e:z>>hEz(400, -450, 0, 400, 0, 10)", cutEBIS && cutE , "colz");
   
   
   /**///=================== 130Xe
   /*c1->cd(1);
   t2->Draw("e:z>>hEzXe(400, -450, 0, 400, 0, 10)", cutEBIS && cutE , "colz");
   TClonesArray * fxList = (TClonesArray*) ft2->FindObjectAny("fxList");
   fxList->At(0)->Draw("same");
   
   
   /**///=================== 206Hg - E vs Z
   /*c1->cd(1);
   TH2F* hEz  = new TH2F("hEz", "e - z; z [mm]; e [MeV]", 450, -450, 0, 400, 0, 10);
   TH2F* hExT = new TH2F("hExT", "Ex - thetaCM; thetaCM [deg]; Ex [MeV]", 400, 0, 50, 400, -1, 5);
   t1->Draw("e:z>>hEz", cutEBIS && cutE && cutX , "colz");
   t1->Draw("Ex:thetaCM>>hExT", cutEBIS && cutE && cutX, "colz");
   
   /*TClonesArray * fxList = (TClonesArray*) ft1->FindObjectAny("fxList");
   TClonesArray * gList = (TClonesArray*) ft1->FindObjectAny("gList");
   fxList->At(0)->Draw("same");
   fxList->At(1)->Draw("same");
   fxList->At(2)->Draw("same");
   fxList->At(3)->Draw("same");
   
   gList->At(0)->Draw("same");
   gList->At(10)->Draw("same");
   gList->At(20)->Draw("same");
   gList->At(30)->Draw("same");
   gList->At(40)->Draw("same");
   gList->At(50)->Draw("same");
   
   /**///=================== 206Hg - Ex vs thetCM
   /*c1->cd(1);
   TH2F* hExT = new TH2F("hExT", "Ex - thetaCM; thetaCM [deg]; Ex [MeV]", 400, 0, 50, 400, -1, 5);
   t1->Draw("Ex:thetaCM>>hExT", cutEBIS && cutE && cutX, "colz");
   
   /**///=================== 206Hg - Ex 4 side of the detector
   /*c1->cd(1);
   
   TH1F * hExr0 = new TH1F("hExr0", "Ex (left); Ex [MeV]; count / 50 keV", 100, -1, 4);
   TH1F * hExr1 = new TH1F("hExr1", "Ex (left); Ex [MeV]; count / 50 keV", 100, -1, 4);
   TH1F * hExr2 = new TH1F("hExr2", "Ex (left); Ex [MeV]; count / 50 keV", 100, -1, 4);
   TH1F * hExr3 = new TH1F("hExr3", "Ex (left); Ex [MeV]; count / 50 keV", 100, -1, 4);
   
   t1->Draw("Ex>>hExr0", cutEBIS && cutE && cutX && cutDet && " 0 <= detID && detID <  6 ", "");
   t1->Draw("Ex>>hExr1", cutEBIS && cutE && cutX && cutDet && " 6 <= detID && detID < 12 ", "");
   t1->Draw("Ex>>hExr2", cutEBIS && cutE && cutX && cutDet && "12 <= detID && detID < 18 ", "");
   t1->Draw("Ex>>hExr3", cutEBIS && cutE && cutX && cutDet && "18 <= detID && detID < 24 ", "");
   
   hExr0->Draw();
   hExr1->SetLineColor(2); hExr1->Draw("same");
   hExr2->SetLineColor(4); hExr2->Draw("same");
   hExr3->SetLineColor(6); hExr3->Draw("same");
   
   /**///=================== 206Hg - thetCM
   /*c1->cd(1);
   TH1F* hT0 = new TH1F("hT0", "Ex (det - 0); Ex [MeV]; count / 50 keV", 100, -1, 4);
   TH1F* hT1 = new TH1F("hT1", "Ex (det - 1); Ex [MeV]; count / 50 keV", 100, -1, 4);
   TH1F* hT2 = new TH1F("hT2", "Ex (det - 2); Ex [MeV]; count / 50 keV", 100, -1, 4);
   TH1F* hT3 = new TH1F("hT3", "Ex (det - 3); Ex [MeV]; count / 50 keV", 100, -1, 4);
   TH1F* hT4 = new TH1F("hT4", "Ex (det - 4); Ex [MeV]; count / 50 keV", 100, -1, 4);
   TH1F* hT5 = new TH1F("hT5", "Ex (det - 5); Ex [MeV]; count / 50 keV", 100, -1, 4);
   
   t1->Draw("Ex>>hT0", cutEBIS && cutE && cutX && "detID%6==0 && thetaCM > 10", "colz");
   t1->Draw("Ex>>hT1", cutEBIS && cutE && cutX && "detID%6==1 && thetaCM > 12", "colz");
   t1->Draw("Ex>>hT2", cutEBIS && cutE && cutX && "detID%6==2 && thetaCM > 15", "colz");
   t1->Draw("Ex>>hT3", cutEBIS && cutE && cutX && "detID%6==3 && thetaCM > 17", "colz");
   t1->Draw("Ex>>hT4", cutEBIS && cutE && cutX && "detID%6==4", "colz");
   t1->Draw("Ex>>hT5", cutEBIS && cutE && cutX && "detID%6==5", "colz");
   
   
   
   /**///=================== 206Hg - Ex
   /*c1->cd(1);
   
   TH1F* hEx  = new TH1F("hEx",  "Ex; Ex[MeV]; count/ 25 keV", 200, -1, 4);
   TH1F* hEx2 = new TH1F("hEx2", "Ex; Ex[MeV]; count/ 25 keV", 200, -1, 4);
   TH1F* hEx3 = new TH1F("hEx3", "Ex; Ex[MeV]; count/ 25 keV", 200, -1, 4);
   hEx2->SetLineColor(2);
   
   t1->Draw("Ex>>hEx", cutEBIS + cutE, "");
   t1->Draw("Ex>>hEx2", cutEBIS + cutE + cutX, "same");
   t1->Draw("Ex>>hEx3", cutEBIS + cutE + !cutX, "same");
   
   
   hEx2->Draw();
   
   
   /**///=================== 206Hg - thetCM, Ex = 0 
   /*c1->cd(1);
   TCut cutTheta1 = "17 < thetaCM && thetaCM < 22";
   TH1F* hEx1 = new TH1F("hEx1", "Ex (thetaCM gated); Ex [MeV]; count / 50 keV", 100, -1, 4);
   t1->Draw("Ex>>hEx1", cutEBIS && cutE && cutX && cutTheta1, "colz");
   
   TCut cutTheta2 = "24 < thetaCM && thetaCM < 28";
   TH1F* hEx2 = new TH1F("hEx2", "Ex (thetaCM gated); Ex [MeV]; count / 50 keV", 100, -1, 4);
   t1->Draw("Ex>>hEx2", cutEBIS && cutE && cutX && cutTheta2, "colz");
   
   TCut cutTheta3 = "29 < thetaCM && thetaCM < 32.5";
   TH1F* hEx3 = new TH1F("hEx3", "Ex (thetaCM gated); Ex [MeV]; count / 50 keV", 100, -1, 4);
   t1->Draw("Ex>>hEx3", cutEBIS && cutE && cutX && cutTheta3, "colz");
   
   TCut cutTheta4 = "33.5 < thetaCM && thetaCM < 36.5";
   TH1F* hEx4 = new TH1F("hEx4", "Ex (thetaCM gated); Ex [MeV]; count / 50 keV", 100, -1, 4);
   t1->Draw("Ex>>hEx4", cutEBIS && cutE && cutX && cutTheta4, "colz");
   
   TCut cutTheta5 = "37 < thetaCM && thetaCM < 40";
   TH1F* hEx5 = new TH1F("hEx5", "Ex (thetaCM gated); Ex [MeV]; count / 50 keV", 100, -1, 4);
   t1->Draw("Ex>>hEx5", cutEBIS && cutE && cutX && cutTheta5, "colz");
   
   TCut cutTheta6 = "41 < thetaCM && thetaCM < 44";
   TH1F* hEx6 = new TH1F("hEx6", "Ex (thetaCM gated); Ex [MeV]; count / 50 keV", 100, -1, 4);
   t1->Draw("Ex>>hEx6", cutEBIS && cutE && cutX && cutTheta6, "colz");
   
   
   /**///=================== 206Hg - thetCM, Ex = 1.2 
   /*c1->cd(1);
   TCut cutTheta1 = "16.5 < thetaCM && thetaCM < 23";
   TH1F* hEx1 = new TH1F("hEx1", "Ex (thetaCM gated); Ex [MeV]; count / 50 keV", 100, -1, 4);
   t1->Draw("Ex>>hEx1", cutEBIS && cutE && cutX && cutTheta1, "colz");
   
   TCut cutTheta2 = "24 < thetaCM && thetaCM < 28.5";
   TH1F* hEx2 = new TH1F("hEx2", "Ex (thetaCM gated); Ex [MeV]; count / 50 keV", 100, -1, 4);
   t1->Draw("Ex>>hEx2", cutEBIS && cutE && cutX && cutTheta2, "colz");
   
   TCut cutTheta3 = "29.5 < thetaCM && thetaCM < 33.5";
   TH1F* hEx3 = new TH1F("hEx3", "Ex (thetaCM gated); Ex [MeV]; count / 50 keV", 100, -1, 4);
   t1->Draw("Ex>>hEx3", cutEBIS && cutE && cutX && cutTheta3, "colz");
   
   TCut cutTheta4 = "34 < thetaCM && thetaCM < 38";
   TH1F* hEx4 = new TH1F("hEx4", "Ex (thetaCM gated); Ex [MeV]; count / 50 keV", 100, -1, 4);
   t1->Draw("Ex>>hEx4", cutEBIS && cutE && cutX && cutTheta4, "colz");
   
   TCut cutTheta5 = "38.5 < thetaCM && thetaCM < 40.5";
   TH1F* hEx5 = new TH1F("hEx5", "Ex (thetaCM gated); Ex [MeV]; count / 50 keV", 100, -1, 4);
   t1->Draw("Ex>>hEx5", cutEBIS && cutE && cutX && cutTheta5, "colz");
   
   
   /**///=================== 206Hg - thetCM, Ex = 1.8
   /*c1->cd(1);
   TCut cutTheta1 = "12.5 < thetaCM && thetaCM < 19.5";
   TH1F* hEx1 = new TH1F("hEx1", "Ex (thetaCM gated); Ex [MeV]; count / 50 keV", 100, -1, 4);
   t1->Draw("Ex>>hEx1", cutEBIS && cutE && cutX && cutTheta1, "colz");
   
   TCut cutTheta2 = "21 < thetaCM && thetaCM < 26";
   TH1F* hEx2 = new TH1F("hEx2", "Ex (thetaCM gated); Ex [MeV]; count / 50 keV", 100, -1, 4);
   t1->Draw("Ex>>hEx2", cutEBIS && cutE && cutX && cutTheta2, "colz");
   
   TCut cutTheta3 = "27.5 < thetaCM && thetaCM < 31.5";
   TH1F* hEx3 = new TH1F("hEx3", "Ex (thetaCM gated); Ex [MeV]; count / 50 keV", 100, -1, 4);
   t1->Draw("Ex>>hEx3", cutEBIS && cutE && cutX && cutTheta3, "colz");
   
   TCut cutTheta4 = "32.5 < thetaCM && thetaCM < 36";
   TH1F* hEx4 = new TH1F("hEx4", "Ex (thetaCM gated); Ex [MeV]; count / 50 keV", 100, -1, 4);
   t1->Draw("Ex>>hEx4", cutEBIS && cutE && cutX && cutTheta4, "colz");
   
   TCut cutTheta5 = "37 < thetaCM && thetaCM < 40";
   TH1F* hEx5 = new TH1F("hEx5", "Ex (thetaCM gated); Ex [MeV]; count / 50 keV", 100, -1, 4);
   t1->Draw("Ex>>hEx5", cutEBIS && cutE && cutX && cutTheta5, "colz");
   
   
   /**///=================== 206Hg - thetCM, Ex = 2.0
   /*c1->cd(1);
   TCut cutTheta1 = "9.5 < thetaCM && thetaCM < 18.5";
   TH1F* hEx1 = new TH1F("hEx1", "Ex (thetaCM gated); Ex [MeV]; count / 50 keV", 100, -1, 4);
   t1->Draw("Ex>>hEx1", cutEBIS && cutE && cutX && cutTheta1, "colz");
   
   TCut cutTheta2 = "20 < thetaCM && thetaCM < 25";
   TH1F* hEx2 = new TH1F("hEx2", "Ex (thetaCM gated); Ex [MeV]; count / 50 keV", 100, -1, 4);
   t1->Draw("Ex>>hEx2", cutEBIS && cutE && cutX && cutTheta2, "colz");
   
   TCut cutTheta3 = "26.5 < thetaCM && thetaCM < 31";
   TH1F* hEx3 = new TH1F("hEx3", "Ex (thetaCM gated); Ex [MeV]; count / 50 keV", 100, -1, 4);
   t1->Draw("Ex>>hEx3", cutEBIS && cutE && cutX && cutTheta3, "colz");
   
   TCut cutTheta4 = "32 < thetaCM && thetaCM < 35.5";
   TH1F* hEx4 = new TH1F("hEx4", "Ex (thetaCM gated); Ex [MeV]; count / 50 keV", 100, -1, 4);
   t1->Draw("Ex>>hEx4", cutEBIS && cutE && cutX && cutTheta4, "colz");
   
   TCut cutTheta5 = "36.5 < thetaCM && thetaCM < 39.5";
   TH1F* hEx5 = new TH1F("hEx5", "Ex (thetaCM gated); Ex [MeV]; count / 50 keV", 100, -1, 4);
   t1->Draw("Ex>>hEx5", cutEBIS && cutE && cutX && cutTheta5, "colz");
   
   /**///==================== compare 130Xe and 206Hg    
   /*c1->cd(1);
   t1->Draw("Ex>>hEx(500, -10, 8)", cutEBIS && cutE , "");
   t1->Draw("Ex>>hEx2(500, -10, 8)", cutEBIS2 && cutE , "");
   t2->Draw("Ex>>hExXe(500, -10, 8)", cutEBIS && cutE, "same");
   hExXe->SetLineColor(2);
   hExXe->Scale(0.37);
   
   hEx2->Scale(0.1);
   hEx2->SetLineColor(6);
   
   gStyle->SetOptStat(0);
   
   hEx->Draw();
   hEx2->Draw("hist same");
   hExXe->Draw("hist same");
   
   /**///==================== ground state  angular distribution
   /*
   c1->cd(1);
   TH1F * hT0  = new TH1F("hT0",  "Ex = 0 MeV; thetCM [deg]; count / 1 deg", 50, 0, 50);
   TH1F * hT0s = new TH1F("hT0s", "Ex = 0 MeV; thetCM [deg]; count / 1 deg", 50, 0, 50);
   hT0s->SetLineColor(2);
   
   t1->Draw("thetaCM>>hT0", cutEBIS + cutE + cutX + cutEx0);
   t1->Draw("thetaCM>>hT0s", cutEBIS + cutE + cutX + cutEx0s, "same");
   
   TH1F * hT0c = (TH1F*) hT0->Clone();
   hT0c->SetName("hT0c");
   hT0c->Sumw2();
   hT0s->Sumw2();
   hT0c->Add(hT0s, -1);
   hT0c->Draw("");
   
   
   /**///==================== angular background construction
   // using TSepctrum to estimate the background
   // substract the total with the background, get the signal
   // substract the total with the signal, get the background.
   /*c1->cd(1);
   t1->Draw("Ex:thetaCM>>hEt(35, 10, 45, 70, 0.5, 4)", cutEBIS && cutE , "colz");
   
   c1->cd(2);
   t2->Draw("Ex:thetaCM>>hEtXe(35, 10, 45, 70, 0.5, 4)", cutEBIS && cutE , "colz");
   
   hEtXe->Scale(0.4);
   hEtXe->GetZaxis()->SetRangeUser(0, 27);
   hEtXe->Draw("colz");
   
   c1->cd(3);
   TH2F * h2 = (TH2F*) hEt->Clone();
   h2->Add(hEtXe, -1);
   
   h2->Draw("colz");
   
   
   c1->cd(4);
   
   TCut cutAux = " 0.5 < Ex && Ex < 1.0";
   
   t1->Draw("thetaCM>>ht0(7, 10, 45)",   cutEBIS && cutE && cutEx2);
   t2->Draw("thetaCM>>ht0Xe(7, 10, 45)", cutEBIS && cutE && cutEx2);
   ht0Xe->Scale(0.4);
   ht0Xe->SetLineColor(2);
   
   ht0->Draw();
   ht0Xe->Draw("hist same");
   
   c1->cd(5);
   
   c1->cd(5)->SetLogy();
   TH1F * h1 = (TH1F*) ht0->Clone();
   h1->Add(ht0Xe, -1);
   
   h1->Draw();
   /**///==================== angular background construction
   /*
   c1->cd(1);
   t1->Draw("Ex:thetaCM>>hEt(35, 10, 45, 100, -1, 4)", cutEBIS && cutE , "colz");
   //t1->Draw("e:z>>hEt(20, -450,  -65, 20, 1.5, 10)", cutEBIS && cutE && (!cutEx0 && !cutEx1 && !cutEx2 && !cutEx3), "colz");
   
   //TF2 * fit = new TF2("fit", " [0] * TMath::Exp(x / [1]) * TMath::Exp(-y/[2])", -450, -65, 1.5, 10);
   //
   //fit->SetNpx(100);
   //fit->SetNpy(100);
   //
   //fit->SetParameter(0,50);
   //fit->SetParameter(1,300);
   //fit->SetParameter(2,5);
   //
   //hEt->Fit(fit);
   
   /*
   int nX = hEt->GetNbinsX();
   int nY = hEt->GetNbinsY();
   
   c1->cd(2);
   TH1D ** hpy = new TH1D*[nX]; 
   for( int iX = 1; iX <= nX; iX++){
      TString name;
      name.Form("hpy%d", iX);
      hpy[iX] = new TH1D(name, "hpy", 160, -4, 4);
      hEt->ProjectionY(name, iX, iX);
      hpy[iX]->Draw("");
      c1->Update();
      gSystem->ProcessEvents();
   }
   
   /*
   TGraph2D * g2D = new TGraph2D();
   int count = 0;
   for( int iX = 1; iX <= nX; iX++){
      for( int iY = 1; iY <= nY; iY++){
         double x = hEt->GetXaxis()->GetBinCenter(iX);
         double y = hEt->GetYaxis()->GetBinCenter(iY);
         double z = hEt->GetBinContent(iX, iY);
         if( z < 0.5 ) continue;
         count ++;
         
         g2D->SetPoint(count, x, y, z);
      }
   }   
   g2D->Draw();
   */
   
   
   
   
   
   
   
   
   
   
   
   
   
   
}
