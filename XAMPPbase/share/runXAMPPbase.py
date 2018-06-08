import argparse

import os
#include the common library instead of importing it. So we have full access to the globals
# Python you're a strange guy sometimes. Sometimes? Alsmost the entire day
include ("XAMPPbase/BaseToolSetup.py")

##########################################
# Call the AssembleIO function to load the input files from the globals
#  inputFile = "<MyInFile>"
#  nevents = <MyNumOfEvt>
#  nskip = <MyEvToSkip>
#  outFile = "<MyOutFile>"
##########################################
AssembleIO()


####################################################################################
# Optionally:
# Call the SetupSystematicTool() method in order to switch on/off specific objects
####################################################################################
SetupSystematicsTool(noJets=False, noBtag=False, noElectrons=False, noMuons=False, noTaus=True, noDiTaus=True, noPhotons=False)

####################################################################
# Define at some point a trigger list and pass it to the TriggerTool
###################################################################
SingleLepTriggers = [
    "HLT_e24_lhmedium_iloose_L1EM20VH",
    "HLT_e24_lhmedium_iloose_L1EM18VH",
    "HLT_e60_lhmedium",
    "HLT_e26_lhtight_iloose",
    "HLT_e120_lhloose",
    "HLT_mu20_iloose_L1MU15",
    "HLT_mu24_iloose_L1MU15",
    "HLT_mu26_imedium",
    "HLT_mu40",
    "HLT_mu50"]
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
    

#SetupSUSYTriggerTool().SingleLeptonTriggers =  [
#                "HLT_mu6_bJpsi_Trkloose",
#                "HLT_mu6_bJpsi_TrkPEB",
#                "HLT_mu18_2mu0noL1_JpsimumuFS",
#                "HLT_mu18_2mu4noL1_JpsimumuL2",
#                "HLT_mu4_mu4_idperf_bJpsimumu_noid",
#               "HLT_mu20_iloose_L1MU15",
#                "HLT_mu20_ivarloose_L1MU15",
#                "HLT_mu24_iloose_L1MU15",
#                "HLT_mu24_ivarloose_L1MU15",
#                "HLT_mu24_iloose",
#                "HLT_mu24_ivarloose",
#                "HLT_mu24_imedium",
#                "HLT_mu24_ivarmedium",
#                "HLT_mu26_imedium",
#                "HLT_mu26_ivarmedium",
#                "HLT_mu28_imedium",
#                "HLT_mu28_ivarmedium",
#                "HLT_mu40",
#                "HLT_mu50",
#               "HLT_AllTriggers", # this is needed, if you want to have combined (inclusive) efficiency of all HLT triggers
#                "HLT_mu4","HLT_mu6","HLT_mu10","HLT_mu20",
#                "HLT_mu4_idperf","HLT_mu6_idperf","HLT_mu10_idperf","HLT_mu20_idperf",
#                "HLT_mu4_msonly","HLT_mu6_msonly","HLT_mu10_msonly","HLT_mu20_msonly",
#                "HLT_2mu6_bJpsimumu",
#                "HLT_mu6_mu4_bJpsimumu",
#               "HLT_2mu6_bJpsimumui_noL2",
#               "HLT_mu6_mu4_bJpsimumu_noL2",
#                "HLT_noalg_L1MU",
#                "HLT_noalg_L1MU20",
#                "HLT_noalg_L1MU21",
#                "HLT_noalg_L1MU10",
#                "HLT_noalg_L1MU11",
#                ]

SetupSUSYTriggerTool().SingleLeptonTriggers = SingleLepTriggers
SetupSUSYTriggerTool().DiLeptonTriggers = DiLepTriggers
SetupSUSYTriggerTool().TriLeptonTriggers = TriLepTriggers
#from AthenaCommon.Constants import WARNING, DEBUG, VERBOSE
#SetupSUSYTriggerTool().OutputLevel = DEBUG   
#ServiceMgr.MessageSvc.debugLimit = 200000
          

##################################################################################
#  Now setup your analysis helper
##################################################################################
SetupAnalysisHelper()

##################################################################################
#  This function adds all needed selector tools to the AnalysisHelper. If you want
#  to initialize other tools instead call their creation methods before 
##################################################################################
ParseBasicConfigsToHelper()
SetupAnalysisHelper().GoodRunsLists = []
###################################################################################
#  Finally Setup the algorithm to run every thing
##################################################################################
SetupAlgorithm()

