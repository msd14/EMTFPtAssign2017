# -*- coding: utf-8 -*-
print '------> Setting Environment'

import sys
import math
from ROOT import *
import numpy as np
from array import *
from termcolor import colored
from ROOT import gROOT

print '------> Importing Root File'

## Configuration settings
MAX_EVT  = -1 ## Maximum number of events to process
PRT_EVT  = 10000 ## Print every Nth event

## ============== Event branches================
evt_tree  = TChain('FlatNtupleMC/tree')

## ================ Read input files, define Output file ======================
dir1 = '/uscms/home/mdecaro/nobackup/BDTGEM/CMSSW_10_6_1_patch2/src/EMTFPtAssign2017/' 
file_name = dir1+"EMTF_MC_NTuple_Run3stubs.root"
evt_tree.Add(file_name)

## ============== Initialise histograms ================
h_patterns_ME1 = TH1D('h_patterns_ME1', '', 11, 0, 11)
h_patterns_ME2 = TH1D('h_patterns_ME2', '', 11, 0, 11)
h_patterns_ME3 = TH1D('h_patterns_ME3', '', 11, 0, 11)
h_patterns_ME4 = TH1D('h_patterns_ME4', '', 11, 0, 11)

h_dPhi12 = TH1D('h_dPhi12', '', 64, -0.15, 0.15)
h_dPhi23 = TH1D('h_dPhi23', '', 64, -0.15, 0.15)
h_dPhi34 = TH1D('h_dPhi34', '', 64, -0.15, 0.15)
h_dTheta12 = TH1D('h_dTheta12', '', 64, -0.1, 0.1)
h_dTheta23 = TH1D('h_dTheta23', '', 64, -0.1, 0.1)
h_dTheta34 = TH1D('h_dTheta34', '', 64, -0.1, 0.1)

h_dPhi12_dPhi23 = TH2D('h_dPhi12_dPhi23', '', 32, -0.005, 0.005, 32, -0.005, 0.005)
h_dPhi12_dPhi34 = TH2D('h_dPhi12_dPhi34', '', 32, -0.005, 0.005, 32, -0.005, 0.005)
h_dPhi23_dPhi34 = TH2D('h_dPhi23_dPhi34', '', 32, -0.005, 0.005, 32, -0.005, 0.005)

h_dPhi12_pt = TH2D('h_dPhi12_pt', '', 64, 1, 50, 64, -0.15, 0.15)
h_dPhi23_pt = TH2D('h_dPhi23_pt', '', 64, 1, 50, 64, -0.15, 0.15)
h_dPhi34_pt = TH2D('h_dPhi34_pt', '', 64, 1, 50, 64, -0.15, 0.15)
h_dTheta12_pt = TH2D('h_dTheta12_pt', '', 64, 1, 50, 64, -0.15, 0.15)
h_dTheta23_pt = TH2D('h_dTheta23_pt', '', 64, 1, 50, 64, -0.15, 0.15)
h_dTheta34_pt = TH2D('h_dTheta34_pt', '', 64, 1, 50, 64, -0.15, 0.15)

h_dPhi12_patternME1 = TH2D('h_dPhi12_patternME1', '', 11, 0, 11, 32, -0.15, 0.15)
h_dPhi12_patternME2 = TH2D('h_dPhi12_patternME2', '', 11, 0, 11, 32, -0.15, 0.15)
h_dPhi23_patternME2 = TH2D('h_dPhi23_patternME2', '', 11, 0, 11, 32, -0.15, 0.15)
h_dPhi23_patternME3 = TH2D('h_dPhi23_patternME3', '', 11, 0, 11, 32, -0.15, 0.15)
h_dPhi34_patternME3 = TH2D('h_dPhi34_patternME3', '', 11, 0, 11, 32, -0.15, 0.15)
h_dPhi34_patternME4 = TH2D('h_dPhi34_patternME4', '', 11, 0, 11, 32, -0.15, 0.15)

## ============== Event loop ================

