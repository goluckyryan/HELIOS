#!/usr/local/bin/python
import argparse
import math

parser = argparse.ArgumentParser()
parser.add_argument("Ex", type=float, help="excitation energy")
#parser.add_argument("fileName", type=str, help="input file name")
args = parser.parse_args()
Ex = args.Ex

print("=========== Calculate Acceptance in thetaCM at %6.2f Ex " % Ex)

#--- load acceptance polynormial
acceptFileName = "acceptance.dat"
aOpen = open(acceptFileName, "r")

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

#--- load file
'''
Ex = []

fopen = open(fileName, "r")
lineNum = 0
for line in fopen.readlines():
   lineNum += 1
   #read first line to get the energy
   if lineNum == 1 :
      title = line.split()
      for x in title:
         posEx = x.find("Ex=")
         print(x, posEx)
         if posEx >= 0 :
            Ex.append(float(x[posEx+3: posEx+8]))
   #the rest
   
            
print(Ex)

fopen.close()
'''
#--- calculate acceptance
thetaCM = []
for i in range(0,12):
   a = 0.0
   n = len(ap[i])
   for p in range(0, n) : a += ap[i][p] * pow(Ex, p)
   if a > 0.0 : thetaCM.append(a)
   #print(thetaCM)

for i in range(0, 6):
   print("%2d | %6.2f - %6.2f = %6.2f " % (i, thetaCM[2*i], thetaCM[2*i+1], thetaCM[2*i] - thetaCM[2*i+1]))
   
