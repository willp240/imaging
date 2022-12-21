import numpy as np
import ROOT
import sys

if len(sys.argv) != 2:
  print "USAGE: %s <input file> "%(sys.argv[0]) 
  sys.exit(1)

inFileName = sys.argv[1]
print "Reading from", inFileName

inFile = ROOT.TFile.Open(inFileName ,"READ")

h_t = inFile.Get("new_t")
h_llh = inFile.Get("new_llh")

#Create a sphere of radius 6000.
radius = 6000
f3 = ROOT.TF3("sphere", "x*x+y*y+z*z - %f" % radius**2, -radius, radius, -radius, radius, -radius, radius)
f3.SetLineColor(9)

f3.Draw("FBBB")
h_t.Draw("box2z same")
ROOT.gPad.Update()
raw_input("Press Enter to exit...")


f3.Draw("")
#h_llh.Draw("box2z")
ROOT.gPad.Update()
raw_input("Press Enter to exit...")
