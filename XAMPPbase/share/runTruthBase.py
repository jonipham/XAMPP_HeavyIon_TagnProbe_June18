import argparse

import os
#include the common library instead of importing it. So we have full access to the globals
# Python you're a strange guy sometimes. Sometimes? Alsmost the entire day
include ("XAMPPbase/BaseToolSetup.py")
AssembleIO()
SetupSUSYTruthAnalysisHelper()
ParseTruthOptionsToHelper()
if isData():
    print "You're a son of the sickness. That is a TRUTH AnalysisHelper!!! No TRUTH in data. Truth + Data = baeeh"
    exit(1)

##############################################################################
#        Require that the Basline particles originate from the hard process
##############################################################################
SetupTruthSelector().BaselineHardProcess = True
SetupAlgorithm()
