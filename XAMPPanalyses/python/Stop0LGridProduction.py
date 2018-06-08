import os
import commands
import subprocess
import math
import sys
import argparse
from datetime import datetime
import re

# this script can be used for grid productions of the stop -> 0L analysis
# it calls the SumbmitToGrid.py script which does the actual submit

##############################
# adjust your settings here:
##############################

ProductionTagNumber = "3" # number of production this month
doHistos = False
doTrees = True
doxAODs = False

AnalysisName = "Stop0L"


# some datasets (e.g. sliced in Pt) should run in 1 job in order to get an merged output DS (e.g. Pt integrated)
# naming patterns of those DS are listed here
DSpatternsForMergedJobs = [
                "Sherpa_CT10_Wenu_Pt", "Sherpa_CT10_Wmunu_Pt", "Sherpa_CT10_Wtaunu_Pt",
                "Sherpa_CT10_Zee_Pt", "Sherpa_CT10_Zmumu_Pt", "Sherpa_CT10_Ztautau_Pt", "Sherpa_CT10_Znunu_Pt",
                "A14NNPDF23LO_ttW_Np", "A14NNPDF23LO_ttZnnqq_Np", "Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ",
                "Sherpa_CT10_SinglePhotonPt", "Sherpa_CT10_eegammaPt" , "Sherpa_CT10_mumugammaPt" , "Sherpa_CT10_enugammaPt" , "Sherpa_CT10_munugammaPt" , 
                "Sherpa_CT10_taunugammaPt" , "Sherpa_CT10_tautaugammaPt" , "Sherpa_CT10_nunugammaPt",
                "Sherpa_NNPDF30NNLO_Wenu_Pt", "Sherpa_NNPDF30NNLO_Wmunu_Pt", "Sherpa_NNPDF30NNLO_Wtaunu_Pt", 
                "Sherpa_NNPDF30NNLO_Zee_Pt", "Sherpa_NNPDF30NNLO_Zmumu_Pt",  "Sherpa_NNPDF30NNLO_Ztautau_Pt", "Sherpa_NNPDF30NNLO_Znunu_Pt",
                "Sherpa_221_NNPDF30NNLO_Wenu_MAXHTPTV", "Sherpa_221_NNPDF30NNLO_Wmunu_MAXHTPTV", "Sherpa_221_NNPDF30NNLO_Wtaunu_MAXHTPTV", 
                "Sherpa_221_NNPDF30NNLO_Zee_MAXHTPTV", "Sherpa_221_NNPDF30NNLO_Zmumu_MAXHTPTV", "Sherpa_221_NNPDF30NNLO_Ztautau_MAXHTPTV", "Sherpa_221_NNPDF30NNLO_Znunu_MAXHTPTV",
                        ]
# the merged input DS are still separated by the following subpatterns (if available):
DSsubpatternsForMergedJobs = ["CVetoBVeto", "CFilterBVeto", "BFilter"]

# in case a pattern in the DS name contains specific information (e.g. particle masses), find it and add it to the PhysicsShortName
patternsForMasses = ["TT_directTT_",
                     "TT_onestepBB_",
                     "TT_mixedBT_",
                     "TT_bWN_",
                     "GG_directGtc5_",
                     "DM_TTscalar_p",
                     "DM_TTpseudo_p",
                     "DM_BBscalar_p",
                     "DM_BBpseudo_p"
                    ]

##############################
##############################

