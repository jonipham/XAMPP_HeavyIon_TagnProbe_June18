import argparse
import os
def IsTextFile(FileName):
     FileFormats = [".txt" , ".list", ".conf" ]
     for end in FileFormats:
         if FileName.endswith(end): return True
     return False
def AssembleIO():    
    #--------------------------------------------------------------
    # Reduce the event loop spam a bit
    #--------------------------------------------------------------
    if os.path.exists("%s/athfile-cache.ascii.gz" % (os.getcwd())):
        print "Old athfile-cache found. Will delete it otherwise athena just freaks out. This little boy"
        os.system("rm %s/athfile-cache.ascii.gz" % (os.getcwd()))
    from GaudiSvc.GaudiSvcConf import THistSvc
    from AthenaCommon.JobProperties import jobproperties
    import AthenaPoolCnvSvc.ReadAthenaPool
    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf
    from AthenaServices.AthenaServicesConf import AthenaEventLoopMgr
    from AthenaCommon.AppMgr import ServiceMgr
    from RecExConfig import AutoConfiguration
    from PathResolver import PathResolver
    from XAMPPbase.Utils import ReadListFromFile   
    ServiceMgr += AthenaEventLoopMgr(EventPrintoutInterval=5000)
   
    ServiceMgr += THistSvc()
    OutFileName = "AnalysisOutput.root" if not "outFile" in globals() else outFile
    ServiceMgr.THistSvc.Output += ["XAMPP DATAFILE='{}' OPT='RECREATE'".format(OutFileName)]
    ROOTFiles = []
    
    if "inputFile" in globals():
        print "Use the following %s as input" % (inputFile)
        ROOTFiles = []
        ResolvedInFile = PathResolver.FindCalibFile(inputFile)
        ReolvedInDir = PathResolver.FindCalibDirectory(inputFile)
        if os.path.isfile(ResolvedInFile):
            if IsTextFile(ResolvedInFile): ROOTFiles = ReadListFromFile(ResolvedInFile)
            else: ROOTFiles.append(ResolvedInFile)
                                             
        elif os.path.isdir(ReolvedInDir):
          for DirEnt in os.listdir(ReolvedInDir):
              if DirEnt.endswith(".root"):
                  ROOTFiles.append(DirEnt)
        else: raise RuntimeError("Invalid input " + inputFile)
        if len(ROOTFiles) == 0: raise RuntimeError("No ROOT files could be loaded as input")
        ServiceMgr.EventSelector.InputCollections = ROOTFiles
        acf.FilesInput = ROOTFiles
       
    if len(ROOTFiles) > 0: AutoConfiguration.ConfigureSimulationOrRealData()
    if "nevents" in globals():
        print "Only run on %i events" % (int(nevents))
        theApp.EvtMax = int (nevents)
    if "nskip" in globals():
        print "Skip the first %i events" % (int(nskip))
        ServiceMgr.EventSelector.SkipEvents = int(nksip)
    if isData(): print "INFO: We're running over data today"
    elif isAF2(): print "INFO: Please fasten your seatbelt the journey will be on Atlas fast "
    else: print "INFO: Fullsimulation. Make sure that you wear your augmented reality glasses"   
def isData():
    from AthenaCommon.AppMgr import ServiceMgr
    if len(ServiceMgr.EventSelector.InputCollections) == 0: 
        print "WARNING: No infiles were configured thus far"
        return False
    from PyUtils import AthFile
    af = AthFile.fopen(ServiceMgr.EventSelector.InputCollections[0])
    return "data" in af.fileinfos['tag_info']['project_name']
def isAF2():
    if isData():return False
    # currently the best check we know about
    from AthenaCommon.AppMgr import ServiceMgr
    from PyUtils import AthFile
    if len(ServiceMgr.EventSelector.InputCollections) == 0: 
        print "WARNING: No infiles were configured thus far"
        return False
    af = AthFile.fopen(ServiceMgr.EventSelector.InputCollections[0])
    if 'tag_info' in af.fileinfos:
        for key in af.fileinfos['tag_info'].iterkeys():
            if 'AtlfastII' in key or 'Fast' in key: return True
    else: print 'shit, it does not work :-('
    return False
