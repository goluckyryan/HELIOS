{
   cScript->cd(2);
   TSpectrum * spec = new TSpectrum(20);
   int nPeaks = spec->Search(k, 1 ,"", 0.01);
   float * xpos = spec->GetPositionX();
   
   int * index = new int[nPeaks];
   TMath::Sort(nPeaks, xpos, index, 0 );  
   vector<double> energy;   
   for( int j = 0; j < nPeaks; j++){
      printf(" %d , x: %8.3f \n", j, xpos[index[j]]);
      //energy.push_back(xpos[index[j]]-xpos[index[0]]);
      energy.push_back(xpos[index[j]]);
   }
   
   //TH1 *hb = spec->Background
   
   // fitting using gaussians
   
   
   vector<double> knownE;
   knownE.push_back(0);
   knownE.push_back(1808.74);
   knownE.push_back(2938.33);
   knownE.push_back(3941.57);
   knownE.push_back(4318.89);
   knownE.push_back(4896);
   knownE.push_back(5291);
   
   // convert to real energy 
   int numPeak = knownE.size();
   TGraph * ga = new TGraph(numPeak, &energy[0], &knownE[0] );
   ga->Draw("*ap");
   ga->Fit("pol1", "q");
   double eC0 = pol1->GetParameter(0);
   double eC1 = pol1->GetParameter(1);
   printf("====  eC0:%8.3f, eC1:%8.3f \n", eC0, eC1);
   
   vector<double> realEnergy;
   for( int j = 0; j < nPeaks; j++){
      realEnergy.push_back(energy[j] * eC1 + eC0);
      printf(" %d , e: %8.3f \n", j, realEnergy[j]);
   }
   
   
   TH1F * z = new TH1F("z", "z" , 500, -500, 10000);
   z->SetXTitle("Ex [keV]");
   for( int i = 0; i < 6; i++){  
      for( int j = 0; j < 4; j++){   
         TString expression;
         expression.Form("((-(e[%d] - %f * x[%d])*%f - %f)*%f + %f)*%f + %f >>  + z" , 
                            i + 6*j ,
                            m[i] ,
                            i + 6*j , 
                            c1[i][j], 
                            c0[i][j],
                            j1[i],
                            j0[i],
                            eC1, eC0);
         
         tree->Draw(expression, "" , "");
      }
   }
   
   cScript->cd(1);
   z->Draw();
   cScript->cd(2);
   k->Draw();
   
      
   
   
   /*
   for( int i = 0; i < 6; i++){  
      printf("%d === q0: %f, q1: %f \n",i,  q0[i], q1[i]);
   }
   /*
   double j0[6], j1[6];
   for( int i = 1; i < 6; i++){  
      j0[i] = q0[i-1];
      j1[i] = q1[i-1];
   }
   j0[0] = 0;
   j1[0] = 1;
   
   for( int i = 0; i < 6; i++){  
      printf("%d === j0: %f, j1: %f \n",i,  j0[i], j1[i]);
   }
   /**/
   /*
   k->Reset();
   for( int i = 0; i < 6; i++){  
      for( int j = 0; j < 4; j++){   
         TString expression;
         expression.Form("(-(e[%d]*%f - %f * x[%d])*%f - %f)*%f + %f >>  + k" , 
                            i + 6*j ,  
                            cutIntep[0][0]/cutIntep[i][0]  ,  
                            m[i] ,
                            i + 6*j , 
                            c1[i][j], 
                            c0[i][j],
                            q1[i],
                            q0[i]);
         
         TString gate;
         gate.Form("e[%d] > %f + %f * x[%d] ", i + 6*j ,cutIntep[i][j], cutSlope, i + 6*j);
         
         tree->Draw(expression, gate , "");
      }
   }
   
   cScript->cd(2);
   k->Draw();
   /**/
}
