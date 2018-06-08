#! /usr/bin/env python
from XAMPPbase.printCutFlow import *

import os
from array import array 
from copy import deepcopy
import mimetypes

USER = os.getenv("USER")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(prog='PrepareTreesXSecWeight',description='This script creates a slimmed TTree with an EventWeight for MC which already includes all weights including cross section from the output of XAMPP')
    parser.add_argument('-i', '--inputFile', help='specify name of input file (ROOT file or text file with path(s) to ROOT file(s)) (incl. path)', default='', required=True)
    parser.add_argument('-o', '--outputFile', help='specify name of output file (incl. path)', default='SlimmedTree.root')
    parser.add_argument('-l', '--lumi', help='specify integrated luminosity of MC samples (in pb^-1)', type=float, default=1.)

    RunOptions = parser.parse_args()

    # Set batch mode and atlas style
    ROOT.gROOT.SetBatch(True)
    
    if RunOptions.outputFile == 'SlimmedTree.root':
        OutFileName = os.path.dirname(os.path.realpath(RunOptions.inputFile))+'/SlimmedTree.root'
    else:
        OutFileName = RunOptions.outputFile
        
    OutFileName = RunOptions.inputFile.replace(".root.txt","")+"_ttbarCR_skimmed.root"
        
    if not os.path.exists(RunOptions.inputFile):
        print 'File %s does not exist, exiting...'%(RunOptions.inputFile)
        sys.exit(1)
        
    # Do we have one root file, or a text file with a list?
    filelist = []
    if mimetypes.guess_type(RunOptions.inputFile)[0]!=None:
        print 'Having filelist as an input, reading %s ...'%(RunOptions.inputFile) 
        dsfile = open(RunOptions.inputFile)
        MetaDataTree = ROOT.TChain('MetaData')
        InputFile = None
        for line in dsfile:
            # Ignore comment lines and empty lines
            if line.startswith('#'): continue
            realline = line.strip()
            if not realline: continue # Ignore whitespace
            print 'Adding file %s ...'%realline
            MetaDataTree.Add(realline)
            filelist.append(realline)
            if not InputFile:
                InputFile = ROOT.TFile.Open(realline) # need one file of the list for getting the file structure
        if not InputFile:
            print 'Filelist %s does not contain any files, exiting...'%(RunOptions.inputFile) 
            sys.exit(1)
    else:
        InputFile = ROOT.TFile.Open(RunOptions.inputFile)
        MetaDataTree = InputFile.Get("MetaData")
    
    MetaData = getMetaData(MetaDataTree)
    
    isData = False
    if max(MetaData.iterkeys()) == -1:
        print 'Running on data file...'
        isData = True
    else:
        print 'Scaling MC to an integrated luminosity of %.1f pb^-1'%RunOptions.lumi
    
    OutFile = ROOT.TFile.Open(OutFileName,'recreate')
    
                
    ListOfKeys = InputFile.GetListOfKeys()
    myiter = ListOfKeys.MakeIterator()
    mykey = ROOT.TKey()

    print 'Content of input file %s:'%(InputFile.GetName())
    for item in ListOfKeys:
        print '- '+item.GetName()
        if 'Histo' in item.GetName():
            continue
        mykey = (item)
        
        thedir = ROOT.TTree()
        thedir = mykey.ReadObj()
        TreeName = thedir.GetName()
        
        if 'Nominal' not in TreeName:
            continue
        else:
            TreeSyst = ROOT.TChain(TreeName)
            if len(filelist)>0:
                for line in filelist:
                    TreeSyst.Add(line)
            else:
                TreeSyst.Add(RunOptions.inputFile)

            TreeSyst.SetBranchStatus("*",0)
            TreeSyst.SetBranchStatus("MetTST*",1)
            TreeSyst.SetBranchStatus("MetTrack*",1)
            TreeSyst.SetBranchStatus("mcChannelNumber",1)
            TreeSyst.SetBranchStatus("Elec_*",1)
            TreeSyst.SetBranchStatus("Muon_*",1)
            TreeSyst.SetBranchStatus("Jet_*",1)
            TreeSyst.SetBranchStatus("RecoCandidates_*",1)
            TreeSyst.SetBranchStatus("FatJet08_*",1)
            TreeSyst.SetBranchStatus("FatJet12_*",1)
            TreeSyst.SetBranchStatus("N*Jets",1)
            TreeSyst.SetBranchStatus("N_SignalLeptons",1)
            TreeSyst.SetBranchStatus("N_Jets",1)
            TreeSyst.SetBranchStatus("N_BJets",1)
            TreeSyst.SetBranchStatus("Mll",1)
            TreeSyst.SetBranchStatus("MinMt",1)
            TreeSyst.SetBranchStatus("MtLepMet",1)
            TreeSyst.SetBranchStatus("HT",1)
            TreeSyst.SetBranchStatus("Meff",1)
            TreeSyst.SetBranchStatus("DeltaPhi*",1)
            TreeSyst.SetBranchStatus("DPhiMetTrackMet",1)
            TreeSyst.SetBranchStatus("MtBMin",1)
            TreeSyst.SetBranchStatus("MtNonBMin",1)
            TreeSyst.SetBranchStatus("TrigHLT_xe70_tc_lcw",1)
            TreeSyst.SetBranchStatus("eventNumber",1)
            TreeSyst.SetBranchStatus("HasTauCand",1)
            TreeSyst.SetBranchStatus("AsymmetryMTop",1)
            TreeSyst.SetBranchStatus("HtSig",1)
            if not isData:
                TreeSyst.SetBranchStatus("*Weight",1)

            m_newchain = TreeSyst.CloneTree(0)
            Nentries = TreeSyst.GetEntries()
            
            if Nentries == 0:
                continue

            eventWeight = array('d', [-1.])
            m_newchain.Branch("EventWeight", eventWeight, "EventWeight/D")
            eventWeight[0] = 1.
            
            Jet1Pt = array('f', [-1.])
            m_newchain.Branch("Jet1Pt", Jet1Pt, "Jet1Pt/F")
            Jet1Pt[0] = 1. 
            Jet2Pt = array('f', [-1.])
            m_newchain.Branch("Jet2Pt", Jet2Pt, "Jet2Pt/F")
            Jet2Pt[0] = 1. 
            Jet3Pt = array('f', [-1.])
            m_newchain.Branch("Jet3Pt", Jet3Pt, "Jet3Pt/F")
            Jet3Pt[0] = 1. 
            Jet4Pt = array('f', [-1.])
            m_newchain.Branch("Jet4Pt", Jet4Pt, "Jet4Pt/F")
            Jet4Pt[0] = 1.
            Jet5Pt = array('f', [-1.])
            m_newchain.Branch("Jet5Pt", Jet5Pt, "Jet5Pt/F")
            Jet5Pt[0] = 1. 
            Jet6Pt = array('f', [-1.])
            m_newchain.Branch("Jet6Pt", Jet6Pt, "Jet6Pt/F")
            Jet6Pt[0] = 1. 
            
            TopCand1Pt = array('f', [-1.])
            m_newchain.Branch("TopCand1Pt", TopCand1Pt, "TopCand1Pt/F")
            TopCand1Pt[0] = 1. 
            TopCand2Pt = array('f', [-1.])
            m_newchain.Branch("TopCand2Pt", TopCand2Pt, "TopCand2Pt/F")
            TopCand2Pt[0] = 1. 
            FatJet12_1Pt = array('f', [-1.])
            m_newchain.Branch("FatJet12_1Pt", FatJet12_1Pt, "FatJet12_1Pt/F")
            FatJet12_1Pt[0] = 1. 
            FatJet12_2Pt = array('f', [-1.])
            m_newchain.Branch("FatJet12_2Pt", FatJet12_2Pt, "FatJet12_2Pt/F")
            FatJet12_2Pt[0] = 1. 
            FatJet12_1M = array('f', [-1.])
            m_newchain.Branch("FatJet12_1M", FatJet12_1M, "FatJet12_1M/F")
            FatJet12_1M[0] = 1. 
            FatJet12_2M = array('f', [-1.])
            m_newchain.Branch("FatJet12_2M", FatJet12_2M, "FatJet12_2M/F")
            FatJet12_2M[0] = 1. 
            
            BJet1Pt = array('f', [-1.])
            m_newchain.Branch("BJet1Pt", BJet1Pt, "BJet1Pt/F")
            BJet1Pt[0] = 1. 
            BJet2Pt = array('f', [-1.])
            m_newchain.Branch("BJet2Pt", BJet2Pt, "BJet2Pt/F")
            BJet2Pt[0] = 1. 
            
            mb1l_weight = array('f', [-1.])
            m_newchain.Branch("mb1l_weight", mb1l_weight, "mb1l_weight/F")
            mb1l_weight[0] = 1.
            mb2l_weight = array('f', [-1.])
            m_newchain.Branch("mb2l_weight", mb2l_weight, "mb2l_weight/F")
            mb2l_weight[0] = 1.
            mblmin_weight = array('f', [-1.])
            m_newchain.Branch("mblmin_weight", mblmin_weight, "mblmin_weight/F")
            mblmin_weight[0] = 1.
            mbb_weight = array('f', [-1.])
            m_newchain.Branch("mbb_weight", mbb_weight, "mbb_weight/F")
            mbb_weight[0] = 1.
            mbb_pt = array('f', [-1.])
            m_newchain.Branch("mbb_pt", mbb_pt, "mbb_pt/F")
            mbb_pt[0] = 1.
            
            #JetLeps
            JetLep1Pt = array('f', [-1.])
            m_newchain.Branch("JetLep1Pt", JetLep1Pt, "JetLep1Pt/F")
            JetLep1Pt[0] = -1.
            JetLep2Pt = array('f', [-1.])
            m_newchain.Branch("JetLep2Pt", JetLep2Pt, "JetLep2Pt/F")
            JetLep2Pt[0] = -1.
            JetLep3Pt = array('f', [-1.])
            m_newchain.Branch("JetLep3Pt", JetLep3Pt, "JetLep3Pt/F")
            JetLep3Pt[0] = -1.
            JetLep4Pt = array('f', [-1.])
            m_newchain.Branch("JetLep4Pt", JetLep4Pt, "JetLep4Pt/F")
            JetLep4Pt[0] = -1.
            JetLep5Pt = array('f', [-1.])
            m_newchain.Branch("JetLep5Pt", JetLep5Pt, "JetLep5Pt/F")
            JetLep5Pt[0] = -1.
            JetLep6Pt = array('f', [-1.])
            m_newchain.Branch("JetLep6Pt", JetLep6Pt, "JetLep6Pt/F")
            JetLep6Pt[0] = -1.
                
            print 'Start looping over %.0f events...'%Nentries
            for jentries in range(0,Nentries):
                TreeSyst.GetEntry(jentries)
                if not isData:
                    MetaDataInformation = MetaData[TreeSyst.mcChannelNumber]
                    eventWeight[0] = RunOptions.lumi*MetaDataInformation[2]*TreeSyst.BTagWeight*TreeSyst.EleWeight*TreeSyst.MuoWeight*TreeSyst.GenWeight/MetaDataInformation[1]
                    # MET sample overlap cuts:
                    if TreeSyst.mcChannelNumber == 410000 or TreeSyst.mcChannelNumber == 410013 or TreeSyst.mcChannelNumber == 410014:
                        if TreeSyst.GenFiltMET >= 200000.:
                            continue
                        
                # apply preselection:
                if not TreeSyst.N_SignalLeptons == 1:
                    continue
                if not TreeSyst.N_Jets > 2:
                    continue
                if not TreeSyst.N_BJets > 1:
                    continue
                if ord(TreeSyst.TrigHLT_xe70_tc_lcw) != 1:
                    continue
                if not TreeSyst.MetTST_met > 200000.:
                    continue
                if not TreeSyst.MtLepMet < 120000.:
                    continue
                
                # STCR