for iEvt in range(2000000):
#for iEvt in range(evt_tree.GetEntries()):
  if MAX_EVT > 0 and iEvt > MAX_EVT: break
  if iEvt % PRT_EVT is 0: print 'Event #', iEvt
  
  evt_tree.GetEntry(iEvt)

  #Write a short code that picks out specific station LCTs and subtract to get position bending.
  #Keep track of 
  ME1_phi_p=0 ; ME2_phi_p=0 ; ME3_phi_p=0 ; ME4_phi_p=0
  ME1_phi_n=0 ; ME2_phi_n=0 ; ME3_phi_n=0 ; ME4_phi_n=0
  '''
  pattern_ME1_p=0 ; pattern_ME2_p=0 ; pattern_ME3_p=0 ; pattern_ME4_p=0
  pattern_ME1_n=0 ; pattern_ME2_n=0 ; pattern_ME3_n=0 ; pattern_ME4_n=0
  ME1_theta_p=0 ; ME2_theta_p=0 ; ME3_theta_p=0 ; ME4_theta_p=0
  ME1_theta_n=0 ; ME2_theta_n=0 ; ME3_theta_n=0 ; ME4_theta_n=0
  '''
  for j in range(2):
    for i in range(len(evt_tree.hit_station)):
      #If running over low pT muons, uncomment pT cut.
      if evt_tree.hit_neighbor[i]==0 and evt_tree.hit_isCSC[i]==1 and evt_tree.hit_eta[i]*evt_tree.mu_eta[j]>0:# and evt_tree.mu_pt[j]<=15:

	#Phi-position values
	if evt_tree.hit_station[i]==1:
	  if evt_tree.hit_endcap[i]==1: ME1_phi_p = evt_tree.hit_phi[i]
	  if evt_tree.hit_endcap[i]==-1: ME1_phi_n = evt_tree.hit_phi[i]
	if evt_tree.hit_station[i]==2: 
	  if evt_tree.hit_endcap[i]==1: ME2_phi_p = evt_tree.hit_phi[i]
	  if evt_tree.hit_endcap[i]==-1: ME2_phi_n = evt_tree.hit_phi[i]
	if evt_tree.hit_station[i]==3: 
	  if evt_tree.hit_endcap[i]==1: ME3_phi_p = evt_tree.hit_phi[i]
	  if evt_tree.hit_endcap[i]==-1: ME3_phi_n = evt_tree.hit_phi[i]
	if evt_tree.hit_station[i]==4:
	  if evt_tree.hit_endcap[i]==1: ME4_phi_p = evt_tree.hit_phi[i]
	  if evt_tree.hit_endcap[i]==-1: ME4_phi_n = evt_tree.hit_phi[i]

	
	'''
	#Theta-position values
	if evt_tree.hit_station[i]==1:
	  if evt_tree.hit_endcap[i]==1: ME1_theta_p = evt_tree.hit_theta[i]
	  if evt_tree.hit_endcap[i]==-1: ME1_theta_n = evt_tree.hit_theta[i]
	if evt_tree.hit_station[i]==2:
	  if evt_tree.hit_endcap[i]==1: ME2_theta_p = evt_tree.hit_theta[i]
	  if evt_tree.hit_endcap[i]==-1: ME2_theta_n = evt_tree.hit_theta[i]
	if evt_tree.hit_station[i]==3:
	  if evt_tree.hit_endcap[i]==1: ME3_theta_p = evt_tree.hit_theta[i]
	  if evt_tree.hit_endcap[i]==-1: ME3_theta_n = evt_tree.hit_theta[i]
	if evt_tree.hit_station[i]==4: 
	  if evt_tree.hit_endcap[i]==1: ME4_theta_p = evt_tree.hit_theta[i]
	  if evt_tree.hit_endcap[i]==-1: ME4_theta_n = evt_tree.hit_theta[i]
	
	#Patterns
	if evt_tree.hit_neighbor[i]==0 and evt_tree.hit_isCSC[i]==1:
	  if evt_tree.hit_station[i]==1: 
	    if evt_tree.hit_endcap[i]==1: pattern_ME1_p=evt_tree.hit_pattern[i]
	    if evt_tree.hit_endcap[i]==-1: pattern_ME1_n=evt_tree.hit_pattern[i]
	  if evt_tree.hit_station[i]==2:
	    if evt_tree.hit_endcap[i]==1: pattern_ME2_p=evt_tree.hit_pattern[i]
	    if evt_tree.hit_endcap[i]==-1: pattern_ME2_n=evt_tree.hit_pattern[i]
	  if evt_tree.hit_station[i]==3: 
	    if evt_tree.hit_endcap[i]==1: pattern_ME3_p=evt_tree.hit_pattern[i]
	    if evt_tree.hit_endcap[i]==-1: pattern_ME3_n=evt_tree.hit_pattern[i]
	  if evt_tree.hit_station[i]==4: if
	    if evt_tree.hit_endcap[i]==1: pattern_ME4_p=evt_tree.hit_pattern[i]
	    if evt_tree.hit_endcap[i]==-1: pattern_ME4_n=evt_tree.hit_pattern[i]

	#if evt_tree.hit_isCSC[i]==1 and evt_tree.hit_station[i]==1: h_patterns_ME1.Fill(evt_tree.hit_pattern[i])
	#if evt_tree.hit_isCSC[i]==1 and evt_tree.hit_station[i]==2: h_patterns_ME2.Fill(evt_tree.hit_pattern[i])
	#if evt_tree.hit_isCSC[i]==1 and evt_tree.hit_station[i]==3: h_patterns_ME3.Fill(evt_tree.hit_pattern[i])
	#if evt_tree.hit_isCSC[i]==1 and evt_tree.hit_station[i]==4: h_patterns_ME4.Fill(evt_tree.hit_pattern[i])
	'''

  ##2D correlations for phi-position bendings
  #If the stubs in each endcap are well defined, fill the TH2D object.
  if (ME1_phi_p * ME2_phi_p * ME3_phi_p)!=0: h_dPhi12_dPhi23.Fill((ME2_phi_p - ME3_phi_p)*np.pi/180., (ME1_phi_p - ME2_phi_p)*np.pi/180.)
  if (ME1_phi_n * ME2_phi_n * ME3_phi_n)!=0: h_dPhi12_dPhi23.Fill((ME2_phi_n - ME3_phi_n)*np.pi/180., (ME1_phi_n - ME2_phi_n)*np.pi/180.)
  if (ME2_phi_p * ME3_phi_p * ME4_phi_p)!=0: h_dPhi23_dPhi34.Fill((ME3_phi_p - ME4_phi_p)*np.pi/180., (ME2_phi_p - ME3_phi_p)*np.pi/180.)
  if (ME2_phi_n * ME3_phi_n * ME4_phi_n)!=0: h_dPhi23_dPhi34.Fill((ME3_phi_n - ME4_phi_n)*np.pi/180., (ME2_phi_n - ME3_phi_n)*np.pi/180.)
  if (ME1_phi_p * ME2_phi_p * ME3_phi_p)!=0 and ME4_phi_p!=0: h_dPhi12_dPhi34.Fill((ME3_phi_p - ME4_phi_p)*np.pi/180., (ME1_phi_p - ME2_phi_p)*np.pi/180.)
  if (ME1_phi_n * ME2_phi_n * ME3_phi_n)!=0 and ME4_phi_n!=0: h_dPhi12_dPhi34.Fill((ME3_phi_n - ME4_phi_n)*np.pi/180., (ME1_phi_n - ME2_phi_n)*np.pi/180.)

  '''
  #2D correlations for phi-postion vs patterns
  if (ME1_phi_p * ME2_phi_p * pattern_ME1_p)!=0: h_dPhi12_patternME1.Fill(pattern_ME1_p, (ME1_phi_p - ME2_phi_p)*np.pi/180.)
  if (ME1_phi_p * ME2_phi_p * pattern_ME2_p)!=0: h_dPhi12_patternME2.Fill(pattern_ME2_p, (ME1_phi_p - ME2_phi_p)*np.pi/180.)
  if (ME1_phi_n * ME2_phi_n * pattern_ME1_n)!=0: h_dPhi12_patternME1.Fill(pattern_ME1_n, (ME1_phi_n - ME2_phi_n)*np.pi/180.)
  if (ME1_phi_n * ME2_phi_n * pattern_ME2_n)!=0: h_dPhi12_patternME2.Fill(pattern_ME2_n, (ME1_phi_n - ME2_phi_n)*np.pi/180.)
  if (ME2_phi_p * ME3_phi_p * pattern_ME2_p)!=0: h_dPhi23_patternME2.Fill(pattern_ME2_p, (ME2_phi_p - ME3_phi_p)*np.pi/180.)
  if (ME2_phi_p * ME3_phi_p * pattern_ME3_p)!=0: h_dPhi23_patternME3.Fill(pattern_ME3_p, (ME2_phi_p - ME3_phi_p)*np.pi/180.)
  if (ME2_phi_n * ME3_phi_n * pattern_ME2_n)!=0: h_dPhi23_patternME2.Fill(pattern_ME2_n, (ME2_phi_n - ME3_phi_n)*np.pi/180.)
  if (ME2_phi_n * ME3_phi_n * pattern_ME3_n)!=0: h_dPhi23_patternME3.Fill(pattern_ME3_n, (ME2_phi_n - ME3_phi_n)*np.pi/180.)
  if (ME3_phi_p * ME4_phi_p * pattern_ME3_p)!=0: h_dPhi34_patternME3.Fill(pattern_ME3_p, (ME3_phi_p - ME4_phi_p)*np.pi/180.)
  if (ME3_phi_p * ME4_phi_p * pattern_ME4_p)!=0: h_dPhi34_patternME4.Fill(pattern_ME4_p, (ME3_phi_p - ME4_phi_p)*np.pi/180.)
  if (ME3_phi_n * ME4_phi_n * pattern_ME3_n)!=0: h_dPhi34_patternME3.Fill(pattern_ME3_n, (ME3_phi_n - ME4_phi_n)*np.pi/180.)
  if (ME3_phi_n * ME4_phi_n * pattern_ME4_n)!=0: h_dPhi34_patternME4.Fill(pattern_ME4_n, (ME3_phi_n - ME4_phi_n)*np.pi/180.)


  #Fill dPhi bendings into 1D histograms.
  if (ME1_phi_p * ME2_phi_p)!=0: h_dPhi12.Fill((ME1_phi_p - ME2_phi_p)*np.pi/180.)
  if (ME1_phi_n * ME2_phi_n)!=0: h_dPhi12.Fill((ME1_phi_n - ME2_phi_n)*np.pi/180.)
  if (ME2_phi_p * ME3_phi_p)!=0: h_dPhi23.Fill((ME2_phi_p - ME3_phi_p)*np.pi/180.)
  if (ME2_phi_n * ME3_phi_n)!=0: h_dPhi23.Fill((ME2_phi_n - ME3_phi_n)*np.pi/180.)
  if (ME3_phi_p * ME4_phi_p)!=0: h_dPhi34.Fill((ME3_phi_p - ME4_phi_p)*np.pi/180.)
  if (ME3_phi_n * ME4_phi_n)!=0: h_dPhi34.Fill((ME3_phi_n - ME4_phi_n)*np.pi/180.)

  
  #dTheta bendings 1D histograms.
  if (ME1_theta_p * ME2_theta_p)!=0: h_dTheta12.Fill((ME1_theta_p - ME2_theta_p)*np.pi/180.)
  if (ME1_theta_n * ME2_theta_n)!=0: h_dTheta12.Fill((ME1_theta_n - ME2_theta_n)*np.pi/180.)
  if (ME2_theta_p * ME3_theta_p)!=0: h_dTheta23.Fill((ME2_theta_p - ME3_theta_p)*np.pi/180.)
  if (ME2_theta_n * ME3_theta_n)!=0: h_dTheta23.Fill((ME2_theta_n - ME3_theta_n)*np.pi/180.)
  if (ME3_theta_p * ME4_theta_p)!=0: h_dTheta34.Fill((ME3_theta_p - ME4_theta_p)*np.pi/180.)
  if (ME3_theta_n * ME4_theta_n)!=0: h_dTheta34.Fill((ME3_theta_n - ME4_theta_n)*np.pi/180.)
  
  #Pick out pT of muons in (+) and (-) endcap
  for i in range(len(evt_tree.mu_pt)):
    if evt_tree.mu_eta[i]>0 and evt_tree.mu_pt[i]<=15: gen_pt_p = evt_tree.mu_pt[i]
    if evt_tree.mu_eta[i]<0 and evt_tree.mu_pt[i]<=15: gen_pt_n = evt_tree.mu_pt[i]

  #Phi-position bending vs pT 2D histograms.
  if (ME1_phi_p * ME2_phi_p)!=0: h_dPhi12_pt.Fill(gen_pt_p, (ME1_phi_p - ME2_phi_p)*np.pi/180.)
  if (ME1_phi_n * ME2_phi_n)!=0: h_dPhi12_pt.Fill(gen_pt_n, (ME1_phi_n - ME2_phi_n)*np.pi/180.)
  if (ME2_phi_p * ME3_phi_p)!=0: h_dPhi23_pt.Fill(gen_pt_p, (ME2_phi_p - ME3_phi_p)*np.pi/180.)
  if (ME2_phi_n * ME3_phi_n)!=0: h_dPhi23_pt.Fill(gen_pt_n, (ME2_phi_n - ME3_phi_n)*np.pi/180.)
  if (ME3_phi_p * ME4_phi_p)!=0: h_dPhi34_pt.Fill(gen_pt_p, (ME3_phi_p - ME4_phi_p)*np.pi/180.)
  if (ME3_phi_n * ME4_phi_n)!=0: h_dPhi34_pt.Fill(gen_pt_n, (ME3_phi_n - ME4_phi_n)*np.pi/180.)
  
  #Theta-position bending vs pT 2D histogram
  if (ME1_theta_p * ME2_theta_p)!=0: h_dTheta12_pt.Fill(gen_pt_p, (ME1_theta_p - ME2_theta_p)*np.pi/180.)
  if (ME1_theta_n * ME2_theta_n)!=0: h_dTheta12_pt.Fill(gen_pt_n, (ME1_theta_n - ME2_theta_n)*np.pi/180.)
  if (ME2_theta_p * ME3_theta_p)!=0: h_dTheta23_pt.Fill(gen_pt_p, (ME2_theta_p - ME3_theta_p)*np.pi/180.)
  if (ME2_theta_n * ME3_theta_n)!=0: h_dTheta23_pt.Fill(gen_pt_n, (ME2_theta_n - ME3_theta_n)*np.pi/180.)
  if (ME3_theta_p * ME4_theta_p)!=0: h_dTheta34_pt.Fill(gen_pt_p, (ME3_theta_p - ME4_theta_p)*np.pi/180.)
  if (ME3_theta_n * ME4_theta_n)!=0: h_dTheta34_pt.Fill(gen_pt_n, (ME3_theta_n - ME4_theta_n)*np.pi/180.)
  '''