def isRelease21():
    # currently the best check we know about, feel free to improve
    import os
    return os.getenv("AtlasVersion").startswith("21") or os.getenv("AtlasArea").startswith("/cvmfs/atlas.cern.ch/repo/sw/ASG/2.6/")
def isOnDAOD():
    from AthenaCommon.AppMgr import ServiceMgr
    from PyUtils import AthFile
    if len(ServiceMgr.EventSelector.InputCollections) == 0:
        print "WARNING: No inputfiles given"
        return True
    af = AthFile.fopen(ServiceMgr.EventSelector.InputCollections[0])  # opens the first file from the InputCollections list
    # af.fileinfos #this is a dict of dicts, take a look at what's available (e.g. do af.fileinfos.keys() to see the main keys)! Below are some examples:
    streamName = af.fileinfos['stream_names']  # will be something like 'StreamDAOD_XXX' if a derivation
    return "DAOD" in streamName[0]
def isTRUTH3():
    if isData(): return False
    from AthenaCommon.AppMgr import ServiceMgr
    from PyUtils import AthFile
    if len(ServiceMgr.EventSelector.InputCollections) == 0:
        print "WARNING: No inputfiles given"
        return True
    af = AthFile.fopen(ServiceMgr.EventSelector.InputCollections[0])  # opens the first file from the InputCollections list
    # af.fileinfos #this is a dict of dicts, take a look at what's available (e.g. do af.fileinfos.keys() to see the main keys)! Below are some examples:
    streamName = af.fileinfos['stream_names'][0]  # will be something like 'StreamDAOD_XXX' if a derivation
    return "TRUTH3" in streamName
def SetupSystematicsTool(
        noJets=False, noBtag=False,
        noElectrons=False, noMuons=False,
        noTaus=False, noDiTaus=True,
        noPhotons=False):
    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon import CfgMgr, GlobalFlags
    if not hasattr(ToolSvc, "SystematicsTool"):
        from XAMPPbase.XAMPPbaseConf import XAMPP__SUSYSystematics
        SystTool = CfgMgr.XAMPP__SUSYSystematics("SystematicsTool")
        SystTool.doNoJets = noJets
        SystTool.doNoBtag = noBtag
        SystTool.doNoElectrons = noElectrons
        SystTool.doNoMuons = noMuons
        SystTool.doNoTaus = noTaus
        SystTool.doNoDiTaus = noDiTaus
        SystTool.doNoPhotons = noPhotons
        if isData():
            print "The input is data. Set the SystematicsTool to data"
            SystTool.isData = True
            SystTool.doSyst = False
            SystTool.doWeights = False
        
        if "noSyst" in globals():
            print "Switch off the systematics as it is configured by the user"
            SystTool.doSyst = False
        ToolSvc += SystTool
    return getattr(ToolSvc , "SystematicsTool")
def SetupSUSYElectronSelector():
    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon import CfgMgr    
    if not hasattr(ToolSvc, "SUSYElectronSelector"):
           from XAMPPbase.XAMPPbaseConf import XAMPP__SUSYElectronSelector
           EleSelector = CfgMgr.XAMPP__SUSYElectronSelector("SUSYElectronSelector")
           EleSelector.SeparateSF = True
           ToolSvc += EleSelector

    return getattr(ToolSvc, "SUSYElectronSelector")

def SetupSUSYMuonSelector():
    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon import CfgMgr    
    if not hasattr(ToolSvc, "SUSYMuonSelector"):
           from XAMPPbase.XAMPPbaseConf import XAMPP__SUSYMuonSelector
           MuoSelector = CfgMgr.XAMPP__SUSYMuonSelector("SUSYMuonSelector")
           ToolSvc += MuoSelector

    return getattr(ToolSvc, "SUSYMuonSelector")

def SetupSUSYJetSelector():
    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon import CfgMgr    
    if not hasattr(ToolSvc, "SUSYJetSelector"):
           from XAMPPbase.XAMPPbaseConf import XAMPP__SUSYJetSelector
           JetSelector = CfgMgr.XAMPP__SUSYJetSelector("SUSYJetSelector")
           ToolSvc += JetSelector

    return getattr(ToolSvc, "SUSYJetSelector")