PhysicsShortNames = {
    # ttbar
    "PowhegPythiaEvtGen_P2012CT10_ttbarMET200_hdamp172p5_nonAH": "ttbarPowhegNonAHMET200",
    "PowhegPythiaEvtGen_P2012CT10_ttbarMET300_hdamp172p5_nonAH": "ttbarPowhegNonAHMET300",
    "PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad":   "ttbarPowhegNonAH",
    "PowhegPythia8EvtGen_A14_ttbar_hdamp172p5_nonallhad":  "ttbarPowheg8NonAH",
    "Sherpa_221_NNPDF30NNLO_ttbar_AllHadronic_MEPS_NLO": "ttbarSherpaAllHad",
    "Sherpa_221_NNPDF30NNLO_ttbar_SingleLeptonP_MEPS_NLO": "ttbarSherpaSingleLeptonP",
    "Sherpa_221_NNPDF30NNLO_ttbar_SingleLeptonM_MEPS_NLO": "ttbarSherpaSingleLeptonM",
    "Sherpa_221_NNPDF30NNLO_ttbar_dilepton_MEPS_NLO": "ttbarSherpaDilep",
    # ttbar systematic variations
    "PhPyEG_CT10_P2012radHiC6L1_ttbarMET200_hdamp345_down_nonAH": "ttbarPhPyradHiMET200",
    "PowhegPythiaEvtGen_P2012radHi_ttbar_hdamp345_down_nonallhad": "ttbarPhPyradHi",
    "PhPyEG_CT10_P2012radLoC6L1_ttbarMET200_hdamp172_up_nonAH": "ttbarPhPyradLoMET200",
    "PowhegPythiaEvtGen_P2012radLo_ttbar_hdamp172_up_nonallhad": "ttbarPhPyradLo",
    "aMcAtNloPythia8EvtGen_MEN30NLO_A14N23LO_ttbar_nonallhad": "ttbaraMcAtNlo",
    "PowhegHerwigppEvtGen_UEEE5_ttbar_hdamp172p5_nonallhad": "ttbarPhHerwigpp",
    "PhHppEG_CT10_UE5C6L1_ttbarMET200_hdamp172p5_nonAH": "ttbarPhHerwigppMET200",
    # single top
    "PowhegPythiaEvtGen_P2012CT10_Wt_inclusive_top_MET200":   "WtPowhegMET200",
    "PowhegPythiaEvtGen_P2012CT10_Wt_inclusive_tbar_MET200":   "WtbarPowhegMET200",
    "PowhegPythiaEvtGen_P2012_singletop_tchan_lept_top":   "singleTPowheg_tChan",
    "PowhegPythiaEvtGen_P2012_singletop_tchan_lept_antitop":   "singleTbarPowheg_tChan",
    "PowhegPythiaEvtGen_P2012_Wt_inclusive_top":   "WtPowheg",
    "PowhegPythiaEvtGen_P2012_Wt_inclusive_antitop":   "WtbarPowheg",
    "PowhegPythiaEvtGen_P2012_SingleTopSchan_noAllHad_top":   "singleTPowheg_sChanNonAH",
    "PowhegPythiaEvtGen_P2012_SingleTopSchan_noAllHad_antitop":   "singleTbarPowheg_sChanNonAH",
    # single top systematic variations
    "PowhegHerwigppEvtGen_UEEE5_Wt_inclusive_top" : "WtHerwigpp",
    "PowhegHerwigppEvtGen_UEEE5_Wt_inclusive_antitop" : "WtbarHerwigpp",
    "PowhegPythiaEvtGen_P2012_Wt_DS_inclusive_top" : "WtDSt",
    "PowhegPythiaEvtGen_P2012_Wt_DS_inclusive_antitop" : "WtbarDS",
    "PowhegPythiaEvtGen_P2012radLo_singletop_tchan_lept_top" : "singleT_tChan_radL",
    "PowhegPythiaEvtGen_P2012radHi_singletop_tchan_lept_top" : "singleT_tChan_radH",
    "PowhegPythiaEvtGen_P2012radLo_singletop_tchan_lept_antitop" : "singleTbar_tChan_radL",
    "PowhegPythiaEvtGen_P2012radHi_singletop_tchan_lept_antitop" : "singleTbar_tChan_radH",
    "PowhegPythiaEvtGen_P2012radHi_Wt_inclusive_top" : "Wt_radH",
    "PowhegPythiaEvtGen_P2012radLo_Wt_inclusive_top" : "Wt_radL",
    "PowhegPythiaEvtGen_P2012radHi_Wt_inclusive_antitop" : "Wtbar_radH",
    "PowhegPythiaEvtGen_P2012radLo_Wt_inclusive_antitop" : "Wtbar_radL",
    "PowhegPythiaEvtGen_P2012radHi_SingleTopSchan_lept_top" : "singleT_sChan_radH",
    "PowhegPythiaEvtGen_P2012radLo_SingleTopSchan_lept_top" : "singleT_sChan_radL",
    "PowhegPythiaEvtGen_P2012radHi_SingleTopSchan_lept_antitop" : "singleTbar_sChan_radH",
    "PowhegPythiaEvtGen_P2012radLo_SingleTopSchan_lept_antitop" : "singleTbar_sChan_radL",

    # ttV
    "MadGraphPythia8EvtGen_A14NNPDF23LO_ttW_Np0":   "ttWMadGraph",
    "MadGraphPythia8EvtGen_A14NNPDF23LO_ttW_Np1":   "ttWMadGraph",
    "MadGraphPythia8EvtGen_A14NNPDF23LO_ttW_Np2":   "ttWMadGraph",
    "MadGraphPythia8EvtGen_A14NNPDF23LO_ttZnnqq_Np0":   "ttZnnqqMadGraph",
    "MadGraphPythia8EvtGen_A14NNPDF23LO_ttZnnqq_Np1":   "ttZnnqqMadGraph",
    "MadGraphPythia8EvtGen_A14NNPDF23LO_ttZnnqq_Np2":   "ttZnnqqMadGraph",
    # ttgamma
    "MadGraphPythia8EvtGen_A14NNPDF23LO_ttgamma_noallhad":   "ttgammaMadGraphNonAH",
    "MadGraphPythia8EvtGen_A14NNPDF23LO_ttgamma4080_noallhad":   "ttgammaMadGraphNonAH4080",
    "MadGraphPythia8EvtGen_A14NNPDF23LO_ttgamma80_noallhad":   "ttgammaMadGraphNonAH80",
    "MadGraphPythia8EvtGen_A14NNPDF23LO_ttgamma_allhad":   "ttgammaMadGraphAH",
    "MadGraphPythia8EvtGen_A14NNPDF23LO_ttgamma4080_allhad":   "ttgammaMadGraphAH4080",
    "MadGraphPythia8EvtGen_A14NNPDF23LO_ttgamma80_allhad":  "ttgammaMadGraphAH80",
    # W+jets
    "Sherpa_CT10_Wenu_Pt":   "WenuSherpa",
    "Sherpa_CT10_Wmunu_Pt":   "WmunuSherpa",
    "Sherpa_CT10_Wtaunu_Pt":   "WtaunuSherpa",
    # W+jets (Sherpa 2.2)
    "Sherpa_NNPDF30NNLO_Wenu_Pt":   "WenuSherpa2",
    "Sherpa_NNPDF30NNLO_Wmunu_Pt":   "WmunuSherpa2",
    "Sherpa_NNPDF30NNLO_Wtaunu_Pt":   "WtaunuSherpa2",
    # Z+jets
    "Sherpa_CT10_Zee_Pt":   "ZeeSherpa",
    "Sherpa_CT10_Zmumu_Pt":   "ZmumuSherpa",
    "Sherpa_CT10_Ztautau_Pt":   "ZtautauSherpa",
    "Sherpa_CT10_Znunu_Pt":   "ZnunuSherpa",
    # Z+jets (Sherpa 2.2)
    "Sherpa_NNPDF30NNLO_Zee_Pt":   "ZeeSherpa2",
    "Sherpa_NNPDF30NNLO_Zmumu_Pt":   "ZmumuSherpa2",
    "Sherpa_NNPDF30NNLO_Ztautau_Pt":   "ZtautauSherpa2",
    "Sherpa_NNPDF30NNLO_Znunu_Pt":   "ZnunuSherpa2",
    # W+jets (Sherpa 2.2.1)
    "Sherpa_221_NNPDF30NNLO_Wenu_MAXHTPTV":   "WenuSherpa21",
    "Sherpa_221_NNPDF30NNLO_Wmunu_MAXHTPTV":   "WmunuSherpa21",
    "Sherpa_221_NNPDF30NNLO_Wtaunu_MAXHTPTV":   "WtaunuSherpa21",
    # Z+jets (Sherpa 2.2.1)
    "Sherpa_221_NNPDF30NNLO_Zee_MAXHTPTV":   "ZeeSherpa21",
    "Sherpa_221_NNPDF30NNLO_Zmumu_MAXHTPTV":   "ZmumuSherpa21",
    "Sherpa_221_NNPDF30NNLO_Ztautau_MAXHTPTV":   "ZtautauSherpa21",
    "Sherpa_221_NNPDF30NNLO_Znunu_MAXHTPTV":   "ZnunuSherpa21",
    # dibosons
    "Sherpa_CT10_llll":   "DibosonllllSherpa",
    "Sherpa_CT10_lvvv":   "DibosonlvvvSherpa",
    "Sherpa_CT10_vvvv":   "DibosonvvvvSherpa",
    "Sherpa_CT10_lllvSFMinus":   "DibosonlllvSherpa",
    "Sherpa_CT10_lllvOFMinus":   "DibosonlllvSherpa",
    "Sherpa_CT10_lllvSFPlus":   "DibosonlllvSherpa",
    "Sherpa_CT10_lllvOFPlus":   "DibosonlllvSherpa",
    "Sherpa_CT10_llvv":   "DibosonllvvSherpa",
    "Sherpa_CT10_WplvWmqq":   "DibosonWplvWmqqSherpa",
    "Sherpa_CT10_WpqqWmlv":   "DibosonWpqqWmlvSherpa",
    "Sherpa_CT10_WlvZqq":   "DibosonWlvZqqSherpa",
    "Sherpa_CT10_WqqZll":   "DibosonWqqZllSherpa",
    "Sherpa_CT10_WqqZvv":   "DibosonWqqZvvSherpa",
    "Sherpa_CT10_ZqqZll":  "DibosonZqqZllSherpa",
    "Sherpa_CT10_ZqqZvv":   "DibosonZqqZvvSherpa",
# SHERPA 2.1 (new prescription)
"Sherpa_CT10_llvvjj_ss_EW4":  "DibosonNewSherpa21llvvjj_ss_EW4",
"Sherpa_CT10_llvvjj_ss_EW6":  "DibosonNewSherpa21llvvjj_ss_EW6",
"Sherpa_CT10_lllvjj_EW6":  "DibosonNewSherpa21lllvjj_EW6",
"Sherpa_CT10_lllljj_EW6":  "DibosonNewSherpa21lllljj_EW6",
"Sherpa_CT10_ggllll":  "DibosonNewSherpa21ggllll",
"Sherpa_CT10_ggllvv":  "DibosonNewSherpa21ggllvv",
# SHERPA 2.2.1 (new prescription)
"Sherpa_221_NNPDF30NNLO_WlvZqq":  "DibosonNewSherpa221WlvZqq",
"Sherpa_221_NNPDF30NNLO_llll":  "DibosonNewSherpa221llll",
"Sherpa_221_NNPDF30NNLO_lllv":  "DibosonNewSherpa221lllv",
"Sherpa_221_NNPDF30NNLO_llvv":  "DibosonNewSherpa221llvv",
"Sherpa_221_NNPDF30NNLO_lvvv":  "DibosonNewSherpa221lvvv",
"Sherpa_221_NNPDF30NNLO_vvvv":  "DibosonNewSherpa221vvvv",
"Sherpa_221_NNPDF30NNLO_ZqqZvv":  "DibosonNewSherpa221ZqqZvv",
"Sherpa_221_NNPDF30NNLO_ZqqZll":  "DibosonNewSherpa221ZqqZll",
"Sherpa_221_NNPDF30NNLO_WqqZvv":  "DibosonNewSherpa221WqqZvv",
"Sherpa_221_NNPDF30NNLO_WqqZll":  "DibosonNewSherpa221WqqZll",
"Sherpa_221_NNPDF30NNLO_WpqqWmlv":  "DibosonNewSherpa221WpqqWmlv",
"Sherpa_221_NNPDF30NNLO_WplvWmqq":  "DibosonNewSherpa221WplvWmqq",
    # signal TN1
    "MadGraphPythia8EvtGen_A14NNPDF23LO_TT_directTT_":   "TT_directTTMadGraph",
    "MGPy8EG_A14N23LO_TT_directTT_" : "ISR_TT_directTT",
    # signal bC1
    "MadGraphPythia8EvtGen_A14NNPDF23LO_TT_onestepBB_":   "TT_onestepBBMadGraph",
    # signal mixed
    "MGPy8EG_A14N23LO_TT_mixedBT_":   "TT_mixedBTMGPy8EG",
    # signal bWN
    "MGPy8EG_A14N23LO_TT_bWN_" : "TT_bWNMGPy8EG",
    # signal GG
    "MadGraphPythia8EvtGen_A14NNPDF23LO_GG_directGtc5_" : "GG_directGtc5MadGraph",
    

    # DM signal
    "MGPy8EG_N30LO_A14N23LO_DM_TTscalar_p" : "DM_TTscalar",
    "MGPy8EG_N30LO_A14N23LO_DM_TTpseudo_p" : "DM_TTpseudo",
    "MGPy8EG_N30LO_A14N23LO_DM_BBscalar_p" : "DM_BBscalar",
    "MGPy8EG_N30LO_A14N23LO_DM_BBpseudo_p" : "DM_BBpseudo",
    
    # ttV
    "aMcAtNloPythia8EvtGen_MEN30NLO_A14N23LO_ttW" : "aMcAtNlo_ttW",
    "aMcAtNloPythia8EvtGen_MEN30NLO_A14N23LO_ttZnunu" : "aMcAtNlo_ttZnunu",
    "aMcAtNloPythia8EvtGen_MEN30NLO_A14N23LO_ttZqq" : "aMcAtNlo_ttZqq", 
    "aMcAtNloPythia8EvtGen_MEN30NLO_A14N23LO_ttee" : "aMcAtNlo_ttee",
    "aMcAtNloPythia8EvtGen_MEN30NLO_A14N23LO_ttmumu" : "aMcAtNlo_ttmumu",
    "aMcAtNloPythia8EvtGen_MEN30NLO_A14N23LO_tttautau" : "aMcAtNlo_tttautau", 
    
    # tt+gamma
    "MadGraphPythia8EvtGen_A14NNPDF23LO_ttgamma": "ttgammaMadGraph",
    "aMcAtNloPythia8EvtGen_MEN30NLO_A14N23LO_tta140" : "aMcAtNlo_ttgamma",
    
    # single photon
    "Sherpa_CT10_SinglePhotonPt": "SingleYSherpa",
    
    # V+photon
    "Sherpa_CT10_eegammaPt" : "eegammaSherpa",
    "Sherpa_CT10_mumugammaPt" : "mumugammaSherpa",
    "Sherpa_CT10_enugammaPt" : "enugammaSherpa",
    "Sherpa_CT10_munugammaPt" : "munugammaSherpa",
    "Sherpa_CT10_taunugammaPt" : "taunugammaSherpa",
    "Sherpa_CT10_tautaugammaPt" : "tautaugammaSherpa",
    "Sherpa_CT10_nunugammaPt" : "nunugammaSherpa",
    
    }