#-------------------------------------
#2D correlations of phi-position bendings
#-------------------------------------
c1 = TCanvas( 'c1', '', 200, 10, 700, 500)
h_dPhi12_dPhi23.Draw("colz")
gStyle.SetOptStat(0)
gPad.SetLogz()
h_dPhi12_dPhi23.SetTitle('dPhi_{12} vs dPhi_{23}')
h_dPhi12_dPhi23.GetXaxis().SetTitle('dPhi_{23} (radians)') ; h_dPhi12_dPhi23.GetYaxis().SetTitle('dPhi_{12} (radians)')
h_dPhi12_dPhi23.Write()
c1.SaveAs('validation_november/dPhi12_dPhi23.png')
#raw_input("Enter")
c1.Close()

c1 = TCanvas( 'c1', '', 200, 10, 700, 500)
h_dPhi12_dPhi34.Draw("colz")
gStyle.SetOptStat(0)
gPad.SetLogz()
h_dPhi12_dPhi34.SetTitle('dPhi_{12} vs dPhi_{34}')
h_dPhi12_dPhi34.GetXaxis().SetTitle('dPhi_{34} (radians)') ; h_dPhi12_dPhi34.GetYaxis().SetTitle('dPhi_{12} (radians)')
h_dPhi12_dPhi34.Write()
c1.SaveAs('validation_november/dPhi12_dPhi34.png')
#raw_input("Enter")
c1.Close()

