#include the common library instead of importing it. So we have full access to the globals
include ("XAMPPanalyses/Stop0LToolSetup.py")
AssembleIO()
############################################################################
#       Initialize the SystematicsTool first to switch off Taus and Photons
#############################################################################
SetupSystematicsTool(noJets=False,noBtag=False,noElectrons=False,noMuons=False,noTaus=True,noDiTaus=True,noPhotons=True)

##############################################
#       Setup the JVT AnalysisHelper + Config
##############################################
SetupJVTAnalysisHelper(TreeName= "JVT", CutFlowsToRun = [])

############################################
#       The JVT run script uses the Stop0LMetSelector
############################################
SetupStop0LMetSelector().IncludePhotons = False
SetupStop0LMetSelector().IncludeTaus = False
SetupStop0LMetSelector().DoTrackMet = False

###############################################################
#       Switch off the TriggerSFs for Electron and MuonSelector
###############################################################
SetupSUSYElectronSelector().ApplyTriggerSF = False
SetupSUSYMuonSelector().ApplyTriggerSF = False

##############################################
#       Configure the JetSelector
###############################################
       
SetupSUSYJetSelector().bJetEtaCut = 2.5
SetupSUSYJetSelector().SeparateSF = True
########################################
#       Setup the trigger tool
#######################################
SingleLepTriggers = [
        "HLT_mu20_iloose_L1MU15", # used by Stop0L 2015 2LCR (mumu) cutflow comparison   
        "HLT_mu50", # used by Stop0L 2015/2016 2LCR (mumu) cutflow comparison
        "HLT_mu26_imedium", # used by Stop0L 2016 2LCR (mumu) cutflow comparison
        "HLT_mu24_iloose_L1MU15",
        "HLT_mu40",
        "HLT_mu24_ivarmedium",
        "HLT_mu24_iloose",
        "HLT_mu24_ivarloose",
        "HLT_mu24_ivarloose_L1MU15",
        "HLT_mu24_imedium",
    ]
SetupSUSYTriggerTool().SingleLeptonTriggers = SingleLepTriggers

##################################################
#       Sign off the configuration and load common options like GRL, PRW etc.
##################################################
ParseBasicConfigsToHelper( STFile = "XAMPPanalyses/SUSYTools_JVT.conf")

SetupAlgorithm()