TODAY = commands.getoutput("date --iso")

MonthLetters = {
    1 :   "A",
    2 :   "B",
    3 :   "C",
    4 :   "D",
    5 :   "E",
    6 :   "F",
    7 :   "G",
    8 :   "H",
    9 :   "I",
    10 :   "J",
    11 :   "K",
    12 :   "L" }

now = datetime.now()
ROOTCOREBIN = os.getenv("ROOTCOREBIN")

isData = False
isAF2 = False
DSID = '012345'
PhysicsShortName = "ttbar"
TAGS = 'e3836_s2608_s2183_r6869_r6282_p2419'

def ReadListFromFile (File):
    List = []
    if os.path.isfile(File) == True:
        for Line in open(File):
            Line = Line.strip().replace("\n", "")
            if Line ==  "": continue
            if Line.startswith("#"): continue
            List.append(Line.replace("\n", ""))
    else:
        print "Could not find ListFile " + str(File)
    return List

def DateIntoShortname(now):
    return now.strftime('%y') + MonthLetters[now.month]

def AnalyseDSName(name, patternsForMasses=[]):
    
    # in case a pattern in the DS name contains specific information (e.g. particle masses), find it and add it to the PhysicsShortName
    DSMasses = []
    shortnamesuffix = ''
    if len(patternsForMasses) > 0:
        for pattern in patternsForMasses:
            if pattern in name:
                # DM samples need special treatment due to '_c' in mass pattern
                if 'DM_TTscalar_p' in pattern or 'DM_TTpseudo_p' in pattern or 'DM_BBscalar_p' in pattern or 'DM_BBpseudo_p' in pattern:
                    DSMasses.append(re.findall("%s(\d+(_c\d+)*)" % pattern, name))
                else:
                    DSMasses.append(re.findall("%s(\d+(_\d+)*)" % pattern, name))
    if len(DSMasses) > 2:
        print "Error: found 2 possible matches between DS name and possible mass ranges in DS: %s" % name
        exit(1)
    elif len(DSMasses) == 1:
        if len(DSMasses[0]) > 0:
            shortnamesuffix = '_' + DSMasses[0][0][0]  
    
    if 'data15_13TeV' in name:
        isData = True
        isAF2 = False
        doSyst = False
        PhysicsShortName = 'Data15'
    elif 'data16_13TeV' in name:
        isData = True
        isAF2 = False
        doSyst = False
        PhysicsShortName = 'Data16'
    elif 'mc15_13TeV' in name:
        isData = False
        doSyst = True
        foundShortName = False
        for fullname, shortname in PhysicsShortNames.iteritems():
            if fullname in name.split(".")[2]:
                PhysicsShortName = shortname + shortnamesuffix
                foundShortName = True
        if not foundShortName:
            PhysicsShortName = name.split(".")[2]
    DSID = name.split(".")[1]
    TAGS = name.split(".")[-1]
    DAOD = "SUSY" + name.split("DAOD_SUSY")[1].split(".")[0]
    if 'a' in TAGS and not isData:
        isAF2 = True
    else:
        isAF2 = False
    if 'r6869_r6282' in TAGS and not isData:
        isMC15a = True
    else:
        isMC15a = False
        
    return [isData, isAF2, isMC15a, DSID, PhysicsShortName, TAGS, DAOD]

