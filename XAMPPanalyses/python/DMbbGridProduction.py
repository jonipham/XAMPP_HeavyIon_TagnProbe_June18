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

ProductionTagNumber = "1" # number of production this month
doTruth = False # this only affects MC, if turned on (ttbar and signal is turned on by default)
doHistos = False
doTrees = True
doxAODs = False
doSyst = False

AnalysisName = "DMbb"


# some datasets (e.g. sliced in Pt) should run in 1 job in order to get an merged output DS (e.g. Pt integrated)
# naming patterns of those DS are listed here
DSpatternsForMergedJobs = ["CT10_SinglePhotonPt",
                "Sherpa_CT10_Wenu_Pt", "Sherpa_CT10_Wmunu_Pt", "Sherpa_CT10_Wtaunu_Pt",
                "Sherpa_CT10_Zee_Pt", "Sherpa_CT10_Zmumu_Pt", "Sherpa_CT10_Ztautau_Pt", "Sherpa_CT10_Znunu_Pt",
                "A14NNPDF23LO_ttW_Np", "A14NNPDF23LO_ttZnnqq_Np", "Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ",
                "Sherpa_NNPDF30NNLO_Wenu_Pt", "Sherpa_NNPDF30NNLO_Wmunu_Pt", "Sherpa_NNPDF30NNLO_Wtaunu_Pt", 
                "Sherpa_NNPDF30NNLO_Zee_Pt", "Sherpa_NNPDF30NNLO_Zmumu_Pt",  "Sherpa_NNPDF30NNLO_Ztautau_Pt", "Sherpa_NNPDF30NNLO_Znunu_Pt"
                        ]
# the merged input DS are still separated by the following subpatterns (if available):
DSsubpatternsForMergedJobs = ["CVetoBVeto", "CFilterBVeto", "BFilter"]

# in case a pattern in the DS name contains specific information (e.g. particle masses), find it and add it to the PhysicsShortName
patternsForMasses = ["TT_directTT_",
                     "TT_onestepBB_",
                     "TT_mixedBT_",
                     "DM_TTscalar_p",
                     "DM_BBscalar_p",
                    ]

##############################
##############################