def SetupSUSYTauSelector():
    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon import CfgMgr    
    if not hasattr(ToolSvc, "SUSYTauSelector"):
           from XAMPPbase.XAMPPbaseConf import XAMPP__SUSYTauSelector
           TauSelector = CfgMgr.XAMPP__SUSYTauSelector("SUSYTauSelector")         
           ToolSvc += TauSelector

    return getattr(ToolSvc, "SUSYTauSelector")

def SetupSUSYPhotonSelector():
    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon import CfgMgr    
    if not hasattr(ToolSvc, "SUSYPhotonSelector"):
           from XAMPPbase.XAMPPbaseConf import XAMPP__SUSYPhotonSelector
           PhotonSelector = CfgMgr.XAMPP__SUSYPhotonSelector("SUSYPhotonSelector")
           ToolSvc += PhotonSelector
    return getattr(ToolSvc, "SUSYPhotonSelector")

def SetupSUSYMetSelector():
    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon import CfgMgr    
    if not hasattr(ToolSvc, "SUSYMetSelector"):
           from XAMPPbase.XAMPPbaseConf import XAMPP__SUSYMetSelector
           MetSelector = CfgMgr.XAMPP__SUSYMetSelector("SUSYMetSelector")
           ToolSvc += MetSelector
    return getattr(ToolSvc, "SUSYMetSelector")

def SetupSUSYTriggerTool():
    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon import CfgMgr    
    if not hasattr(ToolSvc, "TriggerTool"):
           from XAMPPbase.XAMPPbaseConf import XAMPP__SUSYTriggerTool
          
           TriggerTool = CfgMgr.XAMPP__SUSYTriggerTool("TriggerTool")
           TriggerTool.SystematicsTool = SetupSystematicsTool()
           TriggerTool.ElectronSelector = SetupSUSYElectronSelector()
           TriggerTool.MuonSelector = SetupSUSYMuonSelector()
           # TriggerTool.JetSelector = SetupSUSYJetSelector()
           TriggerTool.TauSelector = SetupSUSYTauSelector()
           TriggerTool.PhotonSelector = SetupSUSYPhotonSelector() 
           ToolSvc += TriggerTool
    return getattr(ToolSvc, "TriggerTool")
def SetupTruthSelector():
    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon import CfgMgr    
    if not hasattr(ToolSvc, "SUSYTruthSelector"):
        from XAMPPbase.XAMPPbaseConf import XAMPP__SUSYTruthSelector 
        TruthSelector = CfgMgr.XAMPP__SUSYTruthSelector("SUSYTruthSelector")
        ToolSvc += TruthSelector
    return getattr(ToolSvc, "SUSYTruthSelector")

def SetupTruthAnalysisConfig(Name):
    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon import CfgMgr    
    if not hasattr(ToolSvc, "TruthAnalysisConfig"):
           from XAMPPbase.XAMPPbaseConf import XAMPP__TruthAnalysisConfig 
           AnaConfig = CfgMgr.XAMPP__TruthAnalysisConfig("TruthAnalysisConfig")
           AnaConfig.TreeName = Name
           ToolSvc += AnaConfig

    return getattr(ToolSvc, "TruthAnalysisConfig")

def SetupSUSYTruthAnalysisHelper(TreeName="TruthTree"):
    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon import CfgMgr    
  
    if not hasattr(ToolSvc, "AnalysisHelper"):
        from XAMPPbase.XAMPPbaseConf import XAMPP__SUSYTruthAnalysisHelper
        
        TruthHelper = CfgMgr.XAMPP__SUSYTruthAnalysisHelper("AnalysisHelper")
        ToolSvc += TruthHelper
        TruthHelper.SystematicsTool = SetupSystematicsTool()
        TruthHelper.TruthSelector = SetupTruthSelector()
        TruthHelper.AnalysisConfig = SetupTruthAnalysisConfig(TreeName)
    return getattr(ToolSvc, "AnalysisHelper")

