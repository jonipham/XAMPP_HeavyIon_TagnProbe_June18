#! /usr/bin/env python
from XAMPPbase.runSUSYAnalysis import *

# add additional arguments to the argument parser of XAMPPbase.runSUSYAnalysis
def setupStop0LArgParser(parser):
    
    parser = setupSUSYArgParser(parser)

    parser.add_argument('--noRecluster', help='disable jet reclustering during running', action='store_true', default=False)
    parser.add_argument('--noTrackMET', help='disable track MET retrieve and usage during running', action='store_true', default=False)
    parser.add_argument('--doISR', help='run with ISR analysis', action='store_true', default=False)
    parser.add_argument('--doWDecays', help='classify W decay modes', action='store_true', default=False)
    parser.add_argument('--NoProgress', help='do not print progress bar', action='store_true', default=False)
    parser.set_defaults(noJets=False)
    parser.set_defaults(noBtag=False)
    parser.set_defaults(noElectrons=False)
    parser.set_defaults(noMuons=False)
    parser.set_defaults(noTaus=True)
    parser.set_defaults(noPhotons=False)
    parser.set_defaults(analysis='Stop0L')
    parser.set_defaults(STConfig='XAMPPanalyses/SUSYTools_Stop0L.conf')
    
    return parser

#####
# Stop0L analysis run script
#####
if __name__ == "__main__":
    
    import ROOT
    
    parser = argparse.ArgumentParser(description='This script starts the analysis code. For more help type \"python SUSYFwMPI/util/runStop0Lep.py -h\"', prog='runStop0Lep', formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    
    RunOptions = setupStop0LArgParser(parser).parse_args()    
    
    CutFlowsToRun = []
    if RunOptions.analysis == 'Stop0L_LCR':
        CutFlowsToRun = ['Stop0L_1LCR', 'Stop0L_2LCR']
    elif RunOptions.analysis == 'Stop0L_SR' or RunOptions.analysis == 'Stop0L_SRHard' or RunOptions.analysis == 'Stop0L_1LCR' or RunOptions.analysis == 'Stop0L_2LCR' or RunOptions.analysis == 'Stop0L_ISR_SR':
        CutFlowsToRun = [RunOptions.analysis]
                

    start_time = prepareAnalysis(ROOT)
    
    # create a new analysis helper instance
    logging.info("creating AnalysisHelper algorithm")
    AnalysisHelp = ROOT.XAMPP.Stop0LAnalysisHelper("AnalysisHelper")
    if RunOptions.noTrackMET: setASGProperty(AnalysisHelp, 'bool', 'DoTrackMet', False)
#     setASGProperty(AnalysisHelp, 'int', 'OutputLevelSUSYTools', 1)
    
    # create an analysis configuration with a name which is the suffix of the output trees 
    JobConfig = ROOT.XAMPP.Stop0LAnalysisConfig('Stop0L')
    if RunOptions.analysis != 'Stop0L':
        setASGProperty(JobConfig, 'std::string', 'ActiveCutflowsString', ",".join(CutFlowsToRun))
        
    if RunOptions.doISR:
        setASGProperty(AnalysisHelp, 'bool', 'doISR', True)
        setASGProperty(JobConfig, 'bool', 'doISR', True)
    if RunOptions.noTrackMET: setASGProperty(JobConfig, 'bool', 'DoTrackMet', False)
    if RunOptions.doWDecays:
        setASGProperty(AnalysisHelp, 'bool', 'doWDecays', True)
    # the name of the AnalysisConfig has to be parsed to the AnalysisHelper 
    setASGProperty(AnalysisHelp, 'std::string', 'AnalysisConfigName', 'Stop0L')
    
    if RunOptions.noRecluster :
        setASGProperty(AnalysisHelp, 'bool', 'doReclustering', False)
    setASGProperty(AnalysisHelp, 'double', 'CustomReclusterPt', 20000.)
    
    # create and add object selectors
    AnalysisHelp.SUSYElectronSelector = ROOT.XAMPP.SUSYElectronSelector("SUSYElectronSelector")
    setASGProperty(AnalysisHelp.SUSYElectronSelector,'bool','SeparateSF',True)
    
    AnalysisHelp.SUSYMuonSelector = ROOT.XAMPP.SUSYMuonSelector("SUSYMuonSelector")
    setASGProperty(AnalysisHelp.SUSYMuonSelector,'bool','SeparateSF',True)
        
    AnalysisHelp.SUSYJetSelector = ROOT.XAMPP.SUSYJetSelector("SUSYJetSelector")
    setASGProperty(AnalysisHelp.SUSYJetSelector,'bool','SeparateSF',True)
    setASGProperty(AnalysisHelp.SUSYJetSelector,'float','bJetEtaCut',2.5)
    # can be used if needed for cutflow weight comparisons
    # setASGProperty(AnalysisHelp.SUSYJetSelector,'bool','ApplyBTagSF',False)   
 
    AnalysisHelp.SUSYTauSelector = ROOT.XAMPP.SUSYTauSelector("SUSYTauSelector")

    AnalysisHelp.SUSYPhotonSelector = ROOT.XAMPP.SUSYPhotonSelector("SUSYPhotonSelector")

    AnalysisHelp.SUSYMetSelector = ROOT.XAMPP.Stop0LMetSelector("SUSYMetSelector")
    setASGProperty(AnalysisHelp.SUSYMetSelector, 'bool', 'IncludeTaus', False)
    setASGProperty(AnalysisHelp.SUSYMetSelector, 'bool', 'IncludePhotons', True)
    
    if RunOptions.noTrackMET:
        setASGProperty(AnalysisHelp.SUSYMetSelector, 'bool', 'DoTrackMet', False)
    else:
        setASGProperty(AnalysisHelp.SUSYMetSelector, 'bool', 'DoTrackMet', True)

    # create and add systematics and trigger tools
    SystematicsTool = ROOT.XAMPP.SUSYSystematics("SystematicsTool")
    
    AnalysisHelp.SUSYSystematics = SystematicsTool
    
    AnalysisHelp.TriggerTool = ROOT.XAMPP.SUSYTriggerTool("TriggerTool")
    setASGProperty(AnalysisHelp.TriggerTool, 'int', 'OfflineThreshold', 2000) # set the offline trigger pt threshold to 2GeV for leptons
    setASGProperty(AnalysisHelp.TriggerTool, 'bool', 'WritePrescaling', True)
    setASGProperty(AnalysisHelp.TriggerTool, 'bool', 'StoreMatchingInfo', False)
    METTriggers = [
        "HLT_xe70",
        "HLT_xe70_tc_lcw",
        "HLT_xe70_tc_em",
        "HLT_xe70_mht",
        "HLT_xe70_pueta",
        "HLT_xe70_pufit",
        "HLT_xe80_tc_lcw_L1XE50",
        "HLT_xe80_tc_em_L1XE50",
        "HLT_xe80_mht_L1XE50",
        "HLT_xe80_pueta_L1XE50",
        "HLT_xe80_pufit_L1XE50",
        "HLT_xe80",
        "HLT_xe80_tc_lcw",
        "HLT_xe80_tc_em",
        "HLT_xe80_mht",
        "HLT_xe80_pueta",
        "HLT_xe80_pufit",
        "HLT_xe90_L1XE50",
        "HLT_xe90_tc_lcw_L1XE50",
        "HLT_xe90_tc_em_L1XE50",
        "HLT_xe90_mht_L1XE50",
        "HLT_xe90_pueta_L1XE50",
        "HLT_xe90_pufit_L1XE50",
        "HLT_xe90",
        "HLT_xe90_tc_lcw",
        "HLT_xe90_tc_em",
        "HLT_xe90_mht",
        "HLT_xe90_pueta",
        "HLT_xe90_pufit",
        "HLT_xe100_L1XE50",
        "HLT_xe100_tc_lcw_L1XE50",
        "HLT_xe100_tc_em_L1XE50",
        "HLT_xe100_mht_L1XE50",
        "HLT_xe100_pueta_L1XE50",
        "HLT_xe100_pufit_L1XE50",
        "HLT_xe100",
        "HLT_xe100_tc_lcw",
        "HLT_xe100_tc_em",
        "HLT_xe100_mht",
        "HLT_xe100_pueta",
        "HLT_xe100_pufit",
        "HLT_xe110_L1XE50",
        "HLT_xe110_tc_lcw_L1XE50",
        "HLT_xe110_tc_em_L1XE50",
        "HLT_xe110_mht_L1XE50",
        "HLT_xe110_pueta_L1XE50",
        "HLT_xe110_pufit_L1XE50",
        "HLT_xe110",
        "HLT_xe110_tc_lcw",
        "HLT_xe110_tc_em",
        "HLT_xe110_mht",
        "HLT_xe110_pueta",
        "HLT_xe110_pufit",
        "HLT_xe120_L1XE50",
        "HLT_xe120_tc_lcw_L1XE50",
        "HLT_xe120_tc_em_L1XE50",
        "HLT_xe120_mht_L1XE50",
        "HLT_xe120_pueta_L1XE50",
        "HLT_xe120_pufit_L1XE50",
        "HLT_xe120",
        "HLT_xe120_tc_lcw",
        "HLT_xe120_tc_em",
        "HLT_xe120_mht",
        "HLT_xe120_pueta",
        "HLT_xe120_pufit",
        ]   
    SingleLepTriggers = [
        # taken from https://twiki.cern.ch/twiki/bin/viewauth/Atlas/LowestUnprescaled
        # 2015
        "HLT_e24_lhmedium_L1EM20VH",
        "HLT_e24_lhmedium_L1EM18VH",
        "HLT_e24_lhmedium_iloose_L1EM18VH",
        "HLT_e60_lhmedium",
        "HLT_e120_lhloose ",
        # 2016
        "HLT_e24_lhtight_nod0_ivarloose",
        "HLT_e60_lhmedium_nod0",
        "HLT_e60_medium",
        "HLT_e140_lhloose_nod0",
        "HLT_e300_etcut ",
        "HLT_e26_lhtight_nod0_ivarloose",
        "HLT_e26_lhtight_iloose",
        # 2015
        "HLT_mu20_iloose_L1MU15",
        "HLT_mu40",
        "HLT_mu60_0eta105_msonly",
        # 2016
        "HLT_mu24_iloose",
        "HLT_mu24_iloose_L1MU15",
        "HLT_mu24_ivarloose",
        "HLT_mu24_ivarloose_L1MU15",
        "HLT_mu50 ",
        "HLT_mu24_ivarmedium",
        "HLT_mu24_imedium",
        "HLT_mu26_ivarmedium",
        "HLT_mu26_imedium",
        ]
    # do this for the 2LCR cutflow comparison
    if RunOptions.analysis == 'Stop0L_2LCR':
        SingleLepTriggers = [ 
            "HLT_e24_lhmedium_L1EM18VH",
            "HLT_e120_lhloose",
            "HLT_e60_lhmedium",
            "HLT_e60_lhmedium_nod0",
            "HLT_e26_lhtight_nod0_ivarloose",
            "HLT_e140_lhloose_nod0",
            "HLT_mu50",
            "HLT_mu20_iloose_L1MU15",
            "HLT_mu26_ivarmedium",
            ]
    if 'Stop0L_SR' in CutFlowsToRun or 'Stop0L_SRHard' in CutFlowsToRun or 'Stop0L_ISR_SR' in CutFlowsToRun or 'Stop0L_1LCR' in CutFlowsToRun:
        setASGProperty(AnalysisHelp.TriggerTool, 'std::string', 'METTriggersString', ",".join(METTriggers))
        setASGProperty(AnalysisHelp.SUSYElectronSelector,'std::string','SFTriggerString',"")
        setASGProperty(AnalysisHelp.SUSYMuonSelector,'std::string','SFTrigger2015String',"")
        setASGProperty(AnalysisHelp.SUSYMuonSelector,'std::string','SFTrigger2016String',"")
    if 'Stop0L_1LCR' in CutFlowsToRun or 'Stop0L_2LCR' in CutFlowsToRun:
        setASGProperty(AnalysisHelp.TriggerTool, 'std::string', '1LeptonTriggersString', ",".join(SingleLepTriggers))
        setASGProperty(AnalysisHelp.SUSYElectronSelector,'std::string','SFTriggerString',"e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose")
        setASGProperty(AnalysisHelp.SUSYMuonSelector,'std::string','SFTrigger2015String',"HLT_mu20_iloose_L1MU15_OR_HLT_mu50")
        setASGProperty(AnalysisHelp.SUSYMuonSelector,'std::string','SFTrigger2016String',"HLT_mu26_ivarmedium_OR_HLT_mu50")

    if len(CutFlowsToRun) == 0:
        setASGProperty(AnalysisHelp.TriggerTool, 'std::string', 'METTriggersString', ",".join(METTriggers))
        setASGProperty(AnalysisHelp.TriggerTool, 'std::string', '1LeptonTriggersString', ",".join(SingleLepTriggers))
        setASGProperty(AnalysisHelp.SUSYElectronSelector,'std::string','SFTriggerString',"e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose")
        setASGProperty(AnalysisHelp.SUSYMuonSelector,'std::string','SFTrigger2015String',"HLT_mu20_iloose_L1MU15_OR_HLT_mu50")
        setASGProperty(AnalysisHelp.SUSYMuonSelector,'std::string','SFTrigger2016String',"HLT_mu26_ivarmedium_OR_HLT_mu50")
    
    if not RunOptions.isData:
        AnalysisHelp.SUSYTruthSelector = ROOT.XAMPP.Stop0LTruthSelector("SUSYTruthSelector")
        setASGProperty(AnalysisHelp.SUSYTruthSelector, 'float', 'TruthBJetPtCut', 20000.)
        setASGProperty(AnalysisHelp.SUSYTruthSelector, 'float', 'TruthBJetEtaCut', 2.5)
        setASGProperty(AnalysisHelp.SUSYTruthSelector, 'float', 'TruthBaselineJetPtCut', 20000.)
        setASGProperty(AnalysisHelp.SUSYTruthSelector, 'float', 'TruthBaselineJetEtaCut', 2.8)
        setASGProperty(AnalysisHelp.SUSYTruthSelector, 'float', 'TruthSignalJetPtCut', 20000.)
        setASGProperty(AnalysisHelp.SUSYTruthSelector, 'float', 'TruthSignalJetEtaCut', 2.5)
        setASGProperty(AnalysisHelp.SUSYTruthSelector, 'float', 'TruthBaselineElePtCut', 7000.)                       
        setASGProperty(AnalysisHelp.SUSYTruthSelector, 'float', 'TruthBaselineEleEtaCut', 2.47)                       
        setASGProperty(AnalysisHelp.SUSYTruthSelector, 'float', 'TruthSignalElePtCut', 20000.)    
        setASGProperty(AnalysisHelp.SUSYTruthSelector, 'float', 'TruthBaselineMuPtCut', 6000.)                                                        
        setASGProperty(AnalysisHelp.SUSYTruthSelector, 'float', 'TruthBaselineMuEtaCut', 2.7)                                                        
        setASGProperty(AnalysisHelp.SUSYTruthSelector, 'float', 'TruthSignalMuPtCut', 20000.)
    
    setupSUSYAnalysis(RunOptions, JobConfig, AnalysisHelp, SystematicsTool)
    
    # create a new analysis loop instance
    logging.info("setting up the loop algorithm")
    AnaLoop = ROOT.XAMPP.xAODLoop("xAODLoop")
    if RunOptions.NoProgress: setASGProperty(AnaLoop, 'bool', 'PrintProgress', False)
    AnaLoop.SUSYAnalysisHelper = AnalysisHelp

    runAnalysisLoop(RunOptions, AnaLoop)


    finalizeAnalysis(start_time, AnalysisHelp)
    
