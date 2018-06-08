#! /usr/bin/env python
from XAMPPbase.runSUSYAnalysis import *
from XAMPPbase.runSUSYTruthAnalysis import *

#####
#  Stop0L analysis run script
#####
if __name__ == "__main__":
    
    import ROOT
    
    parser = argparse.ArgumentParser(description='This script starts the analysis code. For more help type \"python XAMPPbase/python/runSUSYAnalysis.py -h\"', prog='runSUSYAnalysis')
    parser.set_defaults(analysis='Stop0LTruth')
    parser.add_argument('--NoProgress', help='do not print progress bar', action='store_true', default=False)


    RunOptions = setupSUSYArgParser(parser).parse_args()

    CutFlowsToRun = []
    if RunOptions.analysis == 'Stop0LTruth_SR':
        CutFlowsToRun = ['Stop0LTruth_SR']
    elif RunOptions.analysis == 'Stop0LTruth_DMSR':
        CutFlowsToRun = ['Stop0LTruth_DMSR']

    start_time = prepareAnalysis(ROOT)
    
    if RunOptions.isData:
        print 'ERROR: You are trying to run a truth analysis on data, exiting...'
        sys.exit(0)
    
    #  create a new analysis loop instance
    logging.info("setting up the loop algorithm")
    AnaLoop = ROOT.XAMPP.xAODLoop("xAODLoop")
    setASGProperty(AnaLoop, 'bool', 'InitWithFirstEvent', True)
    #  create a new analysis helper instance
    logging.info("creating algorithms with AnalysisHelp")
    AnalysisHelp = ROOT.XAMPP.Stop0LTruthAnalysisHelper("AnalysisHelper")
    setASGProperty(AnalysisHelp, 'double', 'CustomReclusterPt', 20000.)
    setASGProperty(AnalysisHelp, 'bool', 'fillLHEWeights', True)
    #  create a new configuration instance and parse its name to the AnalysisHelper
    JobConfig = ROOT.XAMPP.Stop0LTruthAnalysisConfig('Stop0L')
    setASGProperty(JobConfig, 'std::string', 'ActiveCutflowsString', ",".join(CutFlowsToRun))
    setASGProperty(AnalysisHelp, 'std::string', 'AnalysisConfigName', 'Stop0L')

    #  create and add systematics and trigger tools
    SystematicsTool = ROOT.XAMPP.SUSYSystematics("SystematicsTool")
    setASGProperty(SystematicsTool, 'bool', 'doSyst', False)
    setASGProperty(SystematicsTool, 'bool', 'doWeights', False)
    
    AnalysisHelp.SUSYTruthSelector = ROOT.XAMPP.Stop0LTruthSelector("SUSYTruthSelector")
    setASGProperty(AnalysisHelp.SUSYTruthSelector, 'bool', 'doJets', True)
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
    
    setASGProperty(AnalysisHelp.SUSYTruthSelector, 'bool', 'PreselectionHardProcess', True) # done in SimpleAnalysis to select 'iso'-objects
    
    # either use TruthParticles container present in SUSY1 or use single object containers
    setASGProperty(AnalysisHelp.SUSYTruthSelector, 'bool', 'doTruthParticles', True)
#     setASGProperty(AnalysisHelp.SUSYTruthSelector, 'string', 'MuonContainer', "TruthMuons")
#     setASGProperty(AnalysisHelp.SUSYTruthSelector, 'string', 'ElectronContainer', "TruthElectrons")
    
    setupAnalysis(RunOptions, JobConfig, AnalysisHelp, SystematicsTool)
    if RunOptions.NoProgress: setASGProperty(AnaLoop, 'bool', 'PrintProgress', False)
    runAnalysisLoop(RunOptions, AnaLoop)
    

    finalizeAnalysis(start_time, AnalysisHelp)