def SetupAnalysisConfig(Name):
    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon import CfgMgr    
    if not hasattr(ToolSvc, "AnalysisConfig"):
           from XAMPPbase.XAMPPbaseConf import XAMPP__AnalysisConfig 
           AnaConfig = CfgMgr.XAMPP__AnalysisConfig(
                                            name="AnalysisConfig",
                                            TreeName=Name)
           ToolSvc += AnaConfig

    return getattr(ToolSvc, "AnalysisConfig")

def SetupAnalysisHelper(TreeName="XAMPPBaseTree"):
    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon import CfgMgr    
    if not hasattr(ToolSvc, "AnalysisHelper"):
        from XAMPPbase.XAMPPbaseConf import XAMPP__SUSYAnalysisHelper
        BaseHelper = CfgMgr.XAMPP__SUSYAnalysisHelper(name="AnalysisHelper")
        ToolSvc += BaseHelper
        BaseHelper.AnalysisConfig = SetupAnalysisConfig(TreeName)
        
    return getattr(ToolSvc, "AnalysisHelper")
     
def SetupAlgorithm():
    from AthenaCommon.AlgSequence import AlgSequence
    job = AlgSequence()
    if not hasattr(job, "XAMPPAlgorithm"):
        from XAMPPbase.XAMPPbaseConf import XAMPP__XAMPPalgorithm
        from PyUtils import AthFile
        thisAlg = XAMPP__XAMPPalgorithm("XAMPPAlgorithm")
        thisAlg.AnalysisHelper = SetupAnalysisHelper()
        thisAlg.SystematicsTool = SetupSystematicsTool() 
        job += thisAlg
        print "Created XAMPP algorithm"
    return getattr(job, "XAMPPAlgorithm")
                    