c1 = TCanvas( 'c1', '', 200, 10, 700, 500)
h_dPhi23_dPhi34.Draw("colz")
gStyle.SetOptStat(0)
gPad.SetLogz()
h_dPhi23_dPhi34.SetTitle('dPhi_{23} vs dPhi_{34}')
h_dPhi23_dPhi34.GetXaxis().SetTitle('dPhi_{34} (radians)') ; h_dPhi23_dPhi34.GetYaxis().SetTitle('dPhi_{23} (radians)')
h_dPhi23_dPhi34.Write()
c1.SaveAs('validation_november/dPhi23_dPhi34.png')
#raw_input("Enter")
c1.Close()


'''
#-------------------------------------
#2D correlations of phi-position bending and pattern
#-------------------------------------
c1 = TCanvas( 'c1', '', 200, 10, 700, 500)
h_dPhi12_patternME1.Draw("colz")
gStyle.SetOptStat(0)
gPad.SetLogz()
h_dPhi12_patternME1.SetTitle('dPhi_{12} vs ME1 CLCT Pattern')
h_dPhi12_patternME1.GetXaxis().SetTitle('Pattern') ; h_dPhi12_patternME1.GetYaxis().SetTitle('dPhi_{12} (radians)')
h_dPhi12_patternME1.Write()
c1.SaveAs('validation_november/dPhi12_patternME1_pt15.png')
#raw_input("Enter")
c1.Close()

c1 = TCanvas( 'c1', '', 200, 10, 700, 500)
h_dPhi12_patternME2.Draw("colz")
gStyle.SetOptStat(0)
gPad.SetLogz()
h_dPhi12_patternME2.SetTitle('dPhi_{12} vs ME2 CLCT Pattern')
h_dPhi12_patternME2.GetXaxis().SetTitle('Pattern') ; h_dPhi12_patternME2.GetYaxis().SetTitle('dPhi_{12} (radians)')
h_dPhi12_patternME2.Write()
c1.SaveAs('validation_november/dPhi12_patternME2_pt15.png')
#raw_input("Enter")
c1.Close()

c1 = TCanvas( 'c1', '', 200, 10, 700, 500)
h_dPhi23_patternME2.Draw("colz")
gStyle.SetOptStat(0)
gPad.SetLogz()
h_dPhi23_patternME2.SetTitle('dPhi_{23} vs ME2 CLCT Pattern')
h_dPhi23_patternME2.GetXaxis().SetTitle('Pattern') ; h_dPhi23_patternME2.GetYaxis().SetTitle('dPhi_{23} (radians)')
h_dPhi23_patternME2.Write()
c1.SaveAs('validation_november/dPhi23_patternME2_pt15.png')
#raw_input("Enter")
c1.Close()

c1 = TCanvas( 'c1', '', 200, 10, 700, 500)
h_dPhi23_patternME3.Draw("colz")
gStyle.SetOptStat(0)
gPad.SetLogz()
h_dPhi23_patternME3.SetTitle('dPhi_{23} vs ME3 CLCT Pattern')
h_dPhi23_patternME3.GetXaxis().SetTitle('Pattern') ; h_dPhi23_patternME3.GetYaxis().SetTitle('dPhi_{23} (radians)')
h_dPhi23_patternME3.Write()
c1.SaveAs('validation_november/dPhi23_patternME3_pt15.png')
#raw_input("Enter")
c1.Close()

c1 = TCanvas( 'c1', '', 200, 10, 700, 500)
h_dPhi34_patternME3.Draw("colz")
gStyle.SetOptStat(0)
gPad.SetLogz()
h_dPhi34_patternME3.SetTitle('dPhi_{34} vs ME3 CLCT Pattern')
h_dPhi34_patternME3.GetXaxis().SetTitle('Pattern') ; h_dPhi34_patternME3.GetYaxis().SetTitle('dPhi_{34} (radians)')
h_dPhi34_patternME3.Write()
c1.SaveAs('validation_november/dPhi34_patternME3_pt15.png')
#raw_input("Enter")
c1.Close()

c1 = TCanvas( 'c1', '', 200, 10, 700, 500)
h_dPhi34_patternME4.Draw("colz")
gStyle.SetOptStat(0)
gPad.SetLogz()
h_dPhi34_patternME4.SetTitle('dPhi_{34} vs ME4 CLCT Pattern')
h_dPhi34_patternME4.GetXaxis().SetTitle('Pattern') ; h_dPhi34_patternME4.GetYaxis().SetTitle('dPhi_{34} (radians)')
h_dPhi34_patternME4.Write()
c1.SaveAs('validation_november/dPhi34_patternME4_pt15.png')
#raw_input("Enter")
c1.Close()

#-------------------------------------
#2D Correlations of dPhi position bend and pT.
#-------------------------------------
c1 = TCanvas( 'c1', '', 200, 10, 700, 500)
h_dPhi12_pt.Draw("colz")
gStyle.SetOptStat(0)
h_dPhi12_pt.SetTitle('dPhi_{12} vs p_{T}^{GEN}')
h_dPhi12_pt.GetXaxis().SetTitle('p_{T} (GeV)') ; h_dPhi12_pt.GetYaxis().SetTitle('dPhi_{12} (radians)')
h_dPhi12_pt.SetOption("colz")
h_dPhi12_pt.Write()
c1.SaveAs('validation_november/dPhi12_pt.png')
#raw_input("Enter")

c1 = TCanvas( 'c1', '', 200, 10, 700, 500)
h_dPhi23_pt.Draw("colz")
gStyle.SetOptStat(0)
h_dPhi23_pt.SetTitle('dPhi_{23} vs p_{T}^{GEN}')
h_dPhi23_pt.GetXaxis().SetTitle('p_{T} (GeV)') ; h_dPhi23_pt.GetYaxis().SetTitle('dPhi_{23} (radians)')
h_dPhi23_pt.SetOption("colz")
h_dPhi23_pt.Write()
c1.SaveAs('validation_november/dPhi23_pt.png')
#raw_input("Enter")

c1 = TCanvas( 'c1', '', 200, 10, 700, 500)
h_dPhi34_pt.Draw("colz")
gStyle.SetOptStat(0)
h_dPhi34_pt.SetTitle('dPhi_{34} vs p_{T}^{GEN}')
h_dPhi34_pt.GetXaxis().SetTitle('p_{T} (GeV)') ; h_dPhi34_pt.GetYaxis().SetTitle('dPhi_{34} (radians)')
h_dPhi34_pt.SetOption("colz")
h_dPhi34_pt.Write()
c1.SaveAs('validation_november/dPhi34_pt.png')
#raw_input("Enter")


#-------------------------------------
#2D Correlations of dTheta and pT.
#-------------------------------------
c1 = TCanvas( 'c1', '', 200, 10, 700, 500)
h_dTheta12_pt.Draw("colz")
gStyle.SetOptStat(0)
h_dTheta12_pt.SetTitle('dTheta_{12} vs p_{T}^{GEN}')
h_dTheta12_pt.GetXaxis().SetTitle('p_{T} (GeV)') ; h_dTheta12_pt.GetYaxis().SetTitle('dTheta_{12} (radians)')
h_dTheta12_pt.SetOption("colz")
h_dTheta12_pt.Write()
c1.SaveAs('validation_november/dTheta12_pt.png')
#raw_input("Enter")

c1 = TCanvas( 'c1', '', 200, 10, 700, 500)
h_dTheta23_pt.Draw("colz")
gStyle.SetOptStat(0)
h_dTheta23_pt.SetTitle('dTheta_{23} vs p_{T}^{GEN}')
h_dTheta23_pt.GetXaxis().SetTitle('p_{T} (GeV)') ; h_dTheta23_pt.GetYaxis().SetTitle('dTheta_{23} (radians)')
h_dTheta23_pt.SetOption("colz")
h_dTheta23_pt.Write()
c1.SaveAs('validation_november/dTheta23_pt.png')
#raw_input("Enter")

c1 = TCanvas( 'c1', '', 200, 10, 700, 500)
h_dTheta34_pt.Draw("colz")
gStyle.SetOptStat(0)
h_dTheta34_pt.SetTitle('dTheta_{34} vs p_{T}^{GEN}')
h_dTheta34_pt.GetXaxis().SetTitle('p_{T} (GeV)') ; h_dTheta34_pt.GetYaxis().SetTitle('dTheta_{34} (radians)')
h_dTheta34_pt.SetOption("colz")
h_dTheta34_pt.Write()
c1.SaveAs('validation_november/dTheta34_pt.png')
#raw_input("Enter")



#-------------------------------------
#1D Histograms
#-------------------------------------
#c1 = TCanvas( 'c1', '', 200, 10, 700, 500)
#h_dTheta12.Draw()
#h_dTheta12.SetTitle('dTheta_{12} (radians)')
#c1.SaveAs('validation_november/dTheta12.png')
##raw_input("Enter")

#c1 = TCanvas( 'c1', '', 200, 10, 700, 500)
#h_dTheta23.Draw()
#h_dTheta23.SetTitle('dTheta_{23} (radians)')
#c1.SaveAs('validation_november/dTheta23.png')
##raw_input("Enter")

#c1 = TCanvas( 'c1', '', 200, 10, 700, 500)
#h_dTheta34.Draw()
#h_dTheta34.SetTitle('dTheta_{34} (radians)')
#c1.SaveAs('validation_november/dTheta34.png')
##raw_input("Enter")

c1 = TCanvas( 'c1', '', 200, 10, 700, 500)
h_mode.Draw()
h_mode.SetTitle('Track mode occupancy (p_{T}^{GEN} < 5 GeV)')
c1.SaveAs('validation_november/mode_pt5.png')

#Plots for CLCT patterns per ME station
c1 = TCanvas( 'c1', '', 200, 10, 700, 500)
h_patterns_ME1.Draw()
h_patterns_ME1.SetTitle('ME1 CLCT pattern occupany (p_{T}^{GEN} < 5 GeV)')
c1.SaveAs('validation_november/patterns_ME1_pt5.png')
#raw_input("Enter")

c1 = TCanvas( 'c1', '', 200, 10, 700, 500)
h_patterns_ME2.Draw()
h_patterns_ME2.SetTitle('ME2 CLCT pattern occupany (p_{T}^{GEN} < 5 GeV)')
c1.SaveAs('validation_november/patterns_ME2_pt5.png')
#raw_input("Enter")

c1 = TCanvas( 'c1', '', 200, 10, 700, 500)
h_patterns_ME3.Draw()
h_patterns_ME3.SetTitle('ME3 CLCT pattern occupany (p_{T}^{GEN} < 5 GeV)')
c1.SaveAs('validation_november/patterns_ME3_pt5.png')
#raw_input("Enter")

c1 = TCanvas( 'c1', '', 200, 10, 700, 500)
h_patterns_ME4.Draw()
h_patterns_ME4.SetTitle('ME4 CLCT pattern occupany (p_{T}^{GEN} < 5 GeV)')
c1.SaveAs('validation_november/patterns_ME4_pt5.png')
#raw_input("Enter")
'''
