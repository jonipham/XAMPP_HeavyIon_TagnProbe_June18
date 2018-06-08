#! /usr/bin/env python
from XAMPPbase.runSUSYAnalysis import *

# add additional arguments to the argument parser of XAMPPbase.runSUSYAnalysis
def setupStop0LArgParser(parser):
    
    parser = setupSUSYArgParser(parser)

    parser.add_argument('--noRecluster', help='disable jet reclustering during running', action='store_true', default=False)
    parser.add_argument('--noTrackMET', help='disable track MET retrieve and usage during running', action='store_true', default=False)
    parser.add_argument('--doISR', help='run with ISR analysis', action='store_true', default=False)
    parser.add_argument('--NoProgress', help='do not print progress bar', action='store_true', default=False) 
    parser.set_defaults(noJets=False)
    parser.set_defaults(noBtag=False)
    parser.set_defaults(noElectrons=False)
    parser.set_defaults(noMuons=False)
    parser.set_defaults(noTaus=True)
    parser.set_defaults(noPhotons=False)
    parser.set_defaults(analysis='Stop0L_1Y')
    parser.set_defaults(STConfig='XAMPPanalyses/SUSYTools_Stop0LGamma.conf')
    
    return parser

#####
# Stop0L analysis run script
#####
if __name__ == "__main__":
    
    import ROOT
    
    parser = argparse.ArgumentParser(description='This script starts the analysis code. For more help type \"python SUSYFwMPI/util/runStop0LepGamma.py -h\"', prog='runStop0LepGamma')
    
    RunOptions = setupStop0LArgParser(parser).parse_args()    
    
    CutFlowsToRun = []
    if RunOptions.analysis == 'Stop0L_1Y':
        CutFlowsToRun = [RunOptions.analysis]
    else:
        print 'ERROR: Do not run a non-gamma cutflow with runStop0LepGamma.py, exiting...'
        sys.exit(1)    

    start_time = prepareAnalysis(ROOT)
    
    # create a new analysis helper instance
    logging.info("creating AnalysisHelper algorithm")
    AnalysisHelp = ROOT.XAMPP.Stop0LGammaAnalysisHelper("AnalysisHelper")
    if RunOptions.noTrackMET: setASGProperty(AnalysisHelp, 'bool', 'DoTrackMet', False)
    
    # create an analysis configuration with a name which is the suffix of the output trees 
    JobConfig = ROOT.XAMPP.Stop0LGammaAnalysisConfig('Stop0L')
    if RunOptions.analysis != 'Stop0L':
        setASGProperty(JobConfig, 'std::string', 'ActiveCutflowsString', ",".join(CutFlowsToRun))
        
    if RunOptions.doISR:
        setASGProperty(AnalysisHelp, 'bool', 'doISR', True)
        setASGProperty(JobConfig, 'bool', 'doISR', True)
    if RunOptions.noTrackMET: setASGProperty(JobConfig, 'bool', 'DoTrackMet', False)
    
    # the name of the AnalysisConfig has to be parsed to the AnalysisHelper 
    setASGProperty(AnalysisHelp, 'std::string', 'AnalysisConfigName', 'Stop0L')
    
    if RunOptions.noRecluster :
        setASGProperty(AnalysisHelp, 'bool', 'doReclustering', False)
    setASGProperty(AnalysisHelp, 'double', 'CustomReclusterPt', 20000.)
    
    # create and add object selectors
    AnalysisHelp.SUSYElectronSelector = ROOT.XAMPP.SUSYElectronSelector("SUSYElectronSelector")
    setASGProperty(AnalysisHelp.SUSYElectronSelector,'bool','SeparateSF',True)
    setASGProperty(AnalysisHelp.SUSYElectronSelector,'std::string','SFTriggerString',"")
    
    AnalysisHelp.SUSYMuonSelector = ROOT.XAMPP.SUSYMuonSelector("SUSYMuonSelector")
    setASGProperty(AnalysisHelp.SUSYMuonSelector,'bool','SeparateSF',True)
    setASGProperty(AnalysisHelp.SUSYMuonSelector,'std::string','SFTrigger2015String',"")
    setASGProperty(AnalysisHelp.SUSYMuonSelector,'std::string','SFTrigger2016String',"")
        
    AnalysisHelp.SUSYJetSelector = ROOT.XAMPP.SUSYJetSelector("SUSYJetSelector")
    setASGProperty(AnalysisHelp.SUSYJetSelector,'bool','SeparateSF',True)
    setASGProperty(AnalysisHelp.SUSYJetSelector,'float','bJetEtaCut',2.5)
    
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
#     setASGProperty(AnalysisHelp.TriggerTool, 'int', 'OfflineThreshold', -10000000)
    setASGProperty(AnalysisHelp.TriggerTool, 'bool', 'WritePrescaling', True)
    setASGProperty(AnalysisHelp.TriggerTool, 'bool', 'StoreMatchingInfo', False)
    PhotonTriggers = [
            "HLT_g120_loose", # used by Stop0L 2015 (gamma) cutflow comparison    
            "HLT_g140_loose", # used by Stop0L 2016 (gamma) cutflow comparison    
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
    setASGProperty(AnalysisHelp.TriggerTool, 'std::string', '1LeptonTriggersString', ",".join(PhotonTriggers+SingleLepTriggers))
    # setASGProperty(AnalysisHelp.TriggerTool, 'bool', 'DisableSkimming',True)
    setASGProperty(AnalysisHelp.SUSYElectronSelector,'std::string','SFTriggerString',"e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose")
    setASGProperty(AnalysisHelp.SUSYMuonSelector,'std::string','SFTrigger2015String',"HLT_mu20_iloose_L1MU15_OR_HLT_mu50")
    setASGProperty(AnalysisHelp.SUSYMuonSelector,'std::string','SFTrigger2016String',"HLT_mu26_ivarmedium_OR_HLT_mu50")
    
    if not RunOptions.isData:
        AnalysisHelp.SUSYTruthSelector = ROOT.XAMPP.Stop0LTruthSelector("SUSYTruthSelector")
        setASGProperty(AnalysisHelp.SUSYTruthSelector, 'double', 'TruthBJetPtCut', 20000.)
        setASGProperty(AnalysisHelp.SUSYTruthSelector, 'double', 'TruthBJetEtaCut', 2.5)
        setASGProperty(AnalysisHelp.SUSYTruthSelector, 'double', 'TruthJetPtCut', 20000.)
        setASGProperty(AnalysisHelp.SUSYTruthSelector, 'double', 'TruthJetEtaCut', 2.8)

    
    setupSUSYAnalysis(RunOptions, JobConfig, AnalysisHelp, SystematicsTool)
    
    # create a new analysis loop instance
    logging.info("setting up the loop algorithm")
    AnaLoop = ROOT.XAMPP.xAODLoop("xAODLoop")
    if RunOptions.NoProgress: setASGProperty(AnaLoop, 'bool', 'PrintProgress', False)
#     setASGProperty(AnaLoop, 'int', 'OutputLevel', 1)
    AnaLoop.SUSYAnalysisHelper = AnalysisHelp

    runAnalysisLoop(RunOptions, AnaLoop)


    finalizeAnalysis(start_time, AnalysisHelp)
    
