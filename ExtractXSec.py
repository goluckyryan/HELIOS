#!/usr/local/bin/python

import argparse
import math

def is_number(string):
   try:
      float(string)
      return True
   except ValueError:
      return False

#================================

parser = argparse.ArgumentParser()
parser.add_argument("fileName", type=str, help="input file name")
args = parser.parse_args()
fileName = args.fileName

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

#====== save dataMatrix
fsave = open(saveFileName, "w");

for x in saveFileFirstLine :
   fsave.write("%17s\t" % x)
   
fsave.write("\n")

for i in range(0, Size):
   fsave.write("%17.2f\t" % angle[i])
   for j in range(0, reactionNum):
      fsave.write("%17.6f\t" % dataMatrix[j][i])
   fsave.write("\n")

fsave.close()

#========= load acceptance.dat
print("==== calculating angluar acceptance")
acceptFileName = "acceptance.dat"
aOpen = open(acceptFileName, "r")

#------- if open fail
ap = [];

for line in aOpen.readlines():
   line = line.split()
   #print(line)
   temp = []
   for x in line:
      temp.append(float(x))
   ap.append(temp)

#print(ap)

aOpen.close()

#========= calculate intergated X-sec

print("=============================")

dphi = 4* 2 * math.atan2(4.5, 11.5)

nEx = len(Ex)
for k in range(0, nEx) :
   
   thetaCM = []
   for i in range(2,12):
      a = 0.0
      n = len(ap[i])
      for p in range(0, n) : a += ap[i][p] * pow(Ex[k], p)
      if a > 0.0 : thetaCM.append(a)
   #print(thetaCM)
   #=== integrate X-sec
   n = len(thetaCM)/2
   Xsec = 0.0
   for i in range(0, Size) :
      for m in range(0, n):
         if thetaCM[2*m] > angle[i] and angle[i] > thetaCM[2*m+1] : 
            Xsec += dataMatrix[k][i] * math.sin( angle[i] * 3.14159 / 180) * angleStep * dphi
         #print("%d, %d, %5.2f, %5.2f, %5.2f, %d, %10.4f, %10.4f" % (i,  m,  thetaCM[2*m], angle[i], thetaCM[2*m+1], check, dataMatrix[k][i], Xsec))
   
   #print("%7.4f MeV, total Xsec : %f " % (Ex[k] , Xsec))
   print("%s, total Xsec : %10.4f mb" % (saveFileFirstLine[k+1] , Xsec))
   
   
   
      
