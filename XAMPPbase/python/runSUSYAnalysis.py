#! /usr/bin/env python
import shutil, os, logging, re, sys
from datetime import datetime
import argparse

from XAMPPbase.Utils import *
logging.basicConfig(level=logging.INFO)

def setupSUSYArgParser(parser):    
    # Infile is required and should also be indicated as required when calling --help
    requiredNamed = parser.add_argument_group("required named arguments")
    requiredNamed.add_argument('-i', '--inFile', help='input dataset or file list')
    parser.add_argument('-o', '--outFile', help='name of the output file (default is \"AnalysisOutput.root\")', default='AnalysisOutput.root')
    parser.add_argument('-a', '--analysis', help='select the analysis you want to run on', default='MyCutFlow')
    parser.add_argument('--noSyst', help='run without systematic uncertainties', action='store_true', default=False)
    parser.add_argument('--nevents', type=int, help="number of events to process for all the datasets")
    parser.add_argument('--skipEvents', type=int, help="skip the first n events")
    if not ROOTCOREBIN and 'Ath' in ATLASPROJECT: parser.add_argument("--jobOptions", help="The athena jobOptions file to be executed", default = "XAMPPbase/runXAMPPbase.py")
  
    parser.add_argument('--PRWfiles' , help='List of the PRW input files', nargs='+' , default=['dev/SUSYTools/merged_prw_mc15c_latest.root', 'dev/PileupReweighting/mc15c_v2_defaults.NotRecommended.prw.root']) # PileupReweighting/mc15c_v2_defaults.NotRecommended.prw is used as default channel if mcChannelNumber is not found in SUSYTools/merged_prw_mc15c_latest
    parser.add_argument('--PRWlumi' , help='List of the PRW lumi config files', nargs='+' , default=['XAMPPbase/GRL/ilumicalc_histograms_HLT_mu15_L1MU6_313063-314170_OflLumi-HI-004.root', 'XAMPPbase/GRL/data15_13TeV.periodAllYear_DetStatus-v79-repro20-02_DQDefects-00-02-02_PHYS_StandardGRL_All_Good_25ns.root', 'XAMPPbase/GRL/data16_13TeV.periodAllYear_DetStatus-v88-pro20-21_DQDefects-00-02-04_PHYS_StandardGRL_All_Good_25ns.root'])
    parser.add_argument('--GRL' , help='List of the GRL', nargs='+', default=['XAMPPbase/GRL/data16_hip8TeV.periodAllYear_DetStatus-v86-pro20-19_DQDefects-00-02-04_PHYS_HeavyIonP_All_Good.xml', 'XAMPPbase/GRL/data15_13TeV.periodAllYear_DetStatus-v79-repro20-02_DQDefects-00-02-02_PHYS_StandardGRL_All_Good_25ns.xml', 'XAMPPbase/GRL/data16_13TeV.periodAllYear_DetStatus-v88-pro20-21_DQDefects-00-02-04_PHYS_StandardGRL_All_Good_25ns.xml'])
    parser.add_argument('--xSecDir' , help='List of the input cross-section files', default='')
    parser.add_argument('--STConfig', help='name of custom SUSYTools config file located in data folder', default='SUSYTools/SUSYTools_Default.conf')
   
    #########################################################################
    # These options are only available in ROOTCORE since the jobOptions allow the
    #########################################################################
    if ROOTCOREBIN or not 'Ath' in ATLASPROJECT:
        parser.add_argument('-r', '--runModus', help='select where to run', choices=['local', 'dCache', 'grid'], default='local')
        parser.add_argument('--debug', help='run in debug modus', action='store_true', default=False)
        parser.add_argument('--noCutFlow', help='run without CutFlow histos', action='store_true', default=False)
        parser.add_argument('--isData', action='store_true', default=False)
        
        parser.add_argument('--noTruth', help='disable truth particle analysis during running', action='store_true', default=False)
        parser.add_argument('--noHistos', help='disable writing of histograms (apart from cutflows) in analysis output file', action='store_true', default=False)
        parser.add_argument('--noTrees', help='disable writing of trees in analysis output file', action='store_true', default=False)
        parser.add_argument('--noxAODs', help='disable writing of xAODs in analysis output file', action='store_false', default=True)
        parser.add_argument('--IsPrimAOD', help='the input file-type is the primary xAOD', action='store_true', default=False)
 
        parser.add_argument('--isAF2', action='store_true', default=False)
        parser.add_argument('--noPRW', help='disable pileup-reweighting during running', action='store_true', default=False)
        parser.add_argument('--noJets', help='disables the processing of jets', action='store_true', default=False)
        parser.add_argument('--noBtag', help='switch off calculation of b-taggin weights', action='store_true', default=False)
        parser.add_argument('--noElectrons', help='disables the processing of electrons', action='store_true', default=False)
        parser.add_argument('--noMuons', help='disables the processing of muons', action='store_true', default=False)
        parser.add_argument('--noTaus', help='disable the processing of taus', action='store_true', default=False)
        parser.add_argument('--noPhotons', help='disables the processing of photons', action='store_true', default=False)
        parser.add_argument('--SeparateSF', help='If this option is activated the particular parts of the SF are stored in the output', action='store_true', default=False)
        parser.add_argument('--noSF', help='This option disables the SFs entirely', action='store_true', default=False)    
    return parser

