#include <iostream>
#include <iomanip>
using namespace std;
#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TAttFill.h"
#include "TCanvas.h"
#include <vector>
#include "stdio.h"
#include <stdlib.h>
#include "math.h"
#include "TMath.h"
#include "TGraph.h"
#include "TLegend.h"
#include "TPaveStats.h"
#include "TStyle.h"
#include "THStack.h"
#include "TFitResultPtr.h"

//****************************************************************************************
//*Study the pT classifier performance and compare with BDT regression normally used at P5
//*including ROC cureve, signal-to-background ratio, etc
//*
//*Wei Shi @ Nov 11, 2017 CERN Geneva
//****************************************************************************************
//*Reference Table
//*=======================================================================================
//*         Test MC Events   #                     |  GEN < PT_CUT    |    GEN >= PT_CUT
//*======================================================================================
//*MC True: Signal (class1)                        |       NO         |         S
//*---------------------------------------------------------------------------------------
//*MC True: Background (class2)                    |       B          |         NO
//*=======================================================================================
//*Cut: class1 >= a (Signal)                       |       S1         |         S2
//*---------------------------------------------------------------------------------------
//*Cut: class1 < a (Background)                    |       B1         |         B2
//*=======================================================================================
//*In binary classifier, two classes cut "class1>=a && class2<=b" is redundant. Since it 
//*only make sense to have b<=1-a, a>=1-b, this implies "a+b=1", ie, cut on one class is 
//*sufficienct. As "a" decreases, efficiency increase as well as rate(monotonic increase)
//*Use Eff_REF as the stop point for further decreasing "a";
//*True Postive Rate: S2/(S2+B2), i.e. S2/S, signal efficiency/plateau trigger efficiency
//*False Positive Rate: S1/(S1+B1)
//*S=S2+B2
//*B=S1+B1
void ClassifierROC()
{
        //USER modify here ONLY//
        //================================================================
        Int_t PT_CUT = 32;//the classifier trained on this cut
        Float_t EFF_REF = 0.95;//the eff beyond which classifier cut stops
        Int_t Bins=100;//bins on class cut
        Int_t lxplus=1;//machine: lxplus(1) or bonner(0)?
        //================================================================
        TString Cluster="";
        if(lxplus==1){
                Cluster = "/afs/cern.ch/work/w/wshi/public/TMVA_2017/TMVA/";
        }
        else{
                Cluster = "/home/ws13/TMVA/TMVA/";//bonner
        }
        TString fileName = Cluster + "EMTFPtAssign2017/pTMulticlass_MODE_15_bitCompr_RPC_" + Form("%d", PT_CUT) + ".root";
        TString directoryName = "f_MODE_15_noWgt_bitCompr_RPC/TestTree";
        TFile* myFile = new TFile(fileName);
        TTree* myTree = (TTree*) myFile->Get(directoryName);
        
        cout<<"Accessing file:"<<fileName<<endl;
        cout<<"Accessing directory:"<<directoryName<<endl;
        
        TBranch *GEN_pt_br = myTree->GetBranch("GEN_pt");
        TBranch *GEN_charge_br = myTree->GetBranch("GEN_charge");
        TBranch *BDTG_br = myTree->GetBranch("BDTG");
        TBranch *TRK_mode_RPC_br = myTree->GetBranch("TRK_mode_RPC");
        
        double a=1.0;
        double b=0.0;//b is defined but not used in the cut, b==1-a;
        double BIT=0.000001;//in case b become very small positive number
        Long64_t MinRATE=9999;
        Long64_t RATE16=0;//reg pT cut 16 GeV rate
        double OptA=a;//best cut with min rate while high efficiency(>reference eff)
        double OptB=b;
        Int_t fill=0;//only fill 2 classes topology one time
        Int_t flag=1;//mark to stop increase b after reach EFF_REF
        
        auto ROC = new TProfile("ROC","ROC Curve",100,0,1,0,1);
        auto EFFvsCUTs = new TProfile("Efficiency","Signal Efficiency vs Cuts",Bins,0,1,0,1);
        TString RATEvsCUTsTitle="";
        RATEvsCUTsTitle = RATEvsCUTsTitle + "RATE vs Cuts (Eff > "+Form("%0.2lf", EFF_REF) + ")";
        auto RATEvsCUTs = new TProfile("RATE", RATEvsCUTsTitle, Bins, 0, 1, 0, 10000);
        TH2F *Topology = new TH2F("Topology", "Class2 vs Class1", 100, 0, 1, 100, 0, 1);
        
        Long64_t numEvents = myTree->GetEntries();
        cout<<">>>>>>>>>>>>>>>>>>>>>"<<endl;
        cout<<numEvents<<" events to process..."<<endl;
      
        //loop over cut on class1
        while(a>BIT && flag==1){
          
          a = a - 1.0/Bins;//update cut on class1
          b = 1.0 - a;//store b, b is not used in cut
                
          Long64_t S1=0;
          Long64_t S2=0;
          Long64_t B1=0;
          Long64_t B2=0;
          double TPR=-1.0;
          double FPR=-1.0;
          Long64_t RATE=0;
          
          for(Long64_t iEntry = 0; iEntry <numEvents; iEntry++){
              
              myTree->GetEntry(iEntry);
                    
              //access leaves under branch
              Float_t GEN_pt = (GEN_pt_br->GetLeaf("GEN_pt"))->GetValue();
              Float_t GEN_charge = (GEN_charge_br->GetLeaf("GEN_charge"))->GetValue();
              Float_t BDTG_class1 = (BDTG_br->GetLeaf("class1"))->GetValue();
              Float_t BDTG_class2 = (BDTG_br->GetLeaf("class2"))->GetValue();//not used in the cut
                
              if(fill==0){
                Topology->Fill(BDTG_class1,BDTG_class2);//sanity check off diagnoal: class2=1-class1;
              }
                  
              if(GEN_charge > -2 && GEN_pt >= PT_CUT && BDTG_class1 >= a){S2=S2+1;}
              if(GEN_charge > -2 && GEN_pt < PT_CUT && BDTG_class1 >= a){S1=S1+1;}
              if(GEN_charge > -2 && GEN_pt >= PT_CUT && BDTG_class1 < a){B2=B2+1;}
              if(GEN_charge > -2 && GEN_pt < PT_CUT && BDTG_class1 < a){B1=B1+1;}
            
            }//end loop over events
            
            fill=1;
            //Fill ROC curve
            TPR=1.0*S2/(S2+B2);
            FPR=1.0*S1/(S1+B1);
            ROC->Fill(FPR,TPR);
                  
            //Fill Signal efficiency vs cut
            EFFvsCUTs->Fill(a,TPR);
    
            //calculate ratr once signal eff higher than EFF_REF
            if(TPR >= EFF_REF){
                    
              for(Long64_t iEntry = 0; iEntry <numEvents; iEntry++){
              
                myTree->GetEntry(iEntry);
                Float_t GEN_pt = (GEN_pt_br->GetLeaf("GEN_pt"))->GetValue();//not used in rate counts
                Float_t GEN_charge = (GEN_charge_br->GetLeaf("GEN_charge"))->GetValue();
                Float_t BDTG_class1 = (BDTG_br->GetLeaf("class1"))->GetValue();
                Float_t BDTG_class2 = (BDTG_br->GetLeaf("class2"))->GetValue();//not used in cuts
                  
                //ZB events
                if(GEN_charge < -2 && BDTG_class1 >= a){RATE=RATE+1;}//after cut
              
              }//end loop over events for rate
              
              //keep note of rate 
              if(RATE < MinRATE){
                MinRATE=RATE;
                OptA=a;
                OptB=1-OptA;
              }
              
              //fill rate vs cuts only for eff > ref_eff
              RATEvsCUTs->Fill(a,RATE);
              flag=0;
                    
            }//end if TPR higher than reference
             
            cout<<"a:"<<a<<" (b:"<<b<<") TPR:"<<TPR<<" FPR:"<<FPR<<" RATE:"<<RATE<<" S1:"<<S1<<" S2:"<<S2<<" B1:"<<B1<<" B2:"<<B2<<endl;
                  
            
        }//end loop over cut on class1     
        
        //==========================================
        //compare eff b/t regression and classifier
        //==========================================
        //regression 
        TString RegfileName = Cluster + "EMTFPtAssign2017/Regression.root";
        TString RegdirectoryName = "f_MODE_15_invPtTarg_invPtWgt_bitCompr_RPC/TestTree";
        TFile* myRegFile = new TFile(RegfileName);
        TTree* myRegTree = (TTree*) myRegFile->Get(RegdirectoryName);
        
        Long64_t RegnumEvents = myRegTree->GetEntries();
        
        TBranch *RegGEN_pt_br = myRegTree->GetBranch("GEN_pt");
        TBranch *RegGEN_charge_br = myRegTree->GetBranch("GEN_charge");
        TBranch *RegBDTG_br = myRegTree->GetBranch("BDTG_AWB_Sq");
        TBranch *RegTRK_mode_RPC_br = myRegTree->GetBranch("TRK_mode_RPC");

        //GEN pt distribution
        TH1F *RegCSConlyMC = new TH1F("RegCSConlyMC", "RegCSConlyMC", 50, 0, 10);//50 bins
        TH1F *RegCSConlyMCCut = new TH1F("RegCSConlyMCCut", "RegCSConlyMCCut", 50, 0, 10);
        TH1F *CSConlyMC = new TH1F("CSConlyMC", "CSConlyMC", 50, 0, 10);
        TH1F *CSConlyMCCut = new TH1F("CSConlyMCCut", "CSConlyMCCut", 50, 0, 10);
        
        for(Long64_t iEntry = 0; iEntry <RegnumEvents; iEntry++){
              
                myRegTree->GetEntry(iEntry);
                Float_t GEN_pt = (RegGEN_pt_br->GetLeaf("GEN_pt"))->GetValue();
                Float_t GEN_charge = (RegGEN_charge_br->GetLeaf("GEN_charge"))->GetValue();
                Float_t BDTG = (RegBDTG_br->GetLeaf("inv_GEN_pt_trg"))->GetValue();
                Float_t TRK_mode_RPC = (RegTRK_mode_RPC_br->GetLeaf("TRK_mode_RPC"))->GetValue();
                  
                //CSC-only GEN pT distributions
                if(GEN_charge > -2 && TRK_mode_RPC == 0){
                        RegCSConlyMC->Fill(TMath::Log2(GEN_pt));
                        if(1./BDTG >= 16){
                                RegCSConlyMCCut->Fill(TMath::Log2(GEN_pt));
                        }
                }
                if(GEN_charge < -2 && 1./BDTG >= 16){
                        RATE16 = RATE16 + 1;
                }
                
        }//end loop over Regression events
        
        //classifier with OptA and OptB
        for(Long64_t iEntry = 0; iEntry <numEvents; iEntry++){
              
                myTree->GetEntry(iEntry);
                Float_t GEN_pt = (GEN_pt_br->GetLeaf("GEN_pt"))->GetValue();
                Float_t GEN_charge = (GEN_charge_br->GetLeaf("GEN_charge"))->GetValue();
                Float_t BDTG_class1 = (BDTG_br->GetLeaf("class1"))->GetValue();
                Float_t BDTG_class2 = (BDTG_br->GetLeaf("class2"))->GetValue();//not used here
                Float_t TRK_mode_RPC = (TRK_mode_RPC_br->GetLeaf("TRK_mode_RPC"))->GetValue();
                  
                //CSC-only GEN pT distributions
                if(GEN_charge > -2 && TRK_mode_RPC == 0){
                        CSConlyMC->Fill(TMath::Log2(GEN_pt));
                        if(BDTG_class1 >= OptA){
                                CSConlyMCCut->Fill(TMath::Log2(GEN_pt));
                        }
                }
        }//end loop over events 
        
        cout<<">>>>>>>>>>>>>>>>>>>>>"<<endl;
        cout<<"Binary Classifier Settings: PT_CUT="<<PT_CUT<<" EFF_REF="<<EFF_REF<<" Bins:"<<Bins<<endl;
        cout<<"OptA:"<<OptA<<" (OptB:"<<OptB<<") RATE:"<<MinRATE<<endl;
        
        //write to output file
        TString outFile = Cluster + "EMTFPtAssign2017/ClassifierROC_" + Form("%d", PT_CUT) + "_" + Form("%0.2lf", EFF_REF) + "_"+ Form("%d", Bins)+".root";
        TFile myPlot(outFile,"RECREATE");
        
        ROC->GetXaxis()->SetTitle("FPR");
        ROC->GetYaxis()->SetTitle("TPR");
        ROC->Write();
        EFFvsCUTs->GetXaxis()->SetTitle("class1 cut");
        EFFvsCUTs->GetYaxis()->SetTitle("efficiency");
        EFFvsCUTs->Write();
        RATEvsCUTs->GetXaxis()->SetTitle("class1 cut");
        RATEvsCUTs->GetYaxis()->SetTitle("ZeroBias rate");
        RATEvsCUTs->Write();
        Topology->GetXaxis()->SetTitle("class1");
        Topology->GetYaxis()->SetTitle("class2");
        Topology->Write();
        
        TCanvas *C1=new TCanvas("C1","C1",700,500);
        THStack *CSConlyGENpt = new THStack("CSConlyGENpt","CSC only GEN pt: Regression vs Classifier");
        
        C1->cd();
        RegCSConlyMC->SetLineColor(1);//black
        RegCSConlyMC->SetLineStyle(1);//solid
        RegCSConlyMC->SetLineWidth(2);
        gStyle->SetOptStat(0);
        
        RegCSConlyMCCut->SetLineColor(2);//red
        RegCSConlyMCCut->SetLineStyle(1);
        RegCSConlyMCCut->SetLineWidth(2);
        gStyle->SetOptStat(0);
        
        CSConlyMC->SetLineColor(1);//black
        CSConlyMC->SetLineStyle(2);//dash
        CSConlyMC->SetLineWidth(2);
        gStyle->SetOptStat(0);
        
        CSConlyMCCut->SetLineColor(2);//red
        CSConlyMCCut->SetLineStyle(2);//dash
        CSConlyMCCut->SetLineWidth(2);
        gStyle->SetOptStat(0);
        
        CSConlyGENpt->Add(RegCSConlyMC);
        CSConlyGENpt->Add(RegCSConlyMCCut);
        CSConlyGENpt->Add(CSConlyMC);
        CSConlyGENpt->Add(CSConlyMCCut);
        CSConlyGENpt->Draw("nostack");
        CSConlyGENpt->GetXaxis()->SetTitle("log2(GEN pT)");
        C1->Modified();
        
        TLegend* L1 = new TLegend(0.1,0.7,0.7,0.9);
        TString ClassifierL1="";
        ClassifierL1 = ClassifierL1 + "Classifier:GEN pT(class1>=" + Form("%0.4lf", OptA) + ")";
        L1->AddEntry(RegCSConlyMC, "Regression:GEN pT");
        L1->AddEntry(RegCSConlyMCCut,"Regression:GEN pT(trigger pT>=16 GeV)");
        L1->AddEntry(CSConlyMC, "Classifier:GEN pT");
        L1->AddEntry(CSConlyMCCut, ClassifierL1);
        L1->SetFillStyle(0);
        L1->SetBorderSize(0);
        L1->Draw(); 
        C1->Write();
        
        //divide histograms for eff
        TCanvas *C2=new TCanvas("C2","C2",700,500);
        THStack *CSConlyEff = new THStack("CSConlyEff","CSC only Efficiency: Regression vs Classifier");
        C2->cd();
        RegCSConlyMCCut->Divide(RegCSConlyMC);
        CSConlyMCCut->Divide(CSConlyMC);
        CSConlyEff->Add(RegCSConlyMCCut);
        CSConlyEff->Add(CSConlyMCCut);
        CSConlyEff->Draw("nostack");
        CSConlyEff->GetXaxis()->SetTitle("log2(GEN pT)");
        CSConlyEff->GetYaxis()->SetTitle("efficiency");
        C2->Modified();
        
        TLegend* L2 = new TLegend(0.1,0.7,0.7,0.9);
        TString RegL2="";
        RegL2=RegL2 + "Regression:trigger pT>=16GeV" + " Rate:"+ Form("%lld", RATE16);
        TString ClassifierL2="";
        ClassifierL2 = ClassifierL2 + "Classifier:class1>=" + Form("%0.4lf", OptA) + " Rate:"+ Form("%lld", MinRATE);
        L2->AddEntry(RegCSConlyMCCut,RegL2);
        L2->AddEntry(CSConlyMCCut, ClassifierL2);
        L2->SetFillStyle(0);
        L2->SetBorderSize(0);
        L2->Draw(); 
        C2->Write();
        
        //=========================
        //same eff turn on at 90%
        //=========================
        double Delta=1;
        //----------------------------------------------------------
        //find reg cut which gives 90% at same GEN pT as classifier 
        //----------------------------------------------------------
        Double_t CRCBinGENde[49]={0};
        Double_t CRCBinnu[49]={0};//eff for 49 bins from 1 to 50GeV
        Int_t CRCBin=0;
        
        Long64_t CRRBinGENde=0;
        Long64_t CRRBinnu[49]={0};//find reg pT cut 1-50 GeV
        Int_t CRRBin=0;//take note of Log2(pT) bin number when Reg has 90% eff at same place as Classifier
        
        Long64_t CRRATE=0;
 
        //find 90% eff GEN pT bin in classifier
        for(Long64_t iEntry = 0; iEntry <numEvents; iEntry++){
              
                myTree->GetEntry(iEntry);
                Float_t GEN_pt = (GEN_pt_br->GetLeaf("GEN_pt"))->GetValue();
                Float_t GEN_charge = (GEN_charge_br->GetLeaf("GEN_charge"))->GetValue();
                Float_t BDTG_class1 = (BDTG_br->GetLeaf("class1"))->GetValue();
                Float_t BDTG_class2 = (BDTG_br->GetLeaf("class2"))->GetValue();//not used here
                Float_t TRK_mode_RPC = (TRK_mode_RPC_br->GetLeaf("TRK_mode_RPC"))->GetValue();
                  
                //CSC-only GEN pT distributions put each event into bins they belong
                for (Int_t i=0;i<49;i++){
                        if(GEN_charge > -2 && TRK_mode_RPC == 0){
                                if( GEN_pt >= i+1 && GEN_pt < i+2 ){
                                        CRCBinGENde[i]=CRCBinGENde[i]+1;
                                        if(BDTG_class1 >= OptA){
                                               CRCBinnu[i] = CRCBinnu[i] +1;
                                        }
                                }
                                
                        }
                }//end for GEN pT bins
        }//end loop over events 
        
        for (Int_t i=0;i<49;i++){
                if(fabs(CRCBinnu[i]*1.0/CRCBinGENde[i]-0.9) <= Delta){
                        Delta = fabs(CRCBinnu[i]*1.0/CRCBinGENde[i]-0.9);
                        CRCBin = i;//take note of bin number
                }
        }//end loop for
       
        cout<<"CRC Bin#:"<<CRCBin<<" CRC GEN pT:"<<CRCBin+1<<" log2(CRC GEN pT):"<<TMath::Log2(CRCBin+1)<<" Eff:"<<CRCBinnu[CRCBin]*1.0/CRCBinGENde[CRCBin]<<" Rate:"<<MinRATE<<endl;
        
        //loop over Regression events to find 90% at CRCBin
        for(Long64_t iEntry = 0; iEntry <RegnumEvents; iEntry++){
              
                myRegTree->GetEntry(iEntry);
                Float_t GEN_pt = (RegGEN_pt_br->GetLeaf("GEN_pt"))->GetValue();
                Float_t GEN_charge = (RegGEN_charge_br->GetLeaf("GEN_charge"))->GetValue();
                Float_t BDTG = (RegBDTG_br->GetLeaf("inv_GEN_pt_trg"))->GetValue();
                Float_t TRK_mode_RPC = (RegTRK_mode_RPC_br->GetLeaf("TRK_mode_RPC"))->GetValue();
                  
                //CSC-only GEN pT distributions
                if(GEN_charge > -2 && TRK_mode_RPC == 0){
                        if( GEN_pt >= CRCBin+1 && GEN_pt < CRCBin+2 ){//90% eff at same place as Classifier
                                CRRBinGENde = CRRBinGENde +1;
                                for (Int_t i=0;i<49;i++){
                                        if(1./BDTG >= (i+1) ){
                                                CRRBinnu[i] = CRRBinnu[i] +1;
                                        }
                                }//end for loop over pT cut
                                
                        }//end if CRBin GEN
                        
                }//end CSC-only GEN 
                
        }//end loop over Regression events
        
        //calculate Reg eff under all Reg pT Cut for CRCBin
        Delta=1;
        for (Int_t i=0;i<49;i++){
                if(fabs(CRRBinnu[i]*1.0/CRRBinGENde-0.9) <= Delta){
                        Delta = fabs(CRRBinnu[i]*1.0/CRRBinGENde-0.9);
                        CRRBin = i;//take note of bin number
                }
        }//end loop over Reg pT cuts 
        
        //GEN pt distribution for same 90% turn on
        TH1F *CRRegCSConlyMCCut = new TH1F("CRRegCSConlyMCCut", "CRRegCSConlyMCCut", 50, 0, 10);
        
        //loop over regression to get the eff plot & rate
        for(Long64_t iEntry = 0; iEntry <RegnumEvents; iEntry++){
              
                myRegTree->GetEntry(iEntry);
                Float_t GEN_pt = (RegGEN_pt_br->GetLeaf("GEN_pt"))->GetValue();
                Float_t GEN_charge = (RegGEN_charge_br->GetLeaf("GEN_charge"))->GetValue();
                Float_t BDTG = (RegBDTG_br->GetLeaf("inv_GEN_pt_trg"))->GetValue();
                Float_t TRK_mode_RPC = (RegTRK_mode_RPC_br->GetLeaf("TRK_mode_RPC"))->GetValue();
                  
                //CSC-only GEN pT distributions
                if(GEN_charge > -2 && TRK_mode_RPC == 0 && 1./BDTG >= (CRRBin+1) ){
                        CRRegCSConlyMCCut->Fill(TMath::Log2(GEN_pt));
                }
                if(GEN_charge < -2 && 1./BDTG >= (CRRBin+1) ){
                        CRRATE = CRRATE + 1;
                }
                
        }//end loop over Regression events
        
        cout<<"CRR Bin#:"<<CRRBin<<" CRR pT cut:"<<CRRBin+1<<" log2(CRR pT cut):"<<TMath::Log2(CRRBin+1)<<" Eff:"<<CRRBinnu[CRRBin]*1.0/CRRBinGENde<<" CR Rate:"<<CRRATE<<endl;
        
        //divide histograms for eff
        TCanvas *C3=new TCanvas("C3","CR",700,500);
        THStack *CRCSConlyEff = new THStack("CRCSConlyEff","Regression CSC-only 90% Efficiency as Classifier");
        C3->cd();
        CRRegCSConlyMCCut->SetLineColor(2);//red
        CRRegCSConlyMCCut->SetLineStyle(1);//solid
        CRRegCSConlyMCCut->SetLineWidth(2);
        gStyle->SetOptStat(0);
        CRRegCSConlyMCCut->Divide(RegCSConlyMC);//already has RegCSConlyMC
        CRCSConlyEff->Add(CRRegCSConlyMCCut);
        CRCSConlyEff->Add(CSConlyMCCut);//alreday have
        CRCSConlyEff->Draw("nostack");
        CRCSConlyEff->GetXaxis()->SetTitle("log2(GEN pT)");
        CRCSConlyEff->GetYaxis()->SetTitle("efficiency");
        C3->Modified();
        
        TLegend* L3 = new TLegend(0.1,0.7,0.7,0.9);
        TString RegL3="";
        RegL3 = RegL3 + "Regression:trigger pT>=" + Form("%d", CRRBin+1) + "GeV Rate:"+ Form("%lld", CRRATE);
        TString ClassifierL3="";
        ClassifierL3 = ClassifierL3 + "Classifier:class1>=" + Form("%0.4lf", OptA) + " Rate:"+ Form("%lld", MinRATE);
        L3->AddEntry(CRRegCSConlyMCCut,RegL3);
        L3->AddEntry(CSConlyMCCut, ClassifierL3);
        L3->SetFillStyle(0);
        L3->SetBorderSize(0);
        L3->Draw(); 
        C3->Write();
        
        //----------------------------------------------------------
        //find classifier cut which gives 90% at same GEN pT as reg
        //----------------------------------------------------------
        Double_t RCCBinGENde=0;
        Double_t RCCBinnu[399]={0};//cut on class1 from 0-1, step is fixed here, 0.0025 step size
        Int_t RCCBin=0;
        
        Long64_t RCRBinGENde[49]={0};//eff for 49 bins from 1 to 50GeV
        Long64_t RCRBinnu[49]={0};//find reg pT cut 1-50 GeV
        Int_t RCRBin=0;//take note of Log2(pT) bin number when Reg has 90% eff at same place as Classifier
        
        Long64_t RCRATE=0;
        
        //find 90% eff GEN pT bin in regression
        for(Long64_t iEntry = 0; iEntry <RegnumEvents; iEntry++){
              
                myRegTree->GetEntry(iEntry);
                Float_t GEN_pt = (RegGEN_pt_br->GetLeaf("GEN_pt"))->GetValue();
                Float_t GEN_charge = (RegGEN_charge_br->GetLeaf("GEN_charge"))->GetValue();
                Float_t BDTG = (RegBDTG_br->GetLeaf("inv_GEN_pt_trg"))->GetValue();
                Float_t TRK_mode_RPC = (RegTRK_mode_RPC_br->GetLeaf("TRK_mode_RPC"))->GetValue();
                  
                //CSC-only GEN pT distributions put each event into bins they belong
                for (Int_t i=0;i<49;i++){
                        if(GEN_charge > -2 && TRK_mode_RPC == 0){
                                if( GEN_pt >= i+1 && GEN_pt < i+2 ){
                                        RCRBinGENde[i]=RCRBinGENde[i]+1;
                                        if(1./BDTG >= 16){//16 is the reference in reg like OptA is the ref in classifier
                                               RCRBinnu[i] = RCRBinnu[i] +1;
                                        }
                                }
                                
                        }
                }//end for GEN pT bins
        }//end loop over events 
        
        Delta=1;
        for (Int_t i=0;i<49;i++){
                if(fabs(RCRBinnu[i]*1.0/RCRBinGENde[i]-0.9) <= Delta){
                        Delta = fabs(RCRBinnu[i]*1.0/RCRBinGENde[i]-0.9);
                        RCRBin = i;//take note of bin number
                }
        }//end loop for
        cout<<"RCR Bin#:"<<RCRBin<<" RCR GEN pT:"<<RCRBin+1<<" log2(RCR GEN pT):"<<TMath::Log2(RCRBin+1)<<" Eff:"<<RCRBinnu[RCRBin]*1.0/RCRBinGENde[RCRBin]<<" RATE:"<<RATE16<<endl;
        
        //loop over Classifier events to find a cut so it is 90% at RCRBin
        for(Long64_t iEntry = 0; iEntry <numEvents; iEntry++){
              
                myTree->GetEntry(iEntry);
                Float_t GEN_pt = (GEN_pt_br->GetLeaf("GEN_pt"))->GetValue();
                Float_t GEN_charge = (GEN_charge_br->GetLeaf("GEN_charge"))->GetValue();
                Float_t BDTG_class1 = (BDTG_br->GetLeaf("class1"))->GetValue();
                Float_t BDTG_class2 = (BDTG_br->GetLeaf("class2"))->GetValue();//not used here
                Float_t TRK_mode_RPC = (TRK_mode_RPC_br->GetLeaf("TRK_mode_RPC"))->GetValue();
                  
                if(GEN_charge > -2 && TRK_mode_RPC == 0){
                        if( GEN_pt >= RCRBin+1 && GEN_pt < RCRBin+2 ){//90% eff at same place as Regression
                                RCCBinGENde = RCCBinGENde +1;
                                for (Int_t i=0;i<399;i++){
                                        if(BDTG_class1 >= (i+1)*0.0025){
                                                RCCBinnu[i] = RCCBinnu[i] +1;
                                        }
                                }//end for loop over pT cut
                                
                        }//end if 
                        
                }//end if
                
        }//end loop over Classifier events
        
        //calculate classifier eff under all class1 cut for RCRBin
        Delta=1;
        for (Int_t i=0;i<399;i++){
                if(fabs(RCCBinnu[i]*1.0/RCCBinGENde-0.9) <= Delta){
                        Delta = fabs(RCCBinnu[i]*1.0/RCCBinGENde-0.9);
                        RCCBin = i;//take note of bin number
                }
        }//end loop over Reg pT cuts 
        
        //GEN pt distribution for same 90% turn on
        TH1F *RCCSConlyMCCut = new TH1F("RCCSConlyMCCut", "RCCSConlyMCCut", 50, 0, 10);
        
        //loop over classifier to get the eff plot & rate
        for(Long64_t iEntry = 0; iEntry <numEvents; iEntry++){
              
                myTree->GetEntry(iEntry);
                Float_t GEN_pt = (GEN_pt_br->GetLeaf("GEN_pt"))->GetValue();
                Float_t GEN_charge = (GEN_charge_br->GetLeaf("GEN_charge"))->GetValue();
                Float_t BDTG_class1 = (BDTG_br->GetLeaf("class1"))->GetValue();
                Float_t BDTG_class2 = (BDTG_br->GetLeaf("class2"))->GetValue();//not used here
                Float_t TRK_mode_RPC = (TRK_mode_RPC_br->GetLeaf("TRK_mode_RPC"))->GetValue();
                  
                //CSC-only GEN pT distributions
                if(GEN_charge > -2 && TRK_mode_RPC == 0 && BDTG_class1 >= (RCCBin+1)*0.0025 ){
                        RCCSConlyMCCut->Fill(TMath::Log2(GEN_pt));
                }
                if(GEN_charge < -2 && BDTG_class1 >= (RCCBin+1)*0.0025 ){
                        RCRATE = RCRATE + 1;
                }
                
        }//end loop over classifier events
        
        cout<<"RCC Bin#:"<<RCCBin<<" RCC class1 cut:"<<(RCCBin+1)*0.0025<<" Eff:"<<RCCBinnu[RCCBin]*1.0/RCCBinGENde<<" RC Rate:"<<RCRATE<<endl;
        
        //divide histograms for eff
        TCanvas *C4=new TCanvas("C4","RC",700,500);
        THStack *RCCSConlyEff = new THStack("RCCSConlyEff","Classifier CSC-only 90% Efficiency as Regression 16 GeV");
        C4->cd();
        RCCSConlyMCCut->SetLineColor(2);//red
        RCCSConlyMCCut->SetLineStyle(2);//dash
        RCCSConlyMCCut->SetLineWidth(2);
        gStyle->SetOptStat(0);
        RCCSConlyMCCut->Divide(CSConlyMC);//already has CSConlyMC
        RCCSConlyEff->Add(RegCSConlyMCCut);
        RCCSConlyEff->Add(RCCSConlyMCCut);//alreday exist
        RCCSConlyEff->Draw("nostack");
        RCCSConlyEff->GetXaxis()->SetTitle("log2(GEN pT)");
        RCCSConlyEff->GetYaxis()->SetTitle("efficiency");
        C4->Modified();
        
        TLegend* L4 = new TLegend(0.1,0.7,0.7,0.9);
        TString RegL4="";
        RegL4 = RegL4 + "Regression:trigger pT>=16GeV" + " Rate:"+ Form("%lld", RATE16);//this is RegL2
        TString ClassifierL4="";
        ClassifierL4 = ClassifierL4 + "Classifier:class1>=" + Form("%0.4lf", (RCCBin+1)*0.0025) + " Rate:"+ Form("%lld", RCRATE);
        L4->AddEntry(RegCSConlyMCCut,RegL4);
        L4->AddEntry(RCCSConlyMCCut, ClassifierL4);
        L4->SetFillStyle(0);
        L4->SetBorderSize(0);
        L4->Draw(); 
        C4->Write();
        
        myPlot.Close();
          
}//end macro
