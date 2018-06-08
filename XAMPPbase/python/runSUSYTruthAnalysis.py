#! /usr/bin/env python
from XAMPPbase.runSUSYAnalysis import *

def setupAnalysis(RunOptions, JobConfig, AnalysisHelper, SystematicsTool):

    logging.info("Configuring the Analysis job")

    if RunOptions.noTruth or RunOptions.isData:
        setASGProperty(AnalysisHelper, 'bool', 'doTruth', False)
     
    OutFileName = RunOptions.outFile
    if OutFileName.split('.')[-1] == 'root':
        setASGProperty(AnalysisHelper, 'std::string', 'OutFileName', OutFileName)
    else : 
        setASGProperty(AnalysisHelper, 'std::string', 'OutFileName', OutFileName + '.root')

    if RunOptions.isData:
        setASGProperty(SystematicsTool, 'bool', 'doSyst', False)
        setASGProperty(AnalysisHelper, 'bool', 'isData', True)
        setASGProperty(JobConfig, 'bool', 'isData', True)
        
    
    if RunOptions.noSyst:
        setASGProperty(SystematicsTool, 'bool', 'doSyst', False)
        
    if RunOptions.noHistos:
        setASGProperty(AnalysisHelper, 'bool', 'doHistos', False)
    if RunOptions.noTrees:
        setASGProperty(AnalysisHelper, 'bool', 'doTrees', False)
    if RunOptions.noCutFlow:
        setASGProperty(AnalysisHelper, 'bool', 'RunCutFlow', False)
    if RunOptions.noxAODs:
        setASGProperty(AnalysisHelper, 'bool', 'doxAODs', False)
    
    setASGProperty(AnalysisHelper, 'bool', 'doxAODs', False)

#####
#  Stop0L analysis run script
#####
if __name__ == "__main__":
    
    import ROOT
    
    parser = argparse.ArgumentParser(description='This script starts the analysis code. For more help type \"python XAMPPbase/python/runSUSYAnalysis.py -h\"', prog='runSUSYAnalysis', formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    
    RunOptions = setupSUSYArgParser(parser).parse_args()

    start_time = prepareAnalysis(ROOT)
    
    if RunOptions.isData:
        print 'ERROR: You are trying to run a truth analysis on data, exiting...'
        sys.exit(0)
    
    #  create a new analysis loop instance
    logging.info("setting up the loop algorithm")
    AnaLoop = ROOT.XAMPP.xAODLoop("xAODLoop")
    #  create a new analysis helper instance
    logging.info("creating algorithms with AnalysisHelp")
    AnalysisHelp = ROOT.XAMPP.SUSYTruthAnalysisHelper("AnalysisHelper")
    setASGProperty(AnalysisHelp, 'bool', 'UseFileMetaData', False)
    #  create a new configuration instance and parse its name to the AnalysisHelper
    JobConfig = ROOT.XAMPP.TruthAnalysisConfig("SUSYAnalysisConfig")

    #  create and add systematics and trigger tools
    SystematicsTool = ROOT.XAMPP.SUSYSystematics("SystematicsTool")
    setASGProperty(SystematicsTool, 'bool', 'doSyst', False)
    setASGProperty(SystematicsTool, 'bool', 'doWeights', False)
    
    AnalysisHelp.SUSYTruthSelector = ROOT.XAMPP.SUSYTruthSelector("SUSYTruthSelector")
    setupAnalysis(RunOptions, JobConfig, AnalysisHelp, SystematicsTool)

    runAnalysisLoop(RunOptions, AnaLoop)

    finalizeAnalysis(start_time, AnalysisHelp)