def BringToAthenaStyle(Argument):
    Arg = Argument.replace("\"", "\\\"")
    Arg = Arg.replace("/share/", "/")
    Arg = Arg.replace("/data/","/")
    return Arg
def AssembleAthenaOptions(RunOptions, Parser = None, IsRemote= False):
     Options = []
     if RunOptions.noSyst and not IsArgumentDefault(RunOptions.noSyst, 'noSyst', Parser): Options.append("noSyst=True")
     if RunOptions.analysis and not IsArgumentDefault(RunOptions.analysis, 'analysis', Parser): Options.append("XAMPPanalysis='%s'"%(BringToAthenaStyle(RunOptions.analysis)))
     if RunOptions.STConfig and not IsArgumentDefault(RunOptions.STConfig,"STConfig", Parser): Options.append("STConfigFile='%s'"%(BringToAthenaStyle(RunOptions.STConfig)))
     if not IsRemote and RunOptions.inFile and not IsArgumentDefault(RunOptions.inFile, 'inFile', Parser): Options.append("inputFile='%s'"%(BringToAthenaStyle(RunOptions.inFile)))
     if not IsRemote and RunOptions.outFile and not IsArgumentDefault(RunOptions.outFile, 'outFile', Parser): Options.append("outFile='%s'"%(RunOptions.outFile.replace("\"", "\\\"")))
     if not IsRemote and RunOptions.nevents > 0: Options.append("nevents=%i"%(RunOptions.nevents))
     if not IsRemote and RunOptions.skipEvents > 0: Options.append("nskip=%i"%(RunOptions.skipEvents))
     return Options
def AssembleConfigArgument(ListOfArgs):
    if len(ListOfArgs)== 0: return ""
    return "-c \"%s\""%(";".join(ListOfArgs))
