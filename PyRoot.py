#!/usr/bin/python
import math
import argparse
from ROOT import gROOT, gStyle, TCanvas, TH1F, TF1, TFile, TTree, TString, TGraph


def is_number(string):
   try:
      float(string)
      return True
   except ValueError:
      return False

#================================
parser = argparse.ArgumentParser()
parser.add_argument("fileName", type=str, help="input file name")
parser.add_argument("ExID", type=int, help="id of Ex")
args = parser.parse_args()
fileName = args.fileName
ExID = args.ExID

print("=========== Extract X-sec from : ", fileName)

searchAngle = "0INPUT... anglemin"

saveFileName = fileName + ".Xsec.txt"

print(" saving X-sec into : ", saveFileName)

fopen = open(fileName, "r");

startExtract = False

saveFileFirstLine = []
saveFileFirstLine.append("angle") 
dataMatrix = []
angle = []
Ex = []

count = 0
reactionNum = 0
lineNum = 0
Size = 0

print("===============================")

for line in fopen.readlines():
   line = line.strip()
   lineNum += 1
   
   posStart = line.find("$============================================")
   posEx = line.find("Ex=")
   if posEx >= 0 :
      Ex.append(float(line[posEx+3: posEx+8]))
   if posStart > 0: 
      startMsg = line[posStart + 46:]
      saveFileFirstLine.append(startMsg)
      count = 0   
      reactionNum += 1
      print("%2d" % reactionNum, startMsg)
   
   posReaction = line.find("REACTION:")
   if posReaction > 0:
      reaction = line[posReaction + 9:]
      #print("%2d" % reactionNum, reaction)
      
   posAngleMin = line.find("anglemin=")
   posAngleMax = line.find("anglemax=")
   posAngleStep = line.find("anglestep=") 
   if posAngleMin > 0:
      angleMin = int(line[posAngleMin+9:posAngleMax])
      angleMax = int(line[posAngleMax+9:posAngleStep])
      angleStep = float(line[posAngleStep+10:])
      Size = int((angleMax - angleMin)/angleStep)
      tempData = []
      #if reactionNum == 1: print(" getting angle setting : ", angleMin , angleMax, angleStep)
 
   if startExtract == False and line == "0  C.M.  REACTION     REACTION   LOW L  HIGH L   % FROM" :
      startExtract = True
      #print("---------------start extraction.")
      
   if startExtract -- True and line == "0 JP  JT  LX MX      TOTAL      PERCENT     (VALUES FOR M > 0 NOT DOUBLED.)":
      startExtract = False
      #print("---------------end of extraction.")
      
   if startExtract == True :
      #print(" %s " %(line) )
      #get angle and Xsec
      if is_number(line[:5]) == True :  
         dataStr = line[:19]
         data = dataStr.split()
         if count <= Size and len(data) == 2:
            if reactionNum == 1:
               angle.append(float(data[0]))
            tempData.append(float(data[1]))
            #print( count , Size)
            count += 1
            if count == Size : dataMatrix.append(tempData)
         
               
fopen.close()

#====== print data statistics

print("==============================")
print(" number of lines read  : ", lineNum)
print(" getting angle setting : ", angleMin , angleMax, angleStep)
print(" number of Angles      : ", Size )
print(" number of reactions   : ", reactionNum)


######################################################
 
gROOT.Reset()

#============= root file input
rootfile="test.root";
treeName="tree"
f0 = TFile (rootfile, "read") 
tree = f0.Get(treeName)
print("=====> /// %15s //// is loaded. Total #Entry: %10d " % (rootfile,  tree.GetEntries()))

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
   

#ExID = 0

#============= get the acceptance from root 
nBin = 2000
h = TH1F("h", "h", nBin, 0, 50)
expression = "thetaCM*TMath::RadToDeg() >> h"
gate_e = "tag == 2 && TMath::Abs(Ex-%f) < 0.1 && thetaCM*TMath::RadToDeg() > 8 " % Ex[ExID]
tree.Draw(expression, gate_e)
hMax = h.GetMaximum()

#============= create TGraph for theoritical cross section for ExID
gr = TGraph()

for i in range(0, len(angle)):
   gr.SetPoint(i, angle[i], dataMatrix[ExID][i])

gMax = gr.GetYaxis().GetXmax()

#print("%f, %f " % (hMax, gMax))

h.Scale(gMax/hMax)
gr.Draw("AC")
h.Draw("same")
cScript.Update()


#find the acceptance, the angle the count drop
acceptAngle = []
print(" ============================ Ex : %f " % Ex[ExID])
for j in range(1, nBin) :
   a = h.GetBinContent(j);
   b = h.GetBinContent(j+1);
   
   if a == 0 and b > 0:
      acceptAngle.append(h.GetBinLowEdge(j+1))
      #printf(" boundary : %10.5f\n", h[i]->GetBinLowEdge(j+1) );
   
   if a > 0 and b == 0:
      acceptAngle.append(h.GetBinLowEdge(j+1))
      #printf(" boundary : %10.5f\n", h[i]->GetBinLowEdge(j+1) );
      
dCos = []
DegToRad = 0.017453292
for j in range(0, len(acceptAngle)/2-1):
   delta = acceptAngle[2*j+1] - acceptAngle[2*j]
   mean = (acceptAngle[2*j+1] + acceptAngle[2*j])/2
   dCos.append(math.sin(mean*DegToRad)*(delta*DegToRad))
   print("%2d | %10.5f - %10.5f = %10.5f | %10.5f, %10.5f "%(j, acceptAngle[2*j], acceptAngle[2*j+1], delta, mean, 1./dCos[j-1]) )

######################################################
#========= calculate intergated X-sec
print("=============================")

dphi = 4* 2 * math.atan2(4.5, 11.5)

#=== integrate X-sec
Xsec = 0.0
for i in range(0, Size) :
   for m in range(0, len(acceptAngle)/2-1):
      #check = 0
      #if acceptAngle[2*m] < angle[i] and angle[i] < acceptAngle[2*m+1] : 
         #check = 1
      #print("%d, %d, %5.2f, %5.2f, %5.2f, %10.4f, %d, %10.4f" % (i,  m,  acceptAngle[2*m], angle[i], acceptAngle[2*m+1], dataMatrix[ExID][i], check , Xsec))
      if acceptAngle[2*m] < angle[i] and angle[i] < acceptAngle[2*m+1] : 
         Xsec += dataMatrix[ExID][i] * math.sin( angle[i] * DegToRad) * angleStep * dphi
         #print("-------")
         #print("%d, %d, %5.2f, %5.2f, %5.2f, %10.4f, %10.4f" % (i,  m,  acceptAngle[2*m], angle[i], acceptAngle[2*m+1], dataMatrix[ExID][i], Xsec))

print("%s, total Xsec : %10.4f mb" % (saveFileFirstLine[ExID+1] , Xsec))



## wait for input to keep the GUI (which lives on a ROOT event dispatcher) alive
if __name__ == '__main__':
   rep = ''
   while not rep in [ 'q', 'Q' ]:
      rep = raw_input( 'enter "q" to quit: ' )
      if 1 < len(rep):
         rep = rep[0]