def CallFinalSubmitScript(config, inputDS, outputDSname):
    print 'Calling final grid submit script...'
    
    JobOptions = '--inputDS %s' % (inputDS)
    if not doHistos:
        JobOptions += ' --noHistos'
    if not doTrees:
        JobOptions += ' --noTrees'
    if not doxAODs:
        JobOptions += ' --noxAODs'
    if config[0]:
        JobOptions += ' --isData'
    if config[1]:
        JobOptions += ' --isAF2'
    SystematicSamples = ['mc15_13TeV.407032','mc15_13TeV.410001','mc15_13TeV.407036','mc15_13TeV.410002','mc15_13TeV.410225','mc15_13TeV.410004','mc15_13TeV.407040','mc15_13TeV.410147','mc15_13TeV.410148',"mc15_13TeV.410062.", "mc15_13TeV.410063.", "mc15_13TeV.410017.", "mc15_13TeV.410018.", "mc15_13TeV.410019.", "mc15_13TeV.410020.", "mc15_13TeV.410099.", "mc15_13TeV.410100.", "mc15_13TeV.410101.", "mc15_13TeV.410102.", "mc15_13TeV.410107.", "mc15_13TeV.410108.", "mc15_13TeV.410109.", "mc15_13TeV.410110."]
    for smp in SystematicSamples:
        if smp in inputDS:
            JobOptions += ' --noSyst'

    if RunOptions.runGammaCR:
        Command = "python XAMPPanalyses/python/Stop0LSubmitToGridGamma.py %s --outDS %s" % (JobOptions, outputDSname)
    else:
        Command = "python XAMPPanalyses/python/Stop0LSubmitToGrid.py -a Stop0L --doISR %s --outDS %s" % (JobOptions, outputDSname) # --DuplicateTask
    # print Command
    os.system(Command)