def AssembleRemoteRunCmd (RunOptions, Parser=None):
    Options = []
    if not ROOTCOREBIN and 'Ath' in ATLASPROJECT: return AssembleAthenaOptions(RunOptions,Parser,True)
    if RunOptions.analysis and not IsArgumentDefault(RunOptions.analysis, 'analysis', Parser): Options.append("--analysis " + RunOptions.analysis)
    if RunOptions.noSyst and not IsArgumentDefault(RunOptions.noSyst, 'noSyst', Parser): Options.append("--noSyst")
    if RunOptions.STConfig != '' and not IsArgumentDefault(RunOptions.STConfig, 'STConfig', Parser): Options.append("--STConfig " + RunOptions.STConfig)
    if len(RunOptions.PRWfiles) > 0 and not IsArgumentDefault(RunOptions.PRWfiles, 'PRWfiles', Parser): Options.append("--PRWfiles " + " ".join(RunOptions.PRWfiles))
    if len(RunOptions.PRWlumi) > 0 and not IsArgumentDefault(RunOptions.PRWlumi, 'PRWlumi', Parser): Options.append("--PRWlumi " + " ".join(RunOptions.PRWlumi))
    if len(RunOptions.GRL) > 0 and not IsArgumentDefault(RunOptions.GRL, 'GRL', Parser): Options.append("--GRL " + " ".join(RunOptions.GRL)) 
    if RunOptions.xSecDir != '' and not IsArgumentDefault(RunOptions.xSecDir, 'xSecDir', Parser): Options.append("--xSecDir " + RunOptions.xSecDir)
    ############################################################
    #    Parser options that are only available in ROOTCORE 20.7
    ############################################################
    if ROOTCOREBIN or not 'Ath' in ATLASPROJECT:
        if RunOptions.debug and not IsArgumentDefault(RunOptions.debug, 'debug', Parser): Options.append("--debug")
        if RunOptions.noCutFlow and not IsArgumentDefault(RunOptions.noCutFlow, 'noCutFlow', Parser): Options.append("--noCutFlow")
        if RunOptions.isData and not IsArgumentDefault(RunOptions.isData, 'isData', Parser): Options.append("--isData")
        if RunOptions.noTruth and not IsArgumentDefault(RunOptions.noTruth, 'noTruth', Parser): Options.append("--noTruth")
        if RunOptions.noHistos and not IsArgumentDefault(RunOptions.noHistos, 'noHistos', Parser): Options.append("--noHistos")
        if RunOptions.noTrees and not IsArgumentDefault(RunOptions.noTrees, 'noTrees', Parser): Options.append("--noTrees")
        if RunOptions.noxAODs and not IsArgumentDefault(RunOptions.noxAODs, 'noxAODs', Parser): Options.append("--noxAODs")
        if RunOptions.isAF2 and not IsArgumentDefault(RunOptions.isAF2, 'isAF2', Parser): Options.append("--isAF2")
        if RunOptions.noPRW and not IsArgumentDefault(RunOptions.noPRW, 'noPRW', Parser): Options.append("--noPRW")
        if RunOptions.noBtag and not IsArgumentDefault(RunOptions.noBtag, 'noBtag', Parser): Options.append("--noBtag")
        if RunOptions.noTaus and not IsArgumentDefault(RunOptions.noTaus, 'noTaus', Parser): Options.append("--noTaus")
        if RunOptions.noPhotons and not IsArgumentDefault(RunOptions.noPhotons, 'noPhotons', Parser): Options.append("--noPhotons")
        if RunOptions.SeparateSF and not IsArgumentDefault(RunOptions.SeparateSF, 'SeparateSF', Parser): Options.append("--SeparateSF")
        if RunOptions.IsPrimAOD == True and not IsArgumentDefault(RunOptions.IsPrimAOD,"IsPrimAOD",Parser): Options.append("--IsPrimAOD")
    return Options


######################################################################
######################################################################
#
#  The rest of the file is only needed in ROOTCORE based releases
#
######################################################################
######################################################################
   
   
   
def prepareAnalysis(ROOT):
    tstart = datetime.now()
    print str(tstart) + ' : Job started'
    
    #  Set batch mode and atlas style
    ROOT.gROOT.SetBatch(True)
    
    logging.info("loading packages")
    ROOT.gROOT.Macro("$ROOTCOREDIR/scripts/load_packages.C")

    #  Set up the job for xAOD access:
    ROOT.xAOD.Init().ignore();
    return tstart
        
def runAnalysisLoop(RunOptions, AnalysisLoop):
    
    setASGProperty(AnalysisLoop, 'std::string', 'InFile', RunOptions.inFile)
    setASGProperty(AnalysisLoop, 'bool', 'RunCutFlow', not RunOptions.noCutFlow)
    setASGProperty(AnalysisLoop, 'bool', 'IsPrimaryAOD', RunOptions.IsPrimAOD)
      
    if RunOptions.runModus == 'local' :
        print 'Running locally'
        setASGProperty(AnalysisLoop, 'int', 'RunType', 0)
    elif RunOptions.runModus == 'dCache' :
        print 'Running in dCache mode'
        setASGProperty(AnalysisLoop, 'int', 'RunType', 2)
    else : #  # RunOptions.runModus == 'grid' 
        print 'Running in grid mode\n\ncontent of input file:'
        os.system("cat " + RunOptions.inFile)
        setASGProperty(AnalysisLoop, 'int', 'RunType', 1)
        print '\n\n'
        
  
    if RunOptions.debug: setASGProperty(AnalysisLoop, 'int', 'OutputLevel', 2)
    if RunOptions.nevents: setASGProperty(AnalysisLoop, 'unsigned int', 'MaxEvents', RunOptions.nevents)
    if RunOptions.skipEvents: setASGProperty(AnalysisLoop, 'unsigned int', 'SkipEvents', RunOptions.skipEvents)    
  
    
    if RunOptions.debug :
        print 'running in debug mode...'
        setASGProperty(AnalysisLoop, 'int', 'PrintInterval', 100)

    if not AnalysisLoop.initialize().isSuccess(): sys.exit("ERROR: Could not initialize Loop, exiting")
    if not AnalysisLoop.ExecuteLoop().isSuccess(): sys.exit("ERROR: Could not run Loop, exiting")