PhysicsShortNames = {
    # ttbar
    "PowhegPythiaEvtGen_P2012CT10_ttbarMET200_hdamp172p5_nonAH": "ttbarPowhegNonAHMET200",
    "PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad":   "ttbarPowhegNonAH",
    "PowhegPythia8EvtGen_A14_ttbar_hdamp172p5_nonallhad":  "ttbarPowheg8NonAH",
    # single top
    "PowhegPythiaEvtGen_P2012CT10_Wt_inclusive_top_MET200":   "WtPowhegMET200",
    "PowhegPythiaEvtGen_P2012CT10_Wt_inclusive_tbar_MET200":   "WtbarPowhegMET200",
    "PowhegPythiaEvtGen_P2012_singletop_tchan_lept_top":   "singleTPowheg_tChan",
    "PowhegPythiaEvtGen_P2012_singletop_tchan_lept_antitop":   "singleTbarPowheg_tChan",
    "PowhegPythiaEvtGen_P2012_Wt_inclusive_top":   "WtPowheg",
    "PowhegPythiaEvtGen_P2012_Wt_inclusive_antitop":   "WtbarPowheg",
    "PowhegPythiaEvtGen_P2012_SingleTopSchan_noAllHad_top":   "singleTPowheg_sChanNonAH",
    "PowhegPythiaEvtGen_P2012_SingleTopSchan_noAllHad_antitop":   "singleTbarPowheg_sChanNonAH",
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
    # photon+X
    "Sherpa_CT10_SinglePhotonPt":   "SinglePhotSherpa",
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
    # dibosons
    "Sherpa_CT10_llll":   "DibosonllllSherpa",
    "Sherpa_CT10_lvvv":   "DibosonlvvvSherpa",
    "Sherpa_CT10_vvvv":   "DibosonvvvvSherpa",
    "Sherpa_CT10_lllvSFMinus":   "DibosonlllvSherpa",
    "Sherpa_CT10_lllvOFMinus":   "DibosonlllvSherpa",
    "Sherpa_CT10_lllvSFPlus":   "DibosonlllvSherpa",
    "Sherpa_CT10_lllvOFPlus":   "DibosonlllvSherpa",
    "Sherpa_CT10_llvv":   "DibosonllvvSherpa",
    "Sherpa_CT10_ggllvv":   "DibosonggllvvSherpa",
    "Sherpa_CT10_WplvWmqq":   "DibosonWplvWmqqSherpa",
    "Sherpa_CT10_WpqqWmlv":   "DibosonWpqqWmlvSherpa",
    "Sherpa_CT10_WlvZqq":   "DibosonWlvZqqSherpa",
    "Sherpa_CT10_WqqZll":   "DibosonWqqZllSherpa",
    "Sherpa_CT10_WqqZvv":   "DibosonWqqZvvSherpa",
    "Sherpa_CT10_ZqqZll":  "DibosonZqqZllSherpa",
    "Sherpa_CT10_ZqqZvv":   "DibosonZqqZvvSherpa",
    "Sherpa_CT10_llvvjj_ss_EW4":   "Dibosonllvvjj_ss_EW4Sherpa",                            
    "Sherpa_CT10_llvvjj_ss_EW6":   "Dibosonllvvjj_ss_EW6Sherpa",                            
    "Sherpa_CT10_lllvjj_EW6":   "Dibosonlllvjj_EW6Sherpa",                               
    "Sherpa_CT10_lllljj_EW6":   "Dibosonlllljj_EW6Sherpa",
    "Sherpa_CT10_ggllll":  "DibosonggllllSherpa",   
    # signal TN1
    "MadGraphPythia8EvtGen_A14NNPDF23LO_TT_directTT_":   "TT_directTTMadGraph",
    # signal bC1
    "MadGraphPythia8EvtGen_A14NNPDF23LO_TT_onestepBB_":   "TT_onestepBBMadGraph",
    # signal mixed
    "MGPy8EG_A14N23LO_TT_mixedBT_":   "TT_mixedBTMGPy8EG",
    
    "MGPy8EG_A14N23LO_TT_directTT_" : "ISR_TT_directTT",

    # DM signal
    "MGPy8EG_N30LO_A14N23LO_DM_TTscalar_p" : "DM_TTscalar",
    "MGPy8EG_N30LO_A14N23LO_DM_BBscalar_p" : "DM_BBscalar",
    
    # ttV
    "aMcAtNloPythia8EvtGen_MEN30NLO_A14N23LO_ttW" : "aMcAtNlo_ttW",
    "aMcAtNloPythia8EvtGen_MEN30NLO_A14N23LO_ttZnunu" : "aMcAtNlo_ttZnunu",
    "aMcAtNloPythia8EvtGen_MEN30NLO_A14N23LO_ttZqq" : "aMcAtNlo_ttZqq", 
    
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

def ReadListFromFile (File , Campaign="mc15_13TeV"):
    List = []
    if os.path.isfile(File) == True:
        for Line in open(File):
            if Line[0] is not "#":
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
                if 'DM_TTscalar_p' in pattern or 'DM_BBscalar_p' in pattern:
                    DSMasses.append(re.findall("%s(\d+(_c\d+)*)" % pattern, name))
                else:
                    DSMasses.append(re.findall("%s(\d+(_\d+)*)" % pattern, name))
    if len(DSMasses) > 2:
        print "Error: found 2 possible matches between DS name and possible mass ranges in DS: %s" % name
        exit(1)
    elif len(DSMasses) == 1:
        if len(DSMasses[0]) > 0:
            shortnamesuffix = '_' + DSMasses[0][0][0]

    
    m_doSyst = True
    if 'data15_13TeV' in name:
        isData = True
        isAF2 = False
        m_doSyst = False
        PhysicsShortName = 'Data15'
    elif 'data16_13TeV' in name:
        isData = True
        isAF2 = False
        m_doSyst = False
        PhysicsShortName = 'Data16'
    elif 'mc15_13TeV' in name:
        isData = False
        if doSyst:
            m_doSyst = True
        else: 
            m_doSyst = False
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

    return [isData, isAF2, isMC15a, DSID, PhysicsShortName, TAGS, DAOD, m_doSyst]

def CallFinalSubmitScript(config, inputDS, outputDSname):
    
    JobOptions = '-i %s -a %s' % (inputDS, AnalysisName)
#    if not doTruth:
#        if not "ttbar" in config[4] and not "TT_" in config[4]:
#            JobOptions += ' --noTruth'
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
    if config[2]:
        JobOptions += ' --isMC15a'
    if not config[7]:
        JobOptions += ' --noSyst'

    Command = "python XAMPPanalyses/python/DMbbSubmitToGrid.py %s --outDS %s" % (JobOptions, outputDSname)
#     print Command
    os.system(Command)

if __name__ == '__main__':
        
    tstart = datetime.now()
    print str(tstart) + ' : Starting production grid submit...'
        
    parser = argparse.ArgumentParser(description='This script submits the stop-0L analysis code to the grid. For more help type \"python XAMPPbase/python/SubmitToGrid.py -h\"', prog='runStop0Lep')
    parser.add_argument('-i', '--inputDS', help='input dataset or DS list present in \"XAMPPbase/data/SampleLists/mc15c_13TeV/\"', required=True, nargs='+')
    parser.add_argument('--DSCampaign', help='choose campaign of DS production in order to adjust the filepath to the DS list (default is \"mc15c_13TeV\")', default='mc15c_13TeV')

        
    RunOptions = parser.parse_args()

    InDSList = RunOptions.inputDS
    Campaign = RunOptions.DSCampaign
                  
    InputDataSets = []

    # Read in the InputDataSets
    # try to find file first at default directory:
    InDSListDir = "%s/data/XAMPPbase/SampleLists/%s/" % (ROOTCOREBIN, Campaign)
    if os.path.exists(InDSListDir) == False:
        print "ERROR: Campaign is invalid. Could not find the directory containing the sample lists."
        exit(1)
    # check if inputDS was a file
    if len(InDSList)>0 and os.path.exists(InDSListDir + InDSList[0]):
        InputDataSets.extend(ReadListFromFile(InDSListDir + InDSList[0], Campaign))
    # if not, it should be a DS or a comma-separated list of DS
    else:
        InputDataSets = RunOptions.inputDS
        print 'Directly submitting dataset(s) %s'%InputDataSets
            
    if len(InputDataSets) == 0:
        prettyPrint('ERROR', 'No input dataset found')
        exit(1)
        
        
        
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
                        keyname = pattern.replace('_Pt', '') + '_' + subpattern
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
            inDS = ''
            for DS in listToSubmit:
                inDS += '%s,' % DS
            # erase comma in the end again
            if inDS.endswith(','):
                inDS = inDS[:-1]
                
            if len(listToSubmit) > 0:
                config = AnalyseDSName(listToSubmit[0], patternsForMasses)
            else:
                continue
                    
            # now the name of the output dataset does NOT contain the DSID anymore, only the merged pattern
            outDSName = ".".join([DateIntoShortname(now) + ProductionTagNumber, AnalysisName, mergedPattern, config[6], config[5]])
                
            CallFinalSubmitScript(config, inDS, outDSName)
          
    else:  
        for DS in InputDataSets:
            config = AnalyseDSName(DS, patternsForMasses)
            outDSName = ".".join([DateIntoShortname(now) + ProductionTagNumber, AnalysisName, config[3], config[4], config[6], config[5]])
            CallFinalSubmitScript(config, DS, outDSName)
        
    print '\nDONE: All jobs submitted!'
        
