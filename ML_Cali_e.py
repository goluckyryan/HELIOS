#!/usr/bin/python
import math
import argparse
#import ROOT as root
from ROOT import gROOT, gStyle, TCanvas, TH1F, TF1, TFile, TTree, TString, TGraph


def is_number(string):
   try:
      float(string)
      return True
   except ValueError:
      return False

#================================

###################################################### open root file
 
gROOT.Reset()

#============= root file input
rootfile="X_H052_Mg25.root";
treeName="tree"
f0 = TFile (rootfile, "read") 
tree = f0.Get(treeName)
nEntries = tree.GetEntries()
print("=====> /// %15s //// is loaded. Total #Entry: %10d " % (rootfile,  nEntries))

'''
#============= Canvas
Div = [1,1]  #x,y
size = [800,600] #x,y

cScript = TCanvas("cScript", "cScript", 0, 0, size[0]*Div[0], size[1]*Div[1])
cScript.Divide(Div[0],Div[1])

for i in range(1, Div[0]*Div[1]+1) :
   cScript.cd(i).SetGrid()
cScript.cd(1)

gStyle.SetOptStat(1111111)
gStyle.SetStatY(0.99)
gStyle.SetStatX(0.99)
gStyle.SetStatW(0.2)
gStyle.SetStatH(0.1)
'''   

#============= read entry from root 



for index, event in enumerate(tree):
   eArray = event.e
   Ex = event.Ex
   zArray = event.z
   e = 0;
   z = 0;
   detID = event.detID
   for i in range(0,24) :
      if eArray[i] > 0 : e = eArray[i]
      if zArray[i] < 0 : z = zArray[i]
   if Ex > 0 and e > 0 and detID != -4: print(" %d, %f, %f, %f , %d " %( index, e, Ex, z, detID ))
   
   if index > 200 : break


## wait for input to keep the GUI (which lives on a ROOT event dispatcher) alive
if __name__ == '__main__':
   rep = ''
   while not rep in [ 'q', 'Q' ]:
      rep = raw_input( 'enter "q" to quit: ' )
      if 1 < len(rep):
         rep = rep[0]
