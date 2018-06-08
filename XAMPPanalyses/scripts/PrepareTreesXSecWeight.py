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
            TreeSyst.SetBranchStatus("TrigHLT*",1)
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
                m_newchain.Fill()

            OutFile.cd()
            print 'Writing output file...'
            m_newchain.Write("", ROOT.TObject.kOverwrite)

    InputFile.Close()
    OutFile.Write("", ROOT.TObject.kOverwrite)
    print 'Output file %s successfully saved.'%OutFileName,
    OutFile.Close()
    print 'Done.'
    


    
