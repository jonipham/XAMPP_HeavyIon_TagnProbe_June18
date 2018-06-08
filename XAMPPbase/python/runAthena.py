from XAMPPbase.Utils import *
from XAMPPbase.runSUSYAnalysis import  setupSUSYArgParser, AssembleConfigArgument, AssembleAthenaOptions, BringToAthenaStyle
import os
import argparse
import time



if not ATLASPROJECT:
    print ("ERROR: This module is only available in an Athena based release")
    exit(1)


def ExecuteAthena(RunOptions, AthenaArgs):
    if RunOptions.outFile.find("/") != -1:
        print ("Will execute Athena in directory "+RunOptions.outFile.rsplit("/",1)[0])
        os.system("mkdir -p "+RunOptions.outFile.rsplit("/",1)[0])
        os.chdir(RunOptions.outFile.rsplit("/",1)[0])
    if RunOptions.outFile.find("/") == len(RunOptions.outFile) -1:
        print ("ERROR: Please give a file to save not only the directory")
        exit(1)
    ExeCmd = "athena.py %s %s"%(AssembleConfigArgument(AthenaArgs), BringToAthenaStyle(RunOptions.jobOptions))
    os.system(ExeCmd)

if __name__ == "__main__":    
    parser = argparse.ArgumentParser(description='This script starts the analysis code. For more help type \"python XAMPPbase/python/runAthena.py -h\"', prog='runAthena', formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    RunOptions = setupSUSYArgParser(parser).parse_args()
    AthenaArgs = AssembleAthenaOptions(RunOptions,parser)
    ExecuteAthena(RunOptions,AthenaArgs)
   