def ParseBasicConfigsToHelper(
                STFile="SUSYTools/SUSYTools_Default.conf",
                xSecDB="SUSYTools/mc15_13TeV/",
                SeparateSF=True):
    
    BaseHelper = SetupAnalysisHelper()
    BaseHelper.STConfigFile = STFile if not "STConfigFile" in globals() else STConfigFile
    BaseHelper.isAF2 = isAF2()
    if isData():
        if not isRelease21():
            BaseHelper.GoodRunsLists = [
                            'XAMPPbase/GRL/data15_13TeV.periodAllYear_DetStatus-v79-repro20-02_DQDefects-00-02-02_PHYS_StandardGRL_All_Good_25ns.xml',
                            'XAMPPbase/GRL/data16_13TeV.periodAllYear_DetStatus-v88-pro20-21_DQDefects-00-02-04_PHYS_StandardGRL_All_Good_25ns.xml']
        else:
            BaseHelper.GoodRunsLists = [
                            'XAMPPbase/GRL/data15_13TeV.periodAllYear_DetStatus-v89-pro21-02_Unknown_PHYS_StandardGRL_All_Good_25ns.xml',
                            'XAMPPbase/GRL/data16_13TeV.periodAllYear_DetStatus-v89-pro21-01_DQDefects-00-02-04_PHYS_StandardGRL_All_Good_25ns.xml',
                            
                            ]
     
            
    else: BaseHelper.STCrossSectionDB = xSecDB
    if not isRelease21():  # !R21
        BaseHelper.PRWConfigFiles = [
                            "dev/SUSYTools/merged_prw_mc15c_latest.root",
                            "dev/PileupReweighting/mc15c_v2_defaults.NotRecommended.prw.root"                        
                            ]
        BaseHelper.PRWLumiCalcFiles = [
                        'XAMPPbase/GRL/data15_13TeV.periodAllYear_DetStatus-v79-repro20-02_DQDefects-00-02-02_PHYS_StandardGRL_All_Good_25ns.root',
                        'XAMPPbase/GRL/data16_13TeV.periodAllYear_DetStatus-v88-pro20-21_DQDefects-00-02-04_PHYS_StandardGRL_All_Good_25ns.root'
                        ]
    else:  # R21
        BaseHelper.PRWConfigFiles = [
                            "dev/SUSYTools/merged_prw_mc15c_latest.root",
                            "dev/PileupReweighting/mc15c_v2_defaults.NotRecommended.prw.root"                        
#             "/ptmp/mpp/zenon/DirectStau/DAOD_SUSY18.DAOD.mc16_13TeV.361108.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Ztautau.merge.AOD.e3601_s3126_r9546_r9551.root"
                            ]   
        BaseHelper.PRWLumiCalcFiles = [
                        'XAMPPbase/GRL/data15_13TeV.periodAllYear_DetStatus-v89-pro21-02_Unknown_PHYS_StandardGRL_All_Good_25ns.root',
                        'XAMPPbase/GRL/data16_13TeV.periodAllYear_DetStatus-v89-pro21-01_DQDefects-00-02-04_PHYS_StandardGRL_All_Good_25ns.root'
                        ]

    BaseHelper.SystematicsTool = SetupSystematicsTool()
    BaseHelper.ElectronSelector = SetupSUSYElectronSelector()
    BaseHelper.MuonSelector = SetupSUSYMuonSelector()
    BaseHelper.JetSelector = SetupSUSYJetSelector()
    BaseHelper.TruthSelector = SetupTruthSelector()
    BaseHelper.TriggerTool = SetupSUSYTriggerTool()
    BaseHelper.TauSelector = SetupSUSYTauSelector()
    BaseHelper.PhotonSelector = SetupSUSYPhotonSelector()        
    BaseHelper.MetSelector = SetupSUSYMetSelector()
    if SeparateSF:
          SetupSUSYElectronSelector().SeparateSF = True
          SetupSUSYPhotonSelector().SeparateSF = True
          SetupSUSYMuonSelector().SeparateSF = True
          SetupSUSYJetSelector().SeparateSF = True
          SetupSUSYTauSelector().SeparateSF = True
    from XAMPPbase.Utils import GetKinematicCutFromConfFile      
    SetupSUSYJetSelector().SignalPtCut = GetKinematicCutFromConfFile(STFile, "Jet.Pt")
    SetupSUSYJetSelector().SignalEtaCut = GetKinematicCutFromConfFile(STFile, "Jet.Eta")
    
    SetupSUSYTauSelector().BaselinePtCut = GetKinematicCutFromConfFile(STFile, "Tau.PtPrePtCut")
    SetupSUSYTauSelector().BaselineEtaCut = GetKinematicCutFromConfFile(STFile, "Tau.Eta")
#     SetupSUSYTauSelector().PreSelectionDecorator = "baselineID"
    
    if isRelease21() and not isData() and not isOnDAOD():
        print "Disable TRUTH jets as they are not in AODs"
        SetupTruthSelector().doJets = False
        print "Will also disable the JVT efficiencies as well"
        SetupSUSYJetSelector().ApplyJVTSF = False
 
def ParseTruthOptionsToHelper():
    if isData():
        print "Are you sure? You want to run TRUTH on data?!. 46x + 87y / #sqrt{90} ?!"
        exit(1)
    SetupSystematicsTool().doNoJets = True
    SetupSystematicsTool().doNoBtag = True
    SetupSystematicsTool().doNoElectrons = True
    SetupSystematicsTool().doNoMuons = True
    SetupSystematicsTool().doNoTaus = True
    SetupSystematicsTool().doNoDiTaus = True
    SetupSystematicsTool().doNoPhotons = True
    if isTRUTH3():
        print "The input seems to be TRUTH3"
        SetupTruthSelector().doTruthParticles = False
        SetupTruthSelector().isTRUTH3 = True
        SetupTruthSelector().ElectronContainer = "TruthElectrons"
        SetupTruthSelector().MuonContainer = "TruthMuons"
#         SetupTruthSelector().TauContainer = "TruthTaus"
        SetupTruthSelector().PhotonContainer = "Truth3Photons"
        SetupTruthSelector().NeutrinoContainer = "TruthNeutrinos"
        SetupTruthSelector().BosonContainer = "TruthBoson"
        SetupTruthSelector().BSMContainer = "TruthBSM"
      
    
