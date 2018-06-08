#include the common library instead of importing it. So we have full access to the globals
include ("XAMPPanalyses/Stop0LToolSetup.py")
AssembleIO()

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
        "HLT_e120_lhloose",
        # 2016
        "HLT_e24_lhtight_nod0_ivarloose",
        "HLT_e60_lhmedium_nod0",
        "HLT_e60_medium",
        "HLT_e140_lhloose_nod0",
        "HLT_e300_etcut",
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
        "HLT_mu50",
        "HLT_mu24_ivarmedium",
        "HLT_mu24_imedium",
        "HLT_mu26_ivarmedium",
        "HLT_mu26_imedium",
        ]
PhotonTriggers = [
            "HLT_g120_loose", # used by Stop0L 2015 (gamma) cutflow comparison    
            "HLT_g140_loose", # used by Stop0L 2016 (gamma) cutflow comparison    
            ] 
#     # do this for the 2LCR cutflow comparison
#         SingleLepTriggers = [ 
#             "HLT_e24_lhmedium_L1EM18VH",
#             "HLT_e120_lhloose",
#             "HLT_e60_lhmedium",
#             "HLT_e60_lhmedium_nod0",
#             "HLT_e26_lhtight_nod0_ivarloose",
#             "HLT_e140_lhloose_nod0",
#             "HLT_mu50",
#             "HLT_mu20_iloose_L1MU15",
#             "HLT_mu26_ivarmedium",
#             ]

SetupSystematicsTool(noJets=False,noBtag=False,noElectrons=False,noMuons=False,noTaus=True,noDiTaus=True,noPhotons=False)

CutFlowsToRun = []
if "CutFlow" in globals():
    if CutFlow == 'Stop0L_LCR':
        CutFlowsToRun = ['Stop0L_1LCR', 'Stop0L_2LCR']
    elif CutFlow == 'Stop0L_SR' or CutFlow == 'Stop0L_SRHard' or CutFlow == 'Stop0L_1LCR' or CutFlow == 'Stop0L_2LCR' or CutFlow == 'Stop0L_ISR_SR' or CutFlow == 'Stop0L_1Y':
        CutFlowsToRun = [CutFlow]
        
if len(CutFlowsToRun)==1 and 'Stop0L_2LCR' in CutFlowsToRun:
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

SetupStop0LAnalysisHelper(CutFlowsToRun=CutFlowsToRun)

if "doGamma" in globals():
    ParseBasicConfigsToHelper( STConfigFile = "XAMPPanalyses/SUSYTools_Stop0LGamma.conf" )
else:
    ParseBasicConfigsToHelper( STConfigFile = "XAMPPanalyses/SUSYTools_Stop0L.conf" )

TriggerTool = SetupSUSYTriggerTool()
isSR = False
for cf in CutFlowsToRun:
    if 'SR' in cf:
        isSR = True
if len(CutFlowsToRun)==0 or isSR or 'Stop0L_1LCR' in CutFlowsToRun:
    TriggerTool.METTriggers = METTriggers
TriggerTool.OfflineThreshold = 2000
TriggerTool.WritePrescaling = True
TriggerTool.StoreMatchingInfo = False

SetupSUSYJetSelector().bJetEtaCut = 2.5

if 'Stop0L_2LCR' in CutFlowsToRun or 'Stop0L_1Y' in CutFlowsToRun or len(CutFlowsToRun)==0:
    if "doGamma" in globals():
        TriggerTool.SingleLeptonTriggers = SingleLepTriggers+PhotonTriggers
    else:
        TriggerTool.SingleLeptonTriggers = SingleLepTriggers
    SetupSUSYElectonSelector().SFTrigger = ["e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose"]
    SetupSUSYMuonSelector().SFTrigger2015 = ["HLT_mu20_iloose_L1MU15_OR_HLT_mu50"]
    SetupSUSYMuonSelector().SFTrigger2016 = ["HLT_mu26_ivarmedium_OR_HLT_mu50"]

SetupAlgorithm()

