#!/usr/bin/python
import sys, os, ROOT

ROOTCOREBIN = os.getenv("ROOTCOREBIN")
ROOTCOREDIR = os.getenv("ROOTCOREDIR")
if not ROOTCOREDIR:
    print 'Setup RootCore'
    sys.exit(1)

if __name__ == "__main__":
    
    import argparse
    parser = argparse.ArgumentParser(description="Script for calculating luminosities", formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('--PRWFile', help='Specify PRWFile(s)', nargs='+', default=["/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/SUSYTools/merged_prw_mc15c_latest.root"])
    parser.add_argument('--ilumicalcFile', help='Specify ilumicalc file(s)', nargs='+', default=['/afs/cern.ch/atlas/project/muon/mcp/PRWFiles/ilumicalc_histograms_data15_13TeV.periodAllYear_DetStatus-v79-repro20-02_DQDefects-00-02-02_PHYS_StandardGRL_All_Good_25ns.root','/afs/cern.ch/atlas/project/muon/mcp/PRWFiles/ilumicalc_histograms_data16_13TeV.periodAllYear_DetStatus-v83-pro20-15_DQDefects-00-02-04_PHYS_StandardGRL_All_Good_25ns.root'])
    options = parser.parse_args()
    
    ROOT.gROOT.Macro("$ROOTCOREDIR/scripts/load_packages.C")
    ROOT.xAOD.Init().ignore()
            
    m_prw_tool = ROOT.CP.PileupReweightingTool('PRW')
    for prw in options.PRWFile:
        m_prw_tool.AddConfigFile(prw)
    for ilumi in options.ilumicalcFile:
        m_prw_tool.AddLumiCalcFile(ilumi)
    m_prw_tool.SetUnrepresentedDataAction(3)
    m_prw_tool.Initialize()