def setASGProperty(tool, property_type, property_name, property_value, printMsg=True):
    if not tool.setProperty(property_type)(property_name, property_value).isSuccess():
        print "Error: Could not setProperty(\"%s\", %s) for %s, exiting..." % (property_name, str(property_value), str(tool))
        sys.exit(1)
    elif printMsg: print "INFO: Manually set property \"%s\" to %s for ASGTool %s" % (property_name, str(property_value), str(tool))
        

def finalizeAnalysis(start_time, AnalysisHelper):
    import ROOT
    if not AnalysisHelper.finalize().isSuccess():
        sys.exit("ERROR: Problem with output finalization, exiting")
    tend = datetime.now()
    print str(tend) + ' : Job finished'
    tdelta = tend - start_time
    print str(tdelta) + ' : Total Duration'
    ROOT.xAOD.ClearTransientTrees()

def setupSUSYAnalysis(RunOptions, JobConfig, AnalysisHelper, SystematicsTool):
    
    
    logging.info("Configuring the Analysis job")

    if RunOptions.noTruth or RunOptions.isData:
        setASGProperty(AnalysisHelper, 'bool', 'doTruth', False)
     
    OutFileName = RunOptions.outFile
    if OutFileName.split('.')[-1] == 'root':
        setASGProperty(AnalysisHelper, 'std::string', 'OutFileName', OutFileName)
    else : 
        setASGProperty(AnalysisHelper, 'std::string', 'OutFileName', OutFileName + '.root')

    if RunOptions.noSyst: setASGProperty(SystematicsTool, 'bool', 'doSyst', False)
        
    if RunOptions.noHistos: setASGProperty(AnalysisHelper, 'bool', 'doHistos', False)
    if RunOptions.noTrees: setASGProperty(AnalysisHelper, 'bool', 'doTrees', False)
    if RunOptions.noCutFlow: setASGProperty(AnalysisHelper, 'bool', 'RunCutFlow', False)
    if RunOptions.noxAODs: setASGProperty(AnalysisHelper, 'bool', 'doxAODs', False)
    
    setASGProperty(AnalysisHelper, 'bool', 'doxAODs', False)
    setASGProperty(AnalysisHelper, 'bool', 'isAF2', RunOptions.isAF2)
    setASGProperty(AnalysisHelper, 'std::string', 'STConfigFile', RunOptions.STConfig)
    
    if RunOptions.noPRW : setASGProperty(AnalysisHelper, 'bool', 'doPRW', False)   
    
    setASGProperty(AnalysisHelper, 'std::string', 'PRWLumiCalcFilesString', ",".join(RunOptions.PRWlumi))
    prwFiles = RunOptions.PRWfiles
    if len(prwFiles) == 0:
        if ROOTCOREBIN:
            for P in os.listdir(os.getenv("ROOTCOREBIN") + "/data/XAMPPbase/prwFiles/"):
                if P.find(".root") < 0: continue               
                prwFiles.append('XAMPPbase/prwFiles/' + P)
        else:
            for P in os.listdir(os.getenv("TestArea") + "/XAMPPbase/data/prwFiles/"):
                if P.find(".root") < 0: continue               
                prwFiles.append('XAMPPbase/prwFiles/' + P)
    setASGProperty(AnalysisHelper, 'std::string', 'PRWConfigFilesString', ",".join(prwFiles))
    
    if RunOptions.isData:
        setASGProperty(SystematicsTool, 'bool', 'doSyst', False)
        setASGProperty(SystematicsTool, 'bool', 'isData', True)
        setASGProperty(SystematicsTool, 'bool', 'doWeights', False)
        setASGProperty(AnalysisHelper, 'string', 'GoodRunsListVecString', ",".join(RunOptions.GRL))
        
    if RunOptions.xSecDir != '': setASGProperty(AnalysisHelper, 'string', 'STCrossSectionDB', RunOptions.xSecDir)
    
    if RunOptions.noSF: setASGProperty(SystematicsTool, 'bool', 'doWeights', False)
    
    setASGProperty(SystematicsTool, 'bool', 'doNoJets', RunOptions.noJets)
    setASGProperty(SystematicsTool, 'bool', 'doNoBtag', RunOptions.noBtag)
    setASGProperty(SystematicsTool, 'bool', 'doNoElectrons', RunOptions.noElectrons)
    setASGProperty(SystematicsTool, 'bool', 'doNoMuons', RunOptions.noMuons)
    setASGProperty(SystematicsTool, 'bool', 'doNoTaus', RunOptions.noTaus)
    setASGProperty(SystematicsTool, 'bool', 'doNoPhotons', RunOptions.noPhotons)
    
    setASGProperty(AnalysisHelper.SUSYJetSelector,"float", "SignalPtCut", GetKinematicCutFromConfFile(RunOptions.STConfig, "Jet.Pt"))
    setASGProperty(AnalysisHelper.SUSYJetSelector,"float", "SignalEtaCut", GetKinematicCutFromConfFile(RunOptions.STConfig, "Jet.Eta"))
    
    if RunOptions.SeparateSF:
        setASGProperty(AnalysisHelper.SUSYElectronSelector, 'bool', 'SeparateSF' , RunOptions.SeparateSF)
        setASGProperty(AnalysisHelper.SUSYMuonSelector, 'bool', 'SeparateSF' , RunOptions.SeparateSF)
        setASGProperty(AnalysisHelper.SUSYPhotonSelector, 'bool', 'SeparateSF' , RunOptions.SeparateSF)
        setASGProperty(AnalysisHelper.SUSYJetSelector, 'bool', 'SeparateSF' , RunOptions.SeparateSF)
        setASGProperty(AnalysisHelper.SUSYTauSelector, 'bool', 'SeparateSF' , RunOptions.SeparateSF)

    
