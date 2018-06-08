#! /usr/bin/env python
from XAMPPbase.runSUSYAnalysis import *

# add additional arguments to the argument parser of XAMPPbase.runSUSYAnalysis
def setupDMbbArgParser(parser):
    
    parser = setupSUSYArgParser(parser)

    parser.add_argument('--noRecluster', help='disable jet reclustering during running', action='store_true', default=False)
    parser.add_argument('--noTrackMET', help='disable track MET retrieve and usage during running', action='store_true', default=False)
    parser.set_defaults(noTaus=True)
    parser.set_defaults(noPhotons=True)  
    parser.set_defaults(analysis='DMbb')
    parser.set_defaults(STConfig='XAMPPanalyses/SUSYTools_DMbb.conf') 
    return parser


#####
# DMbb analysis run script
#####
if __name__ == "__main__":
    
    import ROOT
    
    parser = argparse.ArgumentParser(description='This script starts the analysis code. For more help type \"python SUSYFwMPI/util/runDMbbep.py -h\"', prog='runDMbbep')
    
    RunOptions = setupDMbbArgParser(parser).parse_args()
    

    start_time = prepareAnalysis(ROOT)

    # create a new configuration instance
    JobConfig = ROOT.XAMPP.DMbbAnalysisConfig(RunOptions.analysis)
    
    # create a new analysis helper instance
    logging.info("creating AnalysisHelper algorithm")
    AnalysisHelp = ROOT.XAMPP.DMbbAnalysisHelper("AnalysisHelper")
    if RunOptions.noTrackMET: setASGProperty(AnalysisHelp, 'bool', 'DoTrackMet', False) 
   
    # the name of the AnalysisConfig has to be parsed to the AnalysisHelper 
    setASGProperty(AnalysisHelp, 'std::string', 'AnalysisConfigName', RunOptions.analysis)
        
    #if RunOptions.noRecluster :
        #setASGProperty(AnalysisHelp, 'bool', 'doReclustering', False)
    #setASGProperty(AnalysisHelp, 'double', 'CustomReclusterPt', 20000.)
    
    # create and add object selectors
    AnalysisHelp.SUSYElectronSelector = ROOT.XAMPP.SUSYElectronSelector("SUSYElectronSelector")
    setASGProperty(AnalysisHelp.SUSYElectronSelector,'bool','SeparateSF',True)
    
    AnalysisHelp.SUSYMuonSelector = ROOT.XAMPP.SUSYMuonSelector("SUSYMuonSelector")
    setASGProperty(AnalysisHelp.SUSYMuonSelector,'bool','SeparateSF',True)
        
    AnalysisHelp.SUSYJetSelector = ROOT.XAMPP.SUSYJetSelector("SUSYJetSelector")
    setASGProperty(AnalysisHelp.SUSYJetSelector,'bool','SeparateSF',True)
    
    AnalysisHelp.SUSYTauSelector = ROOT.XAMPP.SUSYTauSelector("SUSYTauSelector")
    setASGProperty(AnalysisHelp.SUSYTauSelector, 'bool', 'doNoTaus', True)

    AnalysisHelp.SUSYPhotonSelector = ROOT.XAMPP.SUSYPhotonSelector("SUSYPhotonSelector")
    setASGProperty(AnalysisHelp.SUSYPhotonSelector, 'bool', 'doNoPhotons', True)

    AnalysisHelp.SUSYMetSelector = ROOT.XAMPP.SUSYMetSelector("SUSYMetSelector")
    setASGProperty(AnalysisHelp.SUSYMetSelector, 'bool', 'IncludeTaus', False)
    setASGProperty(AnalysisHelp.SUSYMetSelector, 'bool', 'IncludePhotons', False)
    if RunOptions.noTrackMET:
        setASGProperty(AnalysisHelp.SUSYMetSelector, 'bool', 'DoTrackMet', False)
    else:
        setASGProperty(AnalysisHelp.SUSYMetSelector, 'bool', 'DoTrackMet', True)

    # create and add systematics and trigger tools
    SystematicsTool = ROOT.XAMPP.SUSYSystematics("SystematicsTool")
    setASGProperty(SystematicsTool, 'bool', 'doNoTaus', True)
    setASGProperty(SystematicsTool, 'bool', 'doNoPhotons', True)
    
    AnalysisHelp.SUSYSystematics = SystematicsTool
    
    AnalysisHelp.TriggerTool = ROOT.XAMPP.SUSYTriggerTool("TriggerTool")
    setASGProperty(AnalysisHelp.TriggerTool, 'int', 'OfflineThreshold', 2000) # set the offline trigger pt threshold to 2GeV for leptons
    METTriggers = [
        "HLT_xe70_tc_lcw",
        "HLT_xe70",
        "HLT_xe70_pueta",
        "HLT_xe70_pufit",
        "HLT_xe70_tc_em",
        "HLT_xe70_mht",
        "HLT_xe80",
        "HLT_xe80_L1XE50",
        "HLT_xe80_pueta",
        "HLT_xe80_pufit",
        "HLT_xe80_tc_lcw",
        "HLT_xe80_tc_em",
        "HLT_xe80_mht",
        "HLT_xe80_tc_lcw_L1XE50",
        "HLT_xe80_tc_lcw_wEFMu_L1XE50",
        
        "HLT_xe90",
        "HLT_xe90_L1XE50",
        "HLT_xe90_pueta",
        "HLT_xe90_pufit",
        "HLT_xe90_tc_lcw",
        "HLT_xe90_tc_em",
        "HLT_xe90_mht",
        "HLT_xe90_tc_lcw_L1XE50",
        "HLT_xe90_tc_lcw_wEFMu_L1XE50",
        "HLT_xe100",
        "HLT_xe100_L1XE50",
        "HLT_xe100_pueta",
        "HLT_xe100_pufit",
        "HLT_xe100_tc_lcw",
        "HLT_xe100_tc_em",
        "HLT_xe100_mht",
        "HLT_xe100_tc_lcw_L1XE50",
        "HLT_xe100_tc_lcw_wEFMu_L1XE50",
        "HLT_j80_xe80",
        "HLT_xe120_pueta",
        "HLT_xe120_pufit",
   
        "HLT_xe110_pueta_L1XE50",
        ]
    
    setASGProperty(AnalysisHelp.TriggerTool, 'bool', 'DisableSkimming', True)

    setASGProperty(AnalysisHelp.TriggerTool, 'std::string', 'METTriggersString', ",".join(METTriggers))
    
    if not RunOptions.isData:
        AnalysisHelp.SUSYTruthSelector = ROOT.XAMPP.SUSYTruthSelector("SUSYTruthSelector")
        setASGProperty(AnalysisHelp.SUSYTruthSelector, 'double', 'TruthBJetPtCut', 20000.)
        setASGProperty(AnalysisHelp.SUSYTruthSelector, 'double', 'TruthBJetEtaCut', 2.5)
        setASGProperty(AnalysisHelp.SUSYTruthSelector, 'double', 'TruthJetPtCut', 20000.)
        setASGProperty(AnalysisHelp.SUSYTruthSelector, 'double', 'TruthJetEtaCut', 2.8)

    
    setupSUSYAnalysis(RunOptions, JobConfig, AnalysisHelp, SystematicsTool)

    # create a new analysis loop instance
    logging.info("setting up the loop algorithm")
    AnaLoop = ROOT.XAMPP.xAODLoop("xAODLoop")
    AnaLoop.SUSYAnalysisHelper = AnalysisHelp

    runAnalysisLoop(RunOptions, AnaLoop)


    finalizeAnalysis(start_time, AnalysisHelp)
    