#                 if not math.fabs(TreeSyst.DeltaPhiMin3) > 0.3141593:
#                     continue
                # ttbarCR
                if not math.fabs(TreeSyst.DeltaPhiMin3) > 0.6283185:
                    continue
                
                    
                if len(TreeSyst.Jet_pt)>0:
                    Jet1Pt[0] = TreeSyst.Jet_pt[0]
                if len(TreeSyst.Jet_pt)>1:
                    Jet2Pt[0] = TreeSyst.Jet_pt[1]
                if len(TreeSyst.Jet_pt)>2:
                    Jet3Pt[0] = TreeSyst.Jet_pt[2]
                if len(TreeSyst.Jet_pt)>3:
                    Jet4Pt[0] = TreeSyst.Jet_pt[3]
                if len(TreeSyst.Jet_pt)>4:
                    Jet5Pt[0] = TreeSyst.Jet_pt[4]
                if len(TreeSyst.Jet_pt)>5:
                    Jet6Pt[0] = TreeSyst.Jet_pt[5]
                    
                recoCand_idx = 0
                for i in TreeSyst.RecoCandidates_pt:
                    if TreeSyst.RecoCandidates_RecoType[recoCand_idx] == 1 and TreeSyst.RecoCandidates_pdgId[recoCand_idx] == 6:
                        TopCand1Pt[0] = i
                    if TreeSyst.RecoCandidates_RecoType[recoCand_idx] == 2 and TreeSyst.RecoCandidates_pdgId[recoCand_idx] == 6:
                        TopCand2Pt[0] = i
                    recoCand_idx += 1
                    
                if len(TreeSyst.FatJet12_pt)>0:
                    FatJet12_1Pt[0] = TreeSyst.FatJet12_pt[0]
                    FatJet12_1M[0] = TreeSyst.FatJet12_m[0]
                if len(TreeSyst.FatJet12_pt)>1:
                    FatJet12_2Pt[0] = TreeSyst.FatJet12_pt[1]
                    FatJet12_2M[0] = TreeSyst.FatJet12_m[1]
                  
                # sort bjets by weight
                jet_idx = 0
                bjetmax1_idx = -1
                bjetmax2_idx = -1
                bjetmaxpt1_idx = -1
                bjetmaxpt2_idx = -1
                bweights = []
                JetPts = []
                for pt in TreeSyst.Jet_pt:
                    w = TreeSyst.Jet_MV2c20[jet_idx]
                    JetPts.append((pt,jet_idx))
                    if ord(TreeSyst.Jet_bjet[jet_idx]) == 1:
                        bweights.append((w,jet_idx))
                        if bjetmaxpt1_idx == -1:
                            bjetmaxpt1_idx = jet_idx
                        elif bjetmaxpt2_idx == -1:
                            bjetmaxpt2_idx = jet_idx
                    jet_idx += 1
                    
                for w,idx in sorted(bweights,reverse=True):
                    if bjetmax1_idx == -1:
                        bjetmax1_idx = idx
                    elif bjetmax2_idx == -1:
                        bjetmax2_idx = idx
                    else:
                        continue
                BJet1Pt[0] = TreeSyst.Jet_pt[bjetmax1_idx]
                BJet2Pt[0] = TreeSyst.Jet_pt[bjetmax2_idx]
                
                # calculate invariant masses
                b1 = ROOT.TLorentzVector(0.,0.,0.,0.)
                b2 = ROOT.TLorentzVector(0.,0.,0.,0.)
                b1.SetPtEtaPhiE(TreeSyst.Jet_pt[bjetmax1_idx],TreeSyst.Jet_eta[bjetmax1_idx],TreeSyst.Jet_phi[bjetmax1_idx],TreeSyst.Jet_e[bjetmax1_idx])
                b2.SetPtEtaPhiE(TreeSyst.Jet_pt[bjetmax2_idx],TreeSyst.Jet_eta[bjetmax2_idx],TreeSyst.Jet_phi[bjetmax2_idx],TreeSyst.Jet_e[bjetmax2_idx])
                
                lep = ROOT.TLorentzVector(0.,0.,0.,0.)
                isElec = False
                if len(TreeSyst.Elec_pt)>0:
                    lep.SetPtEtaPhiE(TreeSyst.Elec_pt[0],TreeSyst.Elec_eta[0],TreeSyst.Elec_phi[0],TreeSyst.Elec_e[0])
                    isElec = True
                else:
                    lep.SetPtEtaPhiE(TreeSyst.Muon_pt[0],TreeSyst.Muon_eta[0],TreeSyst.Muon_phi[0],TreeSyst.Muon_e[0])
                mb1l_weight[0] = (b1+lep).M()
                mb2l_weight[0] = (b2+lep).M()
                mblmin_weight[0] = min([mb1l_weight[0],mb2l_weight[0]])
                
                mbb_weight[0] = (b1+b2).M()
                b1.SetPtEtaPhiE(TreeSyst.Jet_pt[bjetmaxpt1_idx],TreeSyst.Jet_eta[bjetmaxpt1_idx],TreeSyst.Jet_phi[bjetmaxpt1_idx],TreeSyst.Jet_e[bjetmaxpt1_idx])
                b2.SetPtEtaPhiE(TreeSyst.Jet_pt[bjetmaxpt2_idx],TreeSyst.Jet_eta[bjetmaxpt2_idx],TreeSyst.Jet_phi[bjetmaxpt2_idx],TreeSyst.Jet_e[bjetmaxpt2_idx])
                mbb_pt[0] = (b1+b2).M()
                
                # build jetleps out
                JetLepPts = []
                haveLep = False
                for pt,idx in sorted(JetPts,reverse=True):
                    if isElec:
                        if not haveLep and TreeSyst.Elec_pt[0] > pt:
                            JetLepPts.append(TreeSyst.Elec_pt[0])
                            haveLep = True
                            JetLepPts.append(pt)
                        else:
                            JetLepPts.append(pt)                            
                    else:
                        if not haveLep and TreeSyst.Muon_pt[0] > pt:
                            JetLepPts.append(TreeSyst.Muon_pt[0])
                            haveLep = True
                            JetLepPts.append(pt)
                        else:
                            JetLepPts.append(pt)
                if not haveLep:
                    if isElec:
                        JetLepPts.append(TreeSyst.Elec_pt[0])
                    else:
                        JetLepPts.append(TreeSyst.Muon_pt[0])

                if len(JetLepPts)>0:
                    JetLep1Pt[0] = JetLepPts[0]
                if len(JetLepPts)>1:
                    JetLep2Pt[0] = JetLepPts[1]
                if len(JetLepPts)>2:
                    JetLep3Pt[0] = JetLepPts[2]
                if len(JetLepPts)>3:
                    JetLep4Pt[0] = JetLepPts[3]
                if len(JetLepPts)>4:
                    JetLep5Pt[0] = JetLepPts[4]
                if len(JetLepPts)>5:
                    JetLep6Pt[0] = JetLepPts[5]
                
                m_newchain.Fill()

            OutFile.cd()
            print 'Writing output file...'
            m_newchain.Write("", ROOT.TObject.kOverwrite)

    InputFile.Close()
    OutFile.Write("", ROOT.TObject.kOverwrite)
    print 'Output file %s successfully saved.'%OutFileName,
    OutFile.Close()
    print 'Done.'
    


    
