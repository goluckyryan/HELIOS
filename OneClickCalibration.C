{

   printf("==========================\n");
   printf("This macro is for calibration of the scilicon detectors of HELIOS.\n");
   printf("It requires 2 root files :  alpha_source.root  and  physical_run.root.\n");
   printf("It will first \n  1) calibrate the xf and xn using alpha_source.root.\n");
   printf(" 2) create a new *.root for next calibration on energy \n");
   printf(" 3) calibrate detectors at same z.  \n");   
   printf(" 4) calibrate detectors at difference z.  \n");   
   printf(" 5) create a new *.root after calibration. \n");

   //gROOT->ProcessLine(".x ~/ANALYSIS/Cali_alpha.C")

   

}