#####
#  simple SUSY analysis example
#####

if __name__ == "__main__":    
    import ROOT    
    parser = argparse.ArgumentParser(description='This script starts the analysis code. For more help type \"python XAMPPbase/python/runSUSYAnalysis.py -h\"', prog='runSUSYAnalysis', formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    RunOptions = setupSUSYArgParser(parser).parse_args()
    start_time = prepareAnalysis(ROOT)
    #  create a new analysis loop instance
    logging.info("setting up the loop algorithm")
    AnaLoop = ROOT.XAMPP.xAODLoop("xAODLoop")
#     setASGProperty(AnaLoop, 'int', 'OutputLevel', 1)
    setASGProperty(AnaLoop, 'bool', 'PrintProgress', False)

    #  create a new analysis helper instance
    logging.info("creating algorithms with AnalysisHelp")
    AnalysisHelp = ROOT.XAMPP.SUSYAnalysisHelper("AnalysisHelper")
    #setASGProperty(AnalysisHelp, 'int', 'OutputLevelSUSYTools', 1)
#     setASGProperty(AnalysisHelp, 'int', 'OutputLevel', 1)
    
    #  create a new configuration instance and parse its name to the AnalysisHelper
    JobConfig = ROOT.XAMPP.AnalysisConfig("AnaWithXAMPP")
   
    # the name of the AnalysisConfig (if not "SUSYAnalysisConfig") should be also piped to the AnalysisHelper:
    setASGProperty(AnalysisHelp, 'std::string', 'AnalysisConfigName', "AnaWithXAMPP")
    
    #  create and add object selectors
    AnalysisHelp.SUSYElectronSelector = ROOT.XAMPP.SUSYElectronSelector("SUSYElectronSelector")
     
    AnalysisHelp.SUSYMuonSelector = ROOT.XAMPP.SUSYMuonSelector("SUSYMuonSelector")
    AnalysisHelp.SUSYJetSelector = ROOT.XAMPP.SUSYJetSelector("SUSYJetSelector")
    AnalysisHelp.SUSYTauSelector = ROOT.XAMPP.SUSYTauSelector("SUSYTauSelector")
    AnalysisHelp.SUSYPhotonSelector = ROOT.XAMPP.SUSYPhotonSelector("SUSYPhotonSelector")
    AnalysisHelp.SUSYMetSelector = ROOT.XAMPP.SUSYMetSelector("SUSYMetSelector")   
    
    #Switch off the electron SFs
    setASGProperty(AnalysisHelp.SUSYElectronSelector, "bool", "ApplyTriggerSF" , False)
#     setASGProperty(AnalysisHelp.SUSYElectronSelector, "bool", "ApplyIsoSF" , False)
#     setASGProperty(AnalysisHelp.SUSYElectronSelector, "bool", "ApplyRecoSF" , False)
#     setASGProperty(AnalysisHelp.SUSYElectronSelector, "bool", "ApplyIdSF" , False)
    
    ##Switch of the SFs of the particular tools
    setASGProperty(AnalysisHelp.SUSYMuonSelector, "bool", "ApplyTriggerSF" , False)
#     setASGProperty(AnalysisHelp.SUSYMuonSelector, "bool", "ApplyIsoSF" , False)
#     setASGProperty(AnalysisHelp.SUSYMuonSelector, "bool", "ApplyTTVASF" , False)
#     setASGProperty(AnalysisHelp.SUSYMuonSelector, "bool", "ApplyRecoSF" , False)
    
    
    #  create and add systematics and trigger tools
    AnalysisHelp.SystematicsTool = ROOT.XAMPP.SUSYSystematics("SystematicsTool")
    
    AnalysisHelp.TriggerTool = ROOT.XAMPP.SUSYTriggerTool("TriggerTool")
    METTriggers = [
        "HLT_xe70_tc_lcw",
        "HLT_xe80_tc_lcw",
        "HLT_xe80_mht",
        "HLT_xe90",
        "HLT_xe90_tc_lcw",
        "HLT_xe90_mht",
        "HLT_xe100_L1XE50",
        "HLT_xe100",
        "HLT_xe100_tc_em",
        "HLT_xe100_mht",
        "HLT_xe110",
        "HLT_xe110_tc_lcw",
        "HLT_xe120",
        "HLT_xe120_tc_lcw",
        "HLT_xe120_mht",
        ]
    SingleLepTriggers = [
    	"HLT_mu15_L1MU10",
        "HLT_mu15_L1MU6",
        "HLT_2mu4",
        "HLT_mu4",
        "HLT_mu6",
        "HLT_mu8",
        "HLT_mu14",
        "HLT_mu15"]
    DiLepTriggers = [
        "HLT_2e12_lhloose_L12EM10VH",
        "HLT_2e15_lhloose_L12EM13VH",
        "HLT_2mu10",
        "HLT_2mu14",
        "HLT_e7_lhmedium_mu24",
        "HLT_e17_lhloose_mu14"]
    TriLepTriggers = [
        "HLT_e17_lhmedium_2e9_lhmedium",
        "HLT_2e12_lhloose_mu10",
        "HLT_e12_lhloose_2mu10",
        "HLT_3mu6"]
    setASGProperty(AnalysisHelp.TriggerTool, "bool","WritePrescaling" , True)
    setASGProperty(AnalysisHelp.TriggerTool, "bool","DisableSkimming" , True)
    setASGProperty(AnalysisHelp.TriggerTool, 'std::string', 'METTriggersString', ",".join(METTriggers))
    setASGProperty(AnalysisHelp.TriggerTool, 'std::string', '1LeptonTriggersString', ",".join(SingleLepTriggers))
    setASGProperty(AnalysisHelp.TriggerTool, 'std::string', '2LeptonTriggersString', ",".join(DiLepTriggers))
    setASGProperty(AnalysisHelp.TriggerTool, 'std::string', '3LeptonTriggersString', ",".join(TriLepTriggers))
    
    if not RunOptions.isData:
        AnalysisHelp.SUSYTruthSelector = ROOT.XAMPP.SUSYTruthSelector("SUSYTruthSelector")

    
    setupSUSYAnalysis(RunOptions, JobConfig, AnalysisHelp, AnalysisHelp.SystematicsTool)

    

    runAnalysisLoop(RunOptions, AnaLoop)


    finalizeAnalysis(start_time, AnalysisHelp)