if __name__ == '__main__':
        
    tstart = datetime.now()
    print str(tstart) + ' : Starting production grid submit...'
        
    parser = argparse.ArgumentParser(description='This script submits the stop-0L analysis code to the grid. For more help type \"python XAMPPbase/python/Stop0LGridProduction.py -h\"', prog='Stop0LGridProduction', formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('-i', '--inputDS', help='input dataset or DS list present in \"XAMPPbase/data/SampleLists/mc15c_13TeV/\"', required=True, nargs='+')
    parser.add_argument('--DSCampaign', help='choose campaign of DS production / filepath to the DS list (default is \"mc15c_13TeV\")', default='mc15c_13TeV')
    parser.add_argument('--runGammaCR', help='If you switch this option on you are submitting the gamma CR setup (runStop0LepGamma)', action='store_true', default=False)
    parser.add_argument('--doNotMergeDS', help='Switch off merging of datasets in jobs', action='store_true', default=False)

    RunOptions = parser.parse_args()

    InDSList = RunOptions.inputDS
    Campaign = RunOptions.DSCampaign
                  
    InputDataSets = []
    
    if RunOptions.runGammaCR:
        AnalysisName += "Gamma"

    # Read in the InputDataSets
    # try to find file first at default directory:
    InDSListDir = "%s/data/XAMPPbase/SampleLists/%s/" % (ROOTCOREBIN, Campaign)
    if not os.path.exists(InDSListDir):
        if not os.path.exists(Campaign):
            print "ERROR: Campaign %s is invalid. Could not find the directory containing the sample lists."%Campaign
            exit(1)
        else:
            InDSListDir = Campaign+'/'
    # check if inputDS was a file
    if len(InDSList)>0 and os.path.exists(InDSListDir + InDSList[0]):
        InputDataSets.extend(ReadListFromFile(InDSListDir + InDSList[0]))
    # if not, it should be a DS or a comma-separated list of DS
    else:
        InputDataSets = RunOptions.inputDS
        print 'Directly submitting dataset(s) %s'%InputDataSets
            
    if len(InputDataSets) == 0:
        prettyPrint('ERROR', 'No input dataset found')
        exit(1)
        
        
    if RunOptions.doNotMergeDS:
        DSpatternsForMergedJobs = []
    # some datasets (e.g. sliced in Pt) should run in 1 job in order to get an merged output DS (e.g. Pt integrated)
    DSForMergedJobs = {}
    for pattern in DSpatternsForMergedJobs:
        for DS in InputDataSets:
            # check, if DS name also contains one of the subpatterns
            doSubpattern = False
            for subpattern in DSsubpatternsForMergedJobs:
                if subpattern in DS:
                    doSubpattern = True
            if doSubpattern:
                for subpattern in DSsubpatternsForMergedJobs:
                    if pattern in DS and subpattern in DS:
                        keyname = pattern.replace('_MAXHTPTV', '').replace('_Pt', '') + '_' + subpattern
                        try:
                            if len(DSForMergedJobs[keyname]) == 0:
                                print 'blah'
                        except KeyError:
                            DSForMergedJobs[keyname] = []
                        DSForMergedJobs[keyname].append(DS)
            else:
                if pattern in DS:
                    keyname = pattern.replace('_Np', '')
                    try:
                        if len(DSForMergedJobs[keyname]) == 0:
                            print 'blah'
                    except KeyError:
                        DSForMergedJobs[keyname] = []
                    DSForMergedJobs[keyname].append(DS)
        
    if len(DSForMergedJobs) != 0:
        for mergedPattern, listToSubmit in DSForMergedJobs.iteritems():
            if len(listToSubmit) == 0: continue
            inDS = ''
            for DS in listToSubmit:
                DS = DS.strip().replace("\n","")
                inDS += '%s,' % DS
                # remove DS from full list for not submitting it separately too
                try:
                    InputDataSets.remove(DS)
                except ValueError:
                    try:
                        InputDataSets.remove(DS.replace("/",""))
                    except ValueError:
                        try:
                            InputDataSets.remove(DS+"/")
                        except ValueError:
                            print 'ERROR: Cannot remove dataset %s since it is not in the input list %s!'%(DS,InputDataSets)
                            sys.exit(1)
            # erase comma in the end again
            if inDS.endswith(','):
                inDS = inDS[:-1]
                
            config = AnalyseDSName(listToSubmit[0], patternsForMasses)
            mergedPattern = mergedPattern.replace('Pt', '').replace('_Np', '')
                    
            # now the name of the output dataset does NOT contain the DSID anymore, only the merged pattern
            outDSName = ".".join([DateIntoShortname(now) + ProductionTagNumber, AnalysisName, mergedPattern, config[6], config[5]])
                
            CallFinalSubmitScript(config, inDS, outDSName)
          
    for DS in InputDataSets:
        config = AnalyseDSName(DS, patternsForMasses)
        outDSName = ".".join([DateIntoShortname(now) + ProductionTagNumber, AnalysisName, config[3], config[4], config[6], config[5]])
        CallFinalSubmitScript(config, DS, outDSName)
        
    print '\nDONE: All jobs submitted!'
        
