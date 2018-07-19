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
#print(" number of lines read  : ", lineNum)
print(" getting angle setting : ", angleMin , angleMax, angleStep)
print(" number of Angles      : ", Size )
print(" number of reactions   : ", reactionNum)

###################################################### experimental data

expAngle = [
   [ 24.2, 29.7, 34.3, 38.4],
   [ 15.8, 24.4, 30.2, 34.9, 39.2, 43.0],
   [ 11.5, 19.4, 26.7, 32.1, 36.8, 41.0],
   [       13.5, 22.7, 29.2, 34.4, 39.0],
   [       12.3, 20.9, 27.9, 33.4, 38.1],
   [       10.1, 17.7, 26.0, 31.9, 36.9],
   [             14.2, 24.0, 30.4, 35.7]
]

expDangle = [
   [  5.6, 4.6, 4.0, 3.6],
   [ 10.6, 6.1, 4.9, 4.3, 3.8, 3.5]
]

expData = [
   [ 84,  66,  73,  51],
   [ 71, 140, 140,  78,  45,  24],
   [167, 175, 311, 296, 241,  70],
   [     334, 391, 370, 331, 103],
   [     230, 238, 347, 270, 114],
   [      29, 130, 170, 155,  56],
   [          129, 266, 198,  53]
]

expDataErr = [
   [  9,   8,   9,   7],
   [  8,  11,  12,   9,   8,   7],
   [ 13,  13,  17,  17,  16,  10],
   [      18,  19,  19,  18,  10],
   [      15,  15,  19,  16,  10],
   [       6,  11,  13,  13,   7],
   [           11,  17,  15,   7]
]

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
   

#============= create TGraph for theoritical cross section for ExID
gr = TGraph()
for i in range(0, len(angle)):
   gr.SetPoint(i, angle[i], dataMatrix[ExID][i])

gExp = TGraph()
for i in range(0, len(expData[ExID])):
   gExp.SetPoint(i, expAngle[ExID][i], expData[ExID][i])

gExp.Draw("A* ")
#gr.Draw("AC same")
cScript.Update()




## wait for input to keep the GUI (which lives on a ROOT event dispatcher) alive
if __name__ == '__main__':
   rep = ''
   while not rep in [ 'q', 'Q' ]:
      rep = raw_input( 'enter "q" to quit: ' )
      if 1 < len(rep):
         rep = rep[0]
